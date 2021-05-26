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

struct scene
{
	object *Objects;
	s32     ObjectCount;
	s32     MaxObjectCount;
};

enum render_mode
{
	RenderMode_TraceEdgeTransitions = 0,
};

struct render_context
{
	s32 Mode;

	const c8 *OutputFilename;

	s32 OutputDimX;
	s32 OutputDimY;

	bitmap Output;
};

#endif
