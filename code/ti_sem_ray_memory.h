#ifndef TI_SEM_RAY_MEMORY_H
#define TI_SEM_RAY_MEMORY_H

#include <malloc.h>
#include <string.h>

#define ZeroStruct(Pointer) ZeroMemory(Pointer, sizeof(*Pointer))

internal void
ZeroMemory(void *Pointer, smi Size)
{
	memset(Pointer, 0, Size);
}

#define AllocArray(type, Count, ZeroInit) ((type *)AllocMemory(SizeOf(type) * (Count), ZeroInit))

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
FreeMemory(void *Pointer)
{
	free(Pointer);
}

internal void
CopyMemory(const void *Source, umi Size, void *Dest)
{
	memcpy(Dest, Source, Size);
}

internal void
MoveMemory(const void *Source, smi Size, void *Dest)
{
	memmove(Dest, Source, Size);
}

internal void
Write8(void *Pointer, smi Offset, u8 Value)
{
	u8 *At = (u8 *)Pointer + Offset;

	At[0] = Value;
}

internal void
Write16LE(void *Buffer, smi Offset, u16 Value)
{
	u8 *At = (u8 *)Buffer + Offset;

	At[0] = (u8)(Value >> 0);
	At[1] = (u8)(Value >> 8);
}

internal void
Write16BE(void *Buffer, smi Offset, u16 Value)
{
	u8 *At = (u8 *)Buffer + Offset;

	At[0] = (u8)(Value >> 8);
	At[1] = (u8)(Value >> 0);
}

internal void
Write32LE(void *Buffer, smi Offset, u32 Value)
{
	u8 *At = (u8 *)Buffer + Offset;

	At[0] = (u8)(Value >> 0);
	At[1] = (u8)(Value >> 8);
	At[2] = (u8)(Value >> 16);
	At[3] = (u8)(Value >> 24);
}

internal void
Write32BE(void *Buffer, smi Offset, u32 Value)
{
	u8 *At = (u8 *)Buffer + Offset;

	At[0] = (u8)(Value >> 24);
	At[1] = (u8)(Value >> 16);
	At[2] = (u8)(Value >> 8);
	At[3] = (u8)(Value >> 0);
}

internal void
Write64LE(void *Buffer, smi Offset, u64 Value)
{
	u8 *At = (u8 *)Buffer + Offset;

	At[0] = (u8)(Value >> 0);
	At[1] = (u8)(Value >> 8);
	At[2] = (u8)(Value >> 16);
	At[3] = (u8)(Value >> 24);
	At[4] = (u8)(Value >> 32);
	At[5] = (u8)(Value >> 40);
	At[6] = (u8)(Value >> 48);
	At[7] = (u8)(Value >> 56);
}

internal void
Write64BE(void *Buffer, smi Offset, u64 Value)
{
	u8 *At = (u8 *)Buffer + Offset;

	At[0] = (u8)(Value >> 56);
	At[1] = (u8)(Value >> 48);
	At[2] = (u8)(Value >> 40);
	At[3] = (u8)(Value >> 32);
	At[4] = (u8)(Value >> 24);
	At[5] = (u8)(Value >> 16);
	At[6] = (u8)(Value >> 8);
	At[7] = (u8)(Value >> 0);
}

internal void
Push8(void *Buffer, smi *Offset, u8 Value)
{
	Write8(Buffer, *Offset, Value);

	*Offset += SizeOf(Value);
}

internal void
Push16LE(void *Buffer, smi *Offset, u16 Value)
{
	Write16LE(Buffer, *Offset, Value);

	*Offset += SizeOf(Value);
}

internal void
Push16BE(void *Buffer, smi *Offset, u16 Value)
{
	Write16BE(Buffer, *Offset, Value);

	*Offset += SizeOf(Value);
}

internal void
Push32LE(void *Buffer, smi *Offset, u32 Value)
{
	Write32LE(Buffer, *Offset, Value);

	*Offset += SizeOf(Value);
}

internal void
Push32BE(void *Buffer, smi *Offset, u32 Value)
{
	Write32BE(Buffer, *Offset, Value);

	*Offset += SizeOf(Value);
}

internal void
Push64LE(void *Buffer, smi *Offset, u32 Value)
{
	Write64LE(Buffer, *Offset, Value);

	*Offset += SizeOf(Value);
}

internal void
Push64BE(void *Buffer, smi *Offset, u32 Value)
{
	Write64BE(Buffer, *Offset, Value);

	*Offset += SizeOf(Value);
}

internal void
PushZeros(void *Buffer, smi *Offset, smi Size)
{
	ZeroMemory((u8 *)Buffer + *Offset, Size);

	*Offset += Size;
}

#endif
