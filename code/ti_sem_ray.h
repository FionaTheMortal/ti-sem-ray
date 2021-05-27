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
	v3f Position;
	v3f ViewDirection;
	s32 ObjectIndex;
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

struct material
{
	v3f DiffuseColor;
	v3f SpecularColor;
	v3f AmbientColor;
	f32 Shininess;
};

struct object
{
	s32 MaterialIndex;
	
	object_type Type;

	union
	{
		sphere Sphere;
		plane  Plane;
	};
};

struct light
{
	v3f Position;
	v3f Intensity;
};

struct scene
{
	v3f BackgroundColor;

	material *Materials;
	s32       MaterialCount;
	s32       MaxMaterialCount;

	object *Objects;
	s32     ObjectCount;
	s32     MaxObjectCount;

	light *Lights;
	s32    LighCount;
	s32    MaxLightCount;
};

enum render_mode
{
	RenderMode_TraceEdgeTransitions = 0,
	RenderMode_RecursiveRaytracing
};

struct render_buffer
{
	s32 DimX;
	s32 DimY;

	v3f *Pixels;
};

struct render_context
{
	s32 Mode;

	const c8 *OutputFilename;

	s32 OutputDimX;
	s32 OutputDimY;

	render_buffer Output;
};

#endif
