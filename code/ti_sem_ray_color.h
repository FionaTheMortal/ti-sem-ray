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

#endif
