#ifndef TI_SEM_RAY_MEMORY_H
#define TI_SEM_RAY_MEMORY_H

#include <malloc.h>
#include <string.h>

#define AllocArray(type, Count, ZeroInit) ((type *)AllocMemory(SizeOf(type) * (Count), ZeroInit))

internal void
ZeroMemory(void *Ptr, smi Size)
{
	memset(Ptr, 0, Size);
}

internal void *
AllocMemory(smi Size, b32 ZeroInit)
{
	void *Result = malloc(Size);

	if (ZeroInit && Result)
	{
		ZeroMemory(Result, Size);
	}

	return Result;
}

internal void
CopyMemory(const void *Source, umi Size, void *Dest)
{
	memcpy(Dest, Source, Size);
}

internal u16
ByteSwap(u16 Value)
{
	u16 Result =
		((0xFF & Value) << 8) |
		(0xFF & (Value >> 8));

	return Result;
}

internal u32
ByteSwap(u32 Value)
{
	u32 Result =
		((0xFF & Value) << 24) |
		(0xFF & (Value >> 24)) |
		((0xFF00 & Value) << 8) |
		(0xFF00 & (Value >> 8));

	return Result;
}

internal u16
NativeToLE(u16 Value)
{
	u16 Result;

	if (CPU_IS_LITTLE_ENDIAN())
	{
		Result = Value;
	}
	else
	{
		Result = ByteSwap(Value);
	}

	return Result;
}

internal u32
NativeToLE(u32 Value)
{
	u32 Result;

	if (CPU_IS_LITTLE_ENDIAN())
	{
		Result = Value;
	}
	else
	{
		Result = ByteSwap(Value);
	}

	return Result;
}

internal void
WriteU16(void *Pointer, u16 Value)
{
	CopyMemory(&Value, SizeOf(Value), Pointer);
}

internal void
WriteU32(void *Pointer, u32 Value)
{
	CopyMemory(&Value, SizeOf(Value), Pointer);
}

internal void
WriteU16LE(void *Pointer, u16 Value)
{
	u16 ValueLE = NativeToLE(Value);
	WriteU16(Pointer, ValueLE);
}

internal void
WriteU32LE(void *Pointer, u32 Value)
{
	u32 ValueLE = NativeToLE(Value);
	WriteU32(Pointer, ValueLE);
}

#endif
