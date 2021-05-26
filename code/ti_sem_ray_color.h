#ifndef TI_SEM_RAY_COLOR_H
#define TI_SEM_RAY_COLOR_H

internal u32
RGBA8(u8 R, u8 G, u8 B, u8 A)
{
	u32 Result =
		((u32)R << 0) |
		((u32)G << 8) |
		((u32)B << 16) |
		((u32)A << 24);

	return Result;
}

internal u8
RGBA8GetR(u32 Value)
{
	u8 Result = Value & 0xFF;

	return Result;
}

internal u8
RGBA8GetG(u32 Value)
{
	u8 Result = (Value >> 8) & 0xFF;

	return Result;
}

internal u8
RGBA8GetB(u32 Value)
{
	u8 Result = (Value >> 16) & 0xFF;

	return Result;
}

internal u8
RGBA8GetA(u32 Value)
{
	u8 Result = (Value >> 24) & 0xFF;

	return Result;
}

internal f32
LinearTosRGB(f32 Color)
{
	f32 Result;

	if (Color <= 0.00313066844250063f)
	{
		Result = Color * 12.92f;
	}
	else
	{
		Result = 1.055f * Pow(Color, 1.0f/2.4f) - 0.055f;
	}

	return Result;
}

internal f32
sRGBToLinear(f32 Color)
{
	f32 Result;

	if (Color <= 0.0404482362771082f)
	{
		Result = Color / 12.92f;
	}
	else
	{
		Result = Pow((Color + 0.055f) / 1.055f, 2.4f);
	}

	return Result;
}

internal v3f
LinearTosRGB(v3f Color)
{
	v3f Result;

	Result.R = LinearTosRGB(Color.R);
	Result.G = LinearTosRGB(Color.G);
	Result.B = LinearTosRGB(Color.B);

	return Result;
}

internal v3f
sRGBToLinear(v3f Color)
{
	v3f Result;

	Result.R = sRGBToLinear(Color.R);
	Result.G = sRGBToLinear(Color.G);
	Result.B = sRGBToLinear(Color.B);

	return Result;
}

#endif
