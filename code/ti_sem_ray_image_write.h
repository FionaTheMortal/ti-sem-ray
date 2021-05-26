#ifndef TI_SEM_RAY_IMAGE_WRITE_H
#define TI_SEM_RAY_IMAGE_WRITE_H

#define WRITE_BMP_MIN_DIM (1)
#define WRITE_BMP_MAX_DIM (1 << 15)

#define WRITE_BMP_MIN_DST_REFILL_SIZE (128)
#define WRITE_BMP_MIN_SRC_REFILL_SIZE (32)

enum bmp_compression_mode
{
	BMPCompressionMode_BI_RGB = 0,
	BMPCompressionMode_BI_RLE8,
	BMPCompressionMode_BI_RLE4,
	BMPCompressionMode_BI_BITFIELDS,
	BMPCompressionMode_BI_JPEG,
	BMPCompressionMode_BI_PNG
};

enum bmp_color_space
{
	BMPColorSpace_LCS_CALIBRATED_RGB = 0,
};

#include <Windows.h>

enum write_bmp_state
{
	WriteBMPState_Error = -1,
	WriteBMPState_Done = 0,
	WriteBMPState_Init,
	WriteBMPState_SrcEmpty,
	WriteBMPState_DstFull
};

struct write_bmp
{
	smi DstSizeProcessed;
	smi SrcSizeProcessed;

	s32 DimX;
	s32 DimY;
	s32 SrcChannelCount;
	s32 DstChannelCount;
	s32 DstScanlinePadding;

	s32 State;
	s32 PixelX;
	s32 PixelY;
	b32 AtEOF;

	buffered_reader Src;
	buffered_writer Dst;

	u8 TempDstBuffer[    WRITE_BMP_MIN_DST_REFILL_SIZE];
	u8 TempSrcBuffer[2 * WRITE_BMP_MIN_SRC_REFILL_SIZE];
};

#define WRITE_BMP_CR_RETURN(CRStateIn, RetStateIn) \
	do \
	{ \
		Context->State = CRStateIn; \
		State = RetStateIn; \
		goto WriteBMPCRReturn; \
		case CRStateIn:; \
	} \
	while (0)

#define WRITE_BMP_CR_RETURN_FOREVER(CRState, RetState) \
	for (;;) \
	{ \
		WRITE_BMP_CR_RETURN(CRState, RetState); \
	}

#define WRITE_BMP_CR_BEGIN() \
	switch (Context->State) \
	{ \
		case 0:

#define WRITE_BMP_CR_END() \
	}

#define WRITE_BMP_CR_ERROR() \
	goto WriteBMPCRError;

#define WRITE_BMP_REFILL_DST(CRState) \
	if (Context->Dst.Next >= Context->Dst.Mark) \
	{ \
		while (RefillWriter(&Context->Dst)) \
		{ \
			WRITE_BMP_CR_RETURN(CRState, WriteBMPState_DstFull); \
		} \
	}

#define WRITE_BMP_REFILL_SRC(CRState) \
	if (Context->Src.Next >= Context->Src.Mark) \
	{ \
		if (Context->AtEOF) \
		{ \
			WRITE_BMP_CR_ERROR(); \
		} \
		while (RefillReader(&Context->Src)) \
		{ \
			WRITE_BMP_CR_RETURN(CRState, WriteBMPState_SrcEmpty); \
		} \
	}

internal s32
WriteBMP_GetDstChannelCount(s32 SrcChannelCount)
{
	s32 Result;
	b32 HasAlphaChannel = (SrcChannelCount == 2) || (SrcChannelCount == 4);

	if (HasAlphaChannel)
	{
		Result = 4;
	}
	else
	{
		Result = 3;
	}

	return Result;
}

internal b32
WriteBMPInit(write_bmp *Context, s32 DimX, s32 DimY, s32 ChannelCount)
{
	b32 Success = false;

	ZeroStruct(Context);

	if (DimX >= WRITE_BMP_MIN_DIM &&
		DimX <= WRITE_BMP_MAX_DIM &&
		DimY >= WRITE_BMP_MIN_DIM &&
		DimY <= WRITE_BMP_MAX_DIM &&
		ChannelCount >= 1 &&
		ChannelCount <= 4)
	{
		Success = true;

		Context->DimX = DimX;
		Context->DimY = DimY;
		Context->SrcChannelCount = ChannelCount;
		Context->DstChannelCount = WriteBMP_GetDstChannelCount(ChannelCount);

		s32 DstScanlineSize = Context->DstChannelCount * Context->DimX;
		s32 DstScanlineSizeWithPadding = Align(DstScanlineSize, 4);

		Context->DstScanlinePadding = DstScanlineSizeWithPadding - DstScanlineSize;

		InitBufferedReader(&Context->Src, Context->TempSrcBuffer, SizeOf(Context->TempSrcBuffer), WRITE_BMP_MIN_SRC_REFILL_SIZE);
		InitBufferedWriter(&Context->Dst, Context->TempDstBuffer, SizeOf(Context->TempDstBuffer), WRITE_BMP_MIN_DST_REFILL_SIZE);
	}

	return Success;
}

internal void
WriteBMPSetSrc(write_bmp *Context, const void *Src, smi SrcSize)
{
	SetNextBuffer(&Context->Src, Src, SrcSize);
}

internal void
WriteBMPSetDst(write_bmp *Context, void *Dst, smi DstSize)
{
	SetNextBuffer(&Context->Dst, Dst, DstSize);
}

internal void
WriteBMPExpectEOF(write_bmp *Context)
{
	local const c8 Zeros[2 * WRITE_BMP_MIN_SRC_REFILL_SIZE];
	SetNextBuffer(&Context->Src, Zeros, SizeOf(Zeros));
	Context->AtEOF = true;
}

internal void
WriteBMP_ProcessPixels(write_bmp *Context)
{
	c8 *      Dst     = Context->Dst.Base;
	smi       DstNext = Context->Dst.Next;
	const c8 *Src     = Context->Src.Base;
	smi       SrcNext = Context->Src.Next;

	while (Context->PixelY < Context->DimY)
	{
		s32 DstPixelCount = (Context->Dst.Size - DstNext - Context->DstScanlinePadding) / Context->DstChannelCount;
		s32 SrcPixelCount = (Context->Src.Size - SrcNext)                               / Context->SrcChannelCount;
		s32 MinPixelCount = Min(DstPixelCount, SrcPixelCount);

		if (MinPixelCount > 0)
		{
			s32 ScanlinePixelCount = Context->DimX - Context->PixelX;
			s32 PixelCount = Min(ScanlinePixelCount, SrcPixelCount, DstPixelCount);

			switch (Context->SrcChannelCount)
			{
				case 1:
				{
					for (s32 Index = 0;
						Index < PixelCount;
						++Index, SrcNext += 1)
					{
						Dst[DstNext++] = Src[SrcNext + 0];
						Dst[DstNext++] = Src[SrcNext + 0];
						Dst[DstNext++] = Src[SrcNext + 0];
					}
				} break;
				case 2:
				{
					for (s32 Index = 0;
						Index < PixelCount;
						++Index, SrcNext += 2)
					{
						Dst[DstNext++] = Src[SrcNext + 0];
						Dst[DstNext++] = Src[SrcNext + 0];
						Dst[DstNext++] = Src[SrcNext + 0];
						Dst[DstNext++] = Src[SrcNext + 1];
					}
				} break;
				case 3:
				{
					for (s32 Index = 0;
						Index < PixelCount;
						++Index, SrcNext += 2)
					{
						Dst[DstNext++] = Src[SrcNext + 2];
						Dst[DstNext++] = Src[SrcNext + 1];
						Dst[DstNext++] = Src[SrcNext + 0];
					}
				} break;
				case 4:
				{
					for (s32 Index = 0;
						Index < PixelCount;
						++Index, SrcNext += 4)
					{
						Dst[DstNext++] = Src[SrcNext + 2];
						Dst[DstNext++] = Src[SrcNext + 1];
						Dst[DstNext++] = Src[SrcNext + 0];
						Dst[DstNext++] = Src[SrcNext + 3];
					}
				} break;
			}

			Context->PixelX += PixelCount;

			if (Context->PixelX == Context->DimX)
			{
				for (s32 Byte = 0;
					Byte < Context->DstScanlinePadding;
					++Byte)
				{
					Dst[DstNext++] = 0;
				}

				Context->PixelX = 0;
				Context->PixelY++;
			}
		}
		else
		{
			break;
		}
	}

	Context->Dst.Next = DstNext;
	Context->Src.Next = SrcNext;
}

internal void
WriteBMP_ProcessHeader(write_bmp *Context)
{
	c8 *      Dst     = Context->Dst.Base;
	smi       DstNext = Context->Dst.Next;
	const c8 *Src     = Context->Dst.Base;
	smi       SrcNext = Context->Dst.Next;

	b32 HasAlphaChannel = (Context->SrcChannelCount == 2) || (Context->SrcChannelCount == 4);
	s32 DstChannelCount = Context->DstChannelCount;
	s32 InfoHeaderSize;
	s32 CompressionMode;

	if (HasAlphaChannel)
	{
		InfoHeaderSize = 108;
		CompressionMode = BMPCompressionMode_BI_BITFIELDS;
	}
	else
	{
		InfoHeaderSize = 40;
		CompressionMode = BMPCompressionMode_BI_RGB;
	}

	s32 FileHeaderSize = 14;
	s32 OffsetToBitmap = FileHeaderSize + InfoHeaderSize;

	s32 DstPixelBitCount = 8 * DstChannelCount;

	s32 DstScanlineSize = DstChannelCount * Context->DimX;
	s32 DstScanlineSizeWithPadding = DstScanlineSize + Context->DstScanlinePadding;
	s32 DstBitmapSize = DstScanlineSizeWithPadding * Context->DimY;

	s32 FileSize = OffsetToBitmap + DstBitmapSize;

	Push8(Dst, &DstNext, 'B');
	Push8(Dst, &DstNext, 'M');

	Push32LE(Dst, &DstNext, FileSize);
	Push32LE(Dst, &DstNext, 0);
	Push32LE(Dst, &DstNext, OffsetToBitmap);

	Push32LE(Dst, &DstNext, InfoHeaderSize);
	Push32LE(Dst, &DstNext, Context->DimX);
	Push32LE(Dst, &DstNext, Context->DimY);

	Push16LE(Dst, &DstNext, 1);
	Push16LE(Dst, &DstNext, (u16)DstPixelBitCount);

	Push32LE(Dst, &DstNext, CompressionMode);
	Push32LE(Dst, &DstNext, DstBitmapSize);
	Push32LE(Dst, &DstNext, (u32)(300.0f * 39.3701f));
	Push32LE(Dst, &DstNext, (u32)(300.0f * 39.3701f));
	Push32LE(Dst, &DstNext, 0);
	Push32LE(Dst, &DstNext, 0);

	if (InfoHeaderSize > 40)
	{
		Push32LE(Dst, &DstNext, 0x00FF0000);
		Push32LE(Dst, &DstNext, 0x0000FF00);
		Push32LE(Dst, &DstNext, 0x000000FF);
		Push32LE(Dst, &DstNext, 0xFF000000);

		Push32LE(Dst, &DstNext, BMPColorSpace_LCS_CALIBRATED_RGB);

		s32 RemainingHeaderSize = InfoHeaderSize - 52;

		PushZeros(Dst, &DstNext, RemainingHeaderSize);
	}

	Context->Dst.Next = DstNext;
	Context->Src.Next = SrcNext;
}

internal s32
WriteBMP(write_bmp *Context)
{
	s32 State = WriteBMPState_Error;

	WRITE_BMP_CR_BEGIN();

	WRITE_BMP_REFILL_DST(1);

	WriteBMP_ProcessHeader(Context);

	while (Context->PixelY < Context->DimY)
	{
		WriteBMP_ProcessPixels(Context);

		WRITE_BMP_REFILL_DST(2);
		WRITE_BMP_REFILL_SRC(3);
	}

	WRITE_BMP_CR_RETURN_FOREVER(4, WriteBMPState_Done);

WriteBMPCRError:

	WRITE_BMP_CR_RETURN_FOREVER(5, WriteBMPState_Error);

	WRITE_BMP_CR_END();

WriteBMPCRReturn:

	Context->DstSizeProcessed = GetProcessedSize(&Context->Dst);
	Context->SrcSizeProcessed = GetProcessedSize(&Context->Src);

	return State;
}

internal b32
WriteBMPToFile(const c8 *Filename, const void *Pixels, s32 DimX, s32 DimY, s32 ChannelCount)
{
	b32 Success = false;

	os_file File;

	if (OSOpenFile(&File, Filename, OSFileOp_CreateOrOverwrite))
	{
		write_bmp Context;

		if (WriteBMPInit(&Context, DimX, DimY, ChannelCount))
		{
			smi SrcSize = GetBitmapSize(DimX, DimY, ChannelCount);
			smi DstSize = KILOBYTES(16);

			void *Dst = AllocMemory(DstSize, false);

			WriteBMPSetSrc(&Context, Pixels, SrcSize);
			WriteBMPSetDst(&Context, Dst, DstSize);

			b32 WriteSuccess = true;
			b32 EndOfFile = false;
			s32 State = WriteBMPState_Init;

			while (State > WriteBMPState_Done && WriteSuccess)
			{
				State = WriteBMP(&Context);

				switch (State)
				{
					case WriteBMPState_SrcEmpty:
					{
						WriteBMPExpectEOF(&Context);
					} break;
					case WriteBMPState_DstFull:
					case WriteBMPState_Done:
					{
						WriteSuccess = OSWriteFile(&File, Dst, Context.DstSizeProcessed, OS_FILE_OFFSET_APPEND);

						WriteBMPSetDst(&Context, Dst, DstSize);
					} break;
				}
			}

			if (State == WriteBMPState_Done && WriteSuccess)
			{
				Success = true;
			}

			FreeMemory(Dst);
		}
	}

	return Success;
}

#endif
