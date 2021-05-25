#ifndef TI_SEM_RAY_IMAGE_H
#define TI_SEM_RAY_IMAGE_H

struct bitmap
{
	s32 ChannelCount;
	s32 DimX;
	s32 DimY;
	c8 *Pixels;
};

internal bitmap
AllocBitmap(s32 DimX, s32 DimY, s32 ChannelCount)
{
	bitmap Result;

	s32 PixelsSize = ChannelCount * DimX * DimY;
	c8 *Pixels = AllocArray(c8, PixelsSize, true);

	Result.ChannelCount = ChannelCount;
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

internal smi
GetBitmapSize(smi DimX, smi DimY, s32 ChannelCount)
{
	smi Result = DimX * DimY * ChannelCount;

	return Result;
}

#endif
