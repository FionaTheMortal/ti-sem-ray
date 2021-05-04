#include "ti_sem_ray_def.h"
#include "ti_sem_ray_math.h"

#include "ti_sem_ray.h"

internal void
UpdateCamera(camera *Camera)
{
	Camera->Basis = BasisFromQuaternion(Camera->Orientation);

	Camera->FilmDimX = 2.0f * Tan(Camera->HorizontalFOV / 2.0f);
	Camera->FilmDimY = (Camera->ResolutionY / Camera->ResolutionX) *  Camera->FilmDimX;
}

internal v3f
FilmToWorldPos(camera *Camera, f32 X, f32 Y)
{
	f32 OffsetX = (X / Camera->ResolutionX) - 0.5f;
	f32 OffsetY = (Y / Camera->ResolutionY) - 0.5f;

	v3f Result = 
		Camera->Basis.Y * (-OffsetX) + 
		Camera->Basis.Z *   OffsetY;

	return Result;
}

int
main(int argc, char *argv)
{
	camera Camera = {};
	Camera.Orientation = QuaternionIdentity();
	Camera.Pos = V3F(0, 0, 0);
	Camera.HorizontalFOV = 103.0f;

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

			v3f PixelPosition = FilmToWorldPos(&Camera, SampleX, SampleY);
		}
	}

	v3f x = V3F(1.0f, 0.0f, 0.0f);
	v3f y = V3F(0.0f, 1.0f, 0.0f);
	v3f z = V3F(0.0f, 0.0f, 1.0f);

	v3f p = V3F(0.0f, 0.0f, 0.0f);

	f32 horrizontal_fov = 103.0f;
	f32 horrizontal_fov_rad = DegToRad(horrizontal_fov);

	s32 i = 0;
	s32 j = 0;

	f32 pos_x = Tan(horrizontal_fov_rad / 2.0f) * ((i + 0.5f) / 1920.0f - 0.5f);
	f32 pos_y = Tan(horrizontal_fov_rad / 2.0f) * ((j + 0.5f) / 1080.0f - 0.5f) * (1080.0f / 1920.0f);

	v3f p_pos = -pos_x * y + pos_y * z + x;
	v3f p_ray = NOZ(p_pos - p);
}