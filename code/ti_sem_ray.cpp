// ti_sem_ray by fiona tessmann - public domain, 2021-05
//
// A simple ray tracer written for educational purposes
// 
// NOTE:
// 
// Screen and image coordinates are such that (0, 0) is in the bottom left corner and
//    X increases from left to right
//    Y increases from bottom to top
// 
// For 3D space we use the following conventions:
// 
// 1. We use a right-handed coordinate system where
//    X is forward
//    Y is left
//    Z is up
// 
// 2. Rotations around an axis use the right hand rule.
//
// 3. Rotations are interpreted as active rotations.
//
// 4. Euler angles use yaw, pitch and roll Tait–Bryan angles, so when stored as a vector
//    X is roll
//    Y is yaw
//    Z is pitch
//    and the sequence of rotations is z-y'-x'' in intrinsic rotations

#include <time.h>

#include "ti_sem_ray_def.h"
#include "ti_sem_ray_math.h"
#include "ti_sem_ray_random.h"
#include "ti_sem_ray_memory.h"
#include "ti_sem_ray_memory_stream.h"
#include "ti_sem_ray_os.h"
#include "ti_sem_ray_time.h"
#include "ti_sem_ray_color.h"
#include "ti_sem_ray_image.h"
#include "ti_sem_ray_image_write.h"

#include "ti_sem_ray.h"
#include "ti_sem_ray_camera.h"

// -- Intersection functions. These are based on algorithms given in 'Real-Time Collision Detection' by Christer Ericson

internal f32
IntersectRayPlane(const ray *Ray, const plane *Plane)
{
	f32 ProjectedRayDir = Inner(Plane->Normal, Ray->Dir);
	f32 ProjectedRayPos = Inner(Plane->Normal, Ray->Pos);

	// NOTE: Assuming IEEE-754 floating-point arithmetic a division by 0 gives infinity which can be handled later

	f32 Result = (Plane->Distance - ProjectedRayPos) / ProjectedRayDir;

	return Result;
}

internal f32
IntersectRayShere(const ray *Ray, const sphere *Sphere)
{
	f32 Result;

	v3f M = Ray->Pos - Sphere->Pos;

	f32 B = Inner(M, Ray->Dir);
	f32 C = LengthSqr(M) - Sphere->Radius * Sphere->Radius;

	// NOTE: If the ray starts inside the sphere this generates a false collison at t < 0

	f32 Discriminant = B * B - C;

	if (Discriminant >= 0.0f)
	{
		Result = -B - Sqrt(Discriminant);
	}
	else
	{
		Result = 0.0f;
	}

	return Result;
}

internal f32
IntersectRayObject(const ray *Ray, const object *Object)
{
	f32 Result = 0.0f;

	switch (Object->Type)
	{
		case ObjectType_Sphere:
		{
			Result = IntersectRayShere(Ray, &Object->Sphere);
		} break;
		case ObjectType_Plane:
		{
			Result = IntersectRayPlane(Ray, &Object->Plane);
		} break;
		default:
		{
			// TODO: Error! This should never happen.
		}
	}

	return Result;
}

internal v3f
ObjectNormalAtClosestPoint(const object *Object, v3f Point)
{
	v3f Result = {};

	switch (Object->Type)
	{
		case ObjectType_Sphere:
		{
			Result = NOZ(Point - Object->Sphere.Pos);
		} break;
		case ObjectType_Plane:
		{
			Result = Object->Plane.Normal;
		} break;
		default:
		{
			// TODO: Error! This should never happen.
		}
	}

	return Result;
}

internal hit
IntersectRayScene(const ray *Ray, const scene *Scene)
{
	hit Result = {};

	f32 MinT = F32_INF;
	s32 ObjectIndex = 0;

	for (s32 Index = 1;
		Index < Scene->ObjectCount;
		++Index)
	{
		object *Object = &Scene->Objects[Index];

		f32 T = IntersectRayObject(Ray, Object);

		if (T > 0.0f && T < MinT)
		{
			MinT = T;
			ObjectIndex = Index;
		}
	}

	if (MinT > 0.0f)
	{
		v3f HitPos = Ray->Pos + MinT * Ray->Dir;

		Result.Hit = true;
		Result.Normal = ObjectNormalAtClosestPoint(&Scene->Objects[ObjectIndex], HitPos);
		Result.T = MinT;
		Result.ObjectIndex = ObjectIndex;
	}

	return Result;
}

internal void
AddSphere(scene *Scene, v3f Pos, f32 Radius)
{
	if (Scene->ObjectCount < Scene->MaxObjectCount)
	{
		object *NewObject = &Scene->Objects[Scene->ObjectCount++];

		NewObject->Type = ObjectType_Sphere;
		NewObject->Sphere.Pos = Pos;
		NewObject->Sphere.Radius = Radius;
	}
}

internal void
AddPlane(scene *Scene, v3f Normal, f32 Distance)
{
	if (Scene->ObjectCount < Scene->MaxObjectCount)
	{
		object *NewObject = &Scene->Objects[Scene->ObjectCount++];

		NewObject->Type = ObjectType_Plane;
		NewObject->Plane.Normal = Normal;
		NewObject->Plane.Distance = Distance;
	}
}

internal c8 *
FormatDefaultOutputFilename(c8 *Buffer, smi BufferSize)
{
	c8 Timestamp[128];

	c8 *Filename = "ray_out";
	c8 *FileExtension = ".bmp";

	calendar_time Now = GetLocalCalendarTime();

	FormatFilenameTimestamp(Timestamp, SizeOf(Timestamp), &Now);

	sprintf_s(Buffer, BufferSize, "%s-%s%s", Filename, Timestamp, FileExtension);

	return Buffer;
}

internal render_buffer
AllocRenderBuffer(s32 DimX, s32 DimY)
{
	render_buffer Result = {};

	Result.DimX = DimX;
	Result.DimY = DimY;
	Result.Pixels = AllocArray(v3f, DimX * DimY, true);

	return Result;
}

internal void
RenderBufferSetPixel(render_buffer *Buffer, s32 X, s32 Y, v3f Color)
{
	Buffer->Pixels[X + Y * Buffer->DimX] = Color;
}

internal v3f
RenderBufferGetPixel(render_buffer *Buffer, s32 X, s32 Y)
{
	v3f Result = Buffer->Pixels[X + Y * Buffer->DimX];

	return Result;
}

internal void
TraceEdgeTransitions(render_context *Context, scene *Scene, camera *Camera)
{
	render_buffer *Output = &Context->Output;

	s32 PrevHitObjectIndex = 0;

	for (s32 IndexY = 0;
		IndexY < Output->DimX;
		++IndexY)
	{
		for (s32 IndexX = 0;
			IndexX < Output->DimY;
			++IndexX)
		{
			f32 SampleX = (f32)IndexX + 0.5f;
			f32 SampleY = (f32)IndexY + 0.5f;

			ray Ray = RayFromVirtualScreenPos(Camera, SampleX, SampleY);

			hit Hit = IntersectRayScene(&Ray, Scene);

			v3f Color;

			if (Hit.ObjectIndex != PrevHitObjectIndex)
			{
				Color = V3F(0, 0, 0);
			}
			else
			{
				Color = V3F(1, 1, 1);
			}

			RenderBufferSetPixel(Output, IndexX, IndexY, Color);

			PrevHitObjectIndex = Hit.ObjectIndex;
		}
	}
}


internal void
ResolveRenderBufferToBitmap(render_buffer *Buffer, bitmap *Result, b32 ConvertTosRGB)
{
	for (s32 Y = 0;
		Y < Result->DimY;
		++Y)
	{
		for (s32 X = 0;
			X < Result->DimX;
			++X)
		{
			v3f Color = RenderBufferGetPixel(Buffer, X, Y);

			if (ConvertTosRGB)
			{
				Color = LinearTosRGB(Color);
			}

			u8 R = (u8)(255.0f * Color.R);
			u8 G = (u8)(255.0f * Color.G);
			u8 B = (u8)(255.0f * Color.B);
			u8 A = 0xFF;

			u32 Color8 = RGBA8(R, G, B, A);

			BitmapWriteRGBA8(Result, X, Y, Color8);
		}
	}
}

int
main(int argc, char **argv)
{
	render_context Context = {};
	Context.Mode = RenderMode_TraceEdgeTransitions;
	Context.OutputDimX = 512;
	Context.OutputDimY = 512;
	Context.OutputFilename = 0;
	Context.Output = AllocRenderBuffer(Context.OutputDimX, Context.OutputDimY);

	object Objects[32] = {};

	scene Scene = {};
	Scene.Objects = Objects;
	Scene.ObjectCount = 1;
	Scene.MaxObjectCount = ArrayCount(Objects);

	AddSphere(&Scene, V3F(4.0f, 0.0f, 0.0f), 1.0f);
	AddSphere(&Scene, V3F(3.0f, 0.0f, 1.0f), 0.5f);

	camera Camera = {};
	Camera.Pos = V3F(0, 0, 0);
	Camera.HorizontalFOV = 90.0f;
	Camera.ResolutionX = Context.OutputDimX;
	Camera.ResolutionY = Context.OutputDimX;

	v3f CameraFocus = V3F(1.0f, 0.0f, 0.5f);
	v3f CameraUp = V3F(0.0f, 0.0f, 1.0f);

	CameraLookAt(&Camera, CameraFocus, CameraUp);
	UpdateCamera(&Camera);

	switch (Context.Mode)
	{
		case RenderMode_TraceEdgeTransitions:
		{
			TraceEdgeTransitions(&Context, &Scene, &Camera);
		} break;
	}

	c8 FilenameBuffer[256];

	if (!Context.OutputFilename)
	{
		Context.OutputFilename = FormatDefaultOutputFilename(FilenameBuffer, SizeOf(FilenameBuffer));
	}

	bitmap Output = AllocBitmap(Context.OutputDimX, Context.OutputDimY, 4);

	ResolveRenderBufferToBitmap(&Context.Output, &Output, true);

	WriteBMPToFile(Context.OutputFilename, Output.Pixels, Output.DimX, Output.DimY, Output.ChannelCount);
}
