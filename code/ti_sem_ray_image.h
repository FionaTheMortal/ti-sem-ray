#ifndef TI_SEM_RAY_IMAGE_H
#define TI_SEM_RAY_IMAGE_H

#include <stdio.h>

enum bmp_compression_mode
{
	BMPCompressionMode_BI_RGB,
	BMPCompressionMode_BI_RLE8,
	BMPCompressionMode_BI_RLE4,
	BMPCompressionMode_BI_BITFIELDS,
	BMPCompressionMode_BI_JPEG,
	BMPCompressionMode_BI_PNG
};

internal void
BMPWritePixels(const void *Source, void *Dest, u32 SourceChannelCount, u32 DestChannelCount, s32 DimX, s32 DimY, s32 ScanlinePadding)
{
	const u8 *SourceByte = (const u8 *)Source;
	u8       *DestByte   = (      u8 *)Dest;

#define CHANNEL_COMBINATION(S, D) (((S) - 1) * 4 + ((D) - 1))
#define CHANNEL_CASE(S, D) \
	case CHANNEL_COMBINATION(S, D): \
		for (s32 PixelIndex = 0; \
			PixelIndex < DimX; \
			++PixelIndex, SourceByte += S, DestByte += D)

	for (s32 ScanlineIndex = 0;
		ScanlineIndex < DimY;
		++ScanlineIndex)
	{
		switch (CHANNEL_COMBINATION(SourceChannelCount, DestChannelCount))
		{
			CHANNEL_CASE(4, 4)
			{
				DestByte[0] = SourceByte[2];
				DestByte[1] = SourceByte[1];
				DestByte[2] = SourceByte[0];
				DestByte[3] = SourceByte[3];
			} break;
			CHANNEL_CASE(3, 3)
			{
				DestByte[0] = SourceByte[2];
				DestByte[1] = SourceByte[1];
				DestByte[2] = SourceByte[0];
			} break;
			CHANNEL_CASE(2, 4)
			{
				DestByte[0] = SourceByte[0];
				DestByte[1] = SourceByte[0];
				DestByte[2] = SourceByte[0];
				DestByte[3] = SourceByte[1];
			} break;
			CHANNEL_CASE(1, 3)
			{
				DestByte[0] = SourceByte[0];
				DestByte[1] = SourceByte[0];
				DestByte[2] = SourceByte[0];
			} break;
			default:
			{
				// TODO: Error
			} break;
		}

		for (s32 Byte = 0;
			Byte < ScanlinePadding;
			++Byte)
		{
			*DestByte++ = 0;
		}
	}
#undef CHANNEL_COMBINATION
#undef CHANNEL_CASE
}

internal b32
WriteBMP(const c8 *Filename, const void *Pixels, s32 DimX, s32 DimY, s32 ChannelCount)
{
	// TODO: These shouldn't be macros

#define PUSH_8(Value) \
	(*FileNext++ = (Value))

#define PUSH_16_LE(Value) \
	(WriteU16LE(FileNext, (Value)), FileNext += SizeOf(u16))

#define PUSH_32_LE(Value) \
	(WriteU32LE(FileNext, (Value)), FileNext += SizeOf(u32))

	b32 Success = false;

	if (Pixels &&
		DimX <= (1 << 15) &&
		DimY <= (1 << 15) &&
		ChannelCount > 0 && 
		ChannelCount <= 4)
	{
		b32 HasAlphaChannel = !(ChannelCount & 1);
		u32 InfoHeaderSize;
		u32 OutputChannelCount;
		u32 CompressionMode;

		if (!HasAlphaChannel)
		{
			InfoHeaderSize = 40;
			OutputChannelCount = 3;
			CompressionMode = BMPCompressionMode_BI_RGB;
		}
		else
		{
			InfoHeaderSize = 108;
			OutputChannelCount = 4;
			CompressionMode = BMPCompressionMode_BI_BITFIELDS;
		}

		u32 FileHeaderSize = 14;
		u32 OffsetToBitmap = FileHeaderSize + InfoHeaderSize;

		u32 OutputScanlineSize = OutputChannelCount * DimX;
		u32 OutputPaddedScanlineSize = Align(OutputScanlineSize, 4);
		s32 OutputScanlinePadding = OutputPaddedScanlineSize - OutputScanlineSize;
		u32 OutputBitmapSize = DimY * OutputPaddedScanlineSize;

		u32 FileSize = OffsetToBitmap + OutputBitmapSize;
		u8 *FileMemory = AllocArray(u8, FileSize, false);
		u8 *FileNext = FileMemory;

		PUSH_8('B');
		PUSH_8('M');
		PUSH_32_LE(FileSize);
		PUSH_32_LE(0);
		PUSH_32_LE(OffsetToBitmap);

		PUSH_32_LE(InfoHeaderSize);
		PUSH_32_LE(DimX);
		PUSH_32_LE(DimY);
		PUSH_16_LE(1);
		PUSH_16_LE((u16)(8 * OutputChannelCount));
		PUSH_32_LE(CompressionMode);
		PUSH_32_LE(OutputBitmapSize);
		PUSH_32_LE((u32)(300.0f * 39.3701f));
		PUSH_32_LE((u32)(300.0f * 39.3701f));
		PUSH_32_LE(0);
		PUSH_32_LE(0);

		if (InfoHeaderSize > 40)
		{
			PUSH_32_LE(0x00FF0000);
			PUSH_32_LE(0x0000FF00);
			PUSH_32_LE(0x000000FF);
			PUSH_32_LE(0xFF000000);

			u32 RemainingHeaderSize = InfoHeaderSize - 56;

			ZeroMemory(FileNext, RemainingHeaderSize);

			FileNext += RemainingHeaderSize;
		}

		BMPWritePixels(
			Pixels,
			FileMemory + OffsetToBitmap,
			ChannelCount,
			OutputChannelCount,
			DimX,
			DimY,
			OutputScanlinePadding);

		FILE *OutputFile = fopen(Filename, "wb");
		
		if (OutputFile)
		{
			fwrite(FileMemory, FileSize, 1, OutputFile);
			fclose(OutputFile);

			Success = true;
		}
		else
		{
			// TODO: Error
		}
	}

#undef PUSH_8
#undef PUSH_16_LE
#undef PUSH_32_LE

	return Success;
}

#endif
