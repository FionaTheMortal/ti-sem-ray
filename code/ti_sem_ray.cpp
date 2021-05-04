// NOTES:
// 
// Screen and image coordinates are interpreted as
// X from left to right
// Y from bottom to top
// So (0, 0) is the bottom left corner

#include "ti_sem_ray_def.h"
#include "ti_sem_ray_math.h"

#include "ti_sem_ray.h"

internal f32
CameraAspectRatio(const camera *Camera)
{
	f32 Result = ((f32)Camera->ResolutionY / (f32)Camera->ResolutionX);

	return Result;
}

internal void
UpdateCamera(camera *Camera)
{
	Camera->Basis = BasisFromQuaternion(Camera->Orientation);

	Camera->FilmDimX = 2.0f * Tan(Camera->HorizontalFOV / 2.0f);
	Camera->FilmDimY = CameraAspectRatio(Camera) *  Camera->FilmDimX;
}

internal v3f
WorldPosFromFilmPos(const camera *Camera, f32 X, f32 Y)
{
	f32 OffsetX = (X / (f32)Camera->ResolutionX) - 0.5f;
	f32 OffsetY = (Y / (f32)Camera->ResolutionY) - 0.5f;

	v3f Result = 
		Camera->Basis.X * 1 + 
		Camera->Basis.Y * (-OffsetX) + 
		Camera->Basis.Z * OffsetY;

	return Result;
}

internal ray
RayFromFilmPos(const camera *Camera, f32 X, f32 Y)
{
	ray Result;
	
	v3f WorldPos = WorldPosFromFilmPos(Camera, X, Y);

	Result.Pos = Camera->Pos;
	Result.Dir = NOZ(WorldPos - Camera->Pos);

	return Result;
}

int
main(int argc, char *argv)
{
	camera Camera = {};
	Camera.Orientation = QuaternionIdentity();
	Camera.Pos = V3F(0, 0, 0);
	Camera.HorizontalFOV = 90.0f;
	Camera.ResolutionX = 512;
	Camera.ResolutionY = 512;

	UpdateCamera(&Camera);

	for (s32 IndexY = 0;
		IndexY < Camera.ResolutionY;
		++IndexY)
	{
		for (s32 IndexX = 0;
			IndexX < Camera.ResolutionX;
			++IndexX)
		{
			f32 SampleX = (f32)IndexX + 0.5f;
			f32 SampleY = (f32)IndexY + 0.5f;

			ray Ray = RayFromFilmPos(&Camera, SampleX, SampleY);


		}
	}
}