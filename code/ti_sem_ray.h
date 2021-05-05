#ifndef TI_SEM_RAY_H
#define TI_SEM_RAY_H

struct ray
{
	v3f Pos;
	v3f Dir;
};

struct hit
{
	b32 Hit;
	v3f Normal;
	f32 T;
	s32 ObjectIndex;
};

struct material
{

};

struct sphere
{
	v3f Pos;
	f32 Radius;
};

struct plane
{
	v3f Normal;
	f32 Distance;
};

enum object_type
{
	ObjectType_Sphere = 0,
	ObjectType_Plane,
};

struct object
{
	object_type Type;

	union
	{
		sphere Sphere;
		plane  Plane;
	};
};

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

struct scene
{
	object *Objects;
	s32     ObjectCount;
	s32     MaxObjectCount;
};

struct bitmap
{
	s32 DimX;
	s32 DimY;
	u8 *Pixels;
};

#endif