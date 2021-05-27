#ifndef TI_SEM_RAY_CAMERA_H
#define TI_SEM_RAY_CAMERA_H

struct camera
{
	quaternion Orientation;

	v3f Pos;

	f32 HorizontalFOV;

	s32 ResolutionX;
	s32 ResolutionY;

	basis Basis;

	f32 VirtualScreenDimX;
	f32 VirtualScreenDimY;
};

internal void
CameraLookAt(camera *Camera, v3f Pos, v3f Up)
{
	v3f Delta = Pos - Camera->Pos;
	v3f Dir = NOZ(Delta);

	Camera->Orientation = QuaternionLookAt(Dir, Up);
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
	Camera->VirtualScreenDimY = GetCameraAspectRatio(Camera) * Camera->VirtualScreenDimX;
}

internal v3f
CameraSpacePosFromVirtualScreenPos(const camera *Camera, f32 X, f32 Y)
{
	f32 OffsetX = (X / (f32)Camera->ResolutionX) - 0.5f;
	f32 OffsetY = (Y / (f32)Camera->ResolutionY) - 0.5f;

	v3f Result =
		Camera->Basis.X +
		Camera->Basis.Y * (-OffsetX) +
		Camera->Basis.Z * OffsetY;

	return Result;
}

internal ray
RayFromVirtualScreenPos(const camera *Camera, f32 X, f32 Y)
{
	ray Result;

	v3f FilmPos = CameraSpacePosFromVirtualScreenPos(Camera, X, Y);

	Result.Pos = Camera->Pos;
	Result.Dir = NOZ(FilmPos);

	return Result;
}

#endif
