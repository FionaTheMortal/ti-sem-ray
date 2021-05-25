#ifndef TI_SEM_RAY_IMAGE_H
#define TI_SEM_RAY_IMAGE_H

internal smi
GetBitmapSize(smi DimX, smi DimY, s32 ChannelCount)
{
	smi Result = DimX * DimY * ChannelCount;

	return Result;
}

#endif
