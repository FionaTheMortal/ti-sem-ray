#ifndef TI_SEM_RAY_H
#define TI_SEM_RAY_H

struct ray
{
	v3f Pos;
	v3f Dir;
};

struct sphere
{
	v3f Pos;
	f32 Radius;
};

struct plane
{

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

	f32 FilmDimX;
	f32 FilmDimY;
};

#endif