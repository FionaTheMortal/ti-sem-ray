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
#include "ti_sem_ray_memory.h"
#include "ti_sem_ray_memory_stream.h"
#include "ti_sem_ray_os.h"
#include "ti_sem_ray_image.h"
#include "ti_sem_ray_image_write.h"
#include "ti_sem_ray_color.h"
#include "ti_sem_ray.h"

struct calendar_time
{
	s32 Second;
	s32 Minute;
	s32 Hour;
	s32 Day;
	s32 Month;
	s32 Year;
};

internal calendar_time
GetCurrentLocalCalendarTime()
{
	time_t Now = time(NULL);

	tm *LocalTime = localtime(&Now);

	calendar_time Result;

	Result.Second = LocalTime->tm_sec;
	Result.Minute = LocalTime->tm_min;
	Result.Hour = LocalTime->tm_hour;
	Result.Day = LocalTime->tm_mday;
	Result.Month = LocalTime->tm_mon + 1;
	Result.Year = LocalTime->tm_year + 1900;

	return Result;
}

internal s32
FormatFilenameTimestamp(c8 *Buffer, s32 BufferSize, calendar_time *Time)
{
	s32 Result = sprintf_s(Buffer, BufferSize, "%d-%02d-%02d-%02d%02d%02d", Time->Year, Time->Month, Time->Day, Time->Hour, Time->Minute, Time->Second);

	return Result;
}

internal void
CameraLookAt(camera *Camera, v3f Pos, v3f Up)
{

}

internal f32
GetCameraAspectRatio(const camera *Camera)
{
	f32 Result = ((f32)Camera->ResolutionY / (f32)Camera->ResolutionX);
	
	return Result;
}

internal void
UpdateCamera(camera *Camera)
{
	Camera->Basis = BasisFromQuaternion(Camera->Orientation);

	Camera->VirtualScreenDimX = 2.0f * Tan(Camera->HorizontalFOV / 2.0f);
	Camera->VirtualScreenDimY = GetCameraAspectRatio(Camera) *  Camera->VirtualScreenDimX;
}

internal v3f
CameraSpacePosFromVirtualScreenPos(const camera *Camera, f32 X, f32 Y)
{
	f32 OffsetX = (X / (f32)Camera->ResolutionX) - 0.5f;
	f32 OffsetY = (Y / (f32)Camera->ResolutionY) - 0.5f;

	v3f Result = 
		Camera->Basis.X + 
		Camera->Basis.Y * (-OffsetX) + 
		Camera->Basis.Z *  OffsetY;

	return Result;
}

internal ray
RayFromVirtualScreenPos(const camera *Camera, f32 X, f32 Y)
{
	ray Result;
	
	v3f FilmPos = CameraSpacePosFromVirtualScreenPos(Camera, X, Y);

	Result.Pos = Camera->Pos;
	Result.Dir = NOZ(FilmPos - Camera->Pos);

	return Result;
}

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

internal bitmap
AllocBitmap(s32 DimX, s32 DimY)
{
	bitmap Result;

	s32 PixelsSize = 4 * DimX * DimY;
	u8 *Pixels = AllocArray(u8, PixelsSize, true);

	Result.DimX = DimX;
	Result.DimY = DimY;
	Result.Pixels = Pixels;

	return Result;
}

internal void
BitmapWriteRGBA8(bitmap *Bitmap, s32 IndexX, s32 IndexY, u32 Color)
{
	Bitmap->Pixels[4 * (Bitmap->DimX * IndexY + IndexX) + 0] = RGBA8GetR(Color);
	Bitmap->Pixels[4 * (Bitmap->DimX * IndexY + IndexX) + 1] = RGBA8GetG(Color);
	Bitmap->Pixels[4 * (Bitmap->DimX * IndexY + IndexX) + 2] = RGBA8GetB(Color);
	Bitmap->Pixels[4 * (Bitmap->DimX * IndexY + IndexX) + 3] = RGBA8GetA(Color);
}

internal void
RenderSceneByTracingScanlineEdgeTransitions(bitmap *Image, scene *Scene, camera *Camera)
{
	s32 PrevHitObjectIndex = 0;

	for (s32 IndexY = 0;
		IndexY < Image->DimX;
		++IndexY)
	{
		for (s32 IndexX = 0;
			IndexX < Image->DimY;
			++IndexX)
		{
			f32 SampleX = (f32)IndexX + 0.5f;
			f32 SampleY = (f32)IndexY + 0.5f;

			ray Ray = RayFromVirtualScreenPos(Camera, SampleX, SampleY);

			hit Hit = IntersectRayScene(&Ray, Scene);

			u32 Color;

			if (Hit.ObjectIndex != PrevHitObjectIndex)
			{
				Color = RGBA8(0x00, 0x00, 0x00, 0xFF);
			}
			else
			{
				Color = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
			}

			BitmapWriteRGBA8(Image, IndexX, IndexY, Color);

			PrevHitObjectIndex = Hit.ObjectIndex;
		}
	}
}

int
main(int argc, char **argv)
{
	s32 ImageDimX = 512;
	s32 ImageDimY = 512;

	const c8 *Filename = "ray_out";
	const c8 *FileExtension = ".bmp";

	bitmap Image = AllocBitmap(ImageDimX, ImageDimY);

	object Objects[32] = {};

	scene Scene = {};
	Scene.Objects = Objects;
	Scene.ObjectCount = 1;
	Scene.MaxObjectCount = ArrayCount(Objects);

	AddSphere(&Scene, V3F(4.0f, 0.0f, 0.0f), 1.0f);
	AddSphere(&Scene, V3F(3.0f, 0.0f, 1.0f), 0.5f);

	camera Camera = {};
	Camera.Orientation = QuaternionIdentity();
	Camera.Pos = V3F(0, 0, 0);
	Camera.HorizontalFOV = 90.0f;
	Camera.ResolutionX = ImageDimX;
	Camera.ResolutionY = ImageDimY;

	UpdateCamera(&Camera);

	RenderSceneByTracingScanlineEdgeTransitions(&Image, &Scene, &Camera);

	c8 Timestamp[64];
	c8 FullFilename[128];

	calendar_time Now = GetCurrentLocalCalendarTime();

	FormatFilenameTimestamp(Timestamp, SizeOf(Timestamp), &Now);

	sprintf_s(FullFilename, SizeOf(FullFilename), "%s-%s%s", Filename, Timestamp, FileExtension);

	WriteBMPToFile(FullFilename, Image.Pixels, Image.DimX, Image.DimY, 4);
}
