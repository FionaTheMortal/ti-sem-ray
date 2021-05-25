#ifndef TI_SEM_RAY_MEMORY_STREAM_H
#define TI_SEM_RAY_MEMORY_STREAM_H

enum buffered_stream_state
{
	BufferedStreamState_Success = 0,
	BufferedStreamState_NeedsNextBuffer
};

struct buffered_writer
{
	s32 State;
	smi RefillSize;

	c8 *Base;
	smi Size;
	smi Next;
	smi Mark;

	c8 *NextBufferBase;
	smi NextBufferNext;
	smi NextBufferSize;

	c8 *TempBufferBase;
	smi TempBufferSize;
};

struct buffered_reader
{
	s32 State;
	smi RefillSize;

	const c8 *Base;
	smi       Next;
	smi       Mark;
	smi       Size;

	const c8 *NextBufferBase;
	smi       NextBufferSize;
	smi       NextBufferNext;
	smi       NextBufferMark;

	c8 *TempBufferBase;
	smi TempBufferSize;
};

#define REFILL_READER_CR_BEGIN() \
	switch (Stream->State) \
	{ \
		case 0:

#define REFILL_READER_CR_END() \
	}

#define REFILL_READER_CR_RETURN(CRState, ReturnValue) \
	do \
	{ \
		State = ReturnValue; \
		Stream->State = CRState; \
		goto RefillReaderCRReturn; \
		case CRState:; \
	} \
	while (0)

#define REFILL_READER_REFILL_NEXT_BUFFER(CRState) \
	while (!Stream->NextBufferBase) \
	{ \
		REFILL_READER_CR_RETURN(CRState, BufferedStreamState_NeedsNextBuffer); \
	}

#define REFILL_WRITER_CR_BEGIN() \
	switch (Stream->State) \
	{ \
		case 0:

#define REFILL_WRITER_CR_END() \
	}

#define REFILL_WRITER_CR_RETURN(CRState, ReturnValue) \
	do \
	{ \
		State = ReturnValue; \
		Stream->State = CRState; \
		goto RefillWriterCRReturn; \
		case CRState:; \
	} \
	while (0)

#define REFILL_WRITER_REFILL_NEXT_BUFFER(CRState) \
	while (!Stream->NextBufferBase || Stream->NextBufferNext == Stream->NextBufferSize) \
	{ \
		REFILL_WRITER_CR_RETURN(CRState, BufferedStreamState_NeedsNextBuffer); \
	}


internal void
InitBufferedWriter(buffered_writer *Stream, void *TempBuffer, smi TempBufferSize, smi RefillSize)
{
	ZeroStruct(Stream);

	Stream->TempBufferBase = (c8 *)TempBuffer;
	Stream->TempBufferSize = TempBufferSize;
	Stream->RefillSize = RefillSize;
}

internal void
InitBufferedReader(buffered_reader *Stream, void *TempBuffer, smi TempBufferSize, smi RefillSize)
{
	ZeroStruct(Stream);

	Stream->TempBufferBase = (c8 *)TempBuffer;
	Stream->TempBufferSize = TempBufferSize;
	Stream->RefillSize = RefillSize;
}

internal void
SetNextBuffer(buffered_writer *Stream, void *Buffer, smi BufferSize)
{
	Stream->NextBufferBase = (c8 *)Buffer;
	Stream->NextBufferSize = BufferSize;
	Stream->NextBufferNext = 0;
}

internal void
SetNextBuffer(buffered_reader *Stream, const void *Buffer, smi BufferSize)
{
	Stream->NextBufferBase = (const c8 *)Buffer;
	Stream->NextBufferSize = BufferSize;
	Stream->NextBufferNext = 0;
}

internal smi
GetProcessedSize(buffered_writer *Stream)
{
	smi Result = 0;

	if (Stream->Base == Stream->TempBufferBase)
	{
		Result = Stream->NextBufferNext;
	}
	else
	{
		Result = Stream->Next;
	}

	return Result;
}

internal smi
GetProcessedSize(buffered_reader *Stream)
{
	smi Result = 0;

	if (Stream->Base == Stream->TempBufferBase)
	{
		Result = Stream->NextBufferNext;
	}
	else
	{
		Result = Stream->Next;
	}

	return Result;
}


internal b32
CanSwitchToNextBufferDirectly(buffered_reader *Stream)
{
	b32 Result = false;

	b32 CanLeaveCurrentBufferDirectly = false;
	smi OffsetInNextBuffer = 0;

	if (Stream->Base == Stream->TempBufferBase)
	{
		if (Stream->Next >= Stream->NextBufferMark)
		{
			CanLeaveCurrentBufferDirectly = true;
			OffsetInNextBuffer = Stream->Next - Stream->NextBufferMark;
		}
	}
	else if (Stream->Next == Stream->Size)
	{
		CanLeaveCurrentBufferDirectly = true;
	}

	if (CanLeaveCurrentBufferDirectly)
	{
		b32 CanSwitchToNextBuffer = (Stream->NextBufferSize > OffsetInNextBuffer + Stream->RefillSize);

		if (CanSwitchToNextBuffer)
		{
			Result = true;
		}
	}

	return Result;
}

internal void
ConsumeNextBuffer(buffered_reader *Stream)
{
	Stream->NextBufferBase = 0;
	Stream->NextBufferSize = 0;
	Stream->NextBufferNext = 0;
}

internal void
SwitchToNextBuffer(buffered_reader *Stream)
{
	Stream->Base = Stream->NextBufferBase;
	Stream->Size = Stream->NextBufferSize;

	if (Stream->Base == Stream->TempBufferBase)
	{
		Stream->Next = Stream->NextBufferNext + Stream->Next - Stream->NextBufferMark;
	}
	else
	{
		Stream->Next = Stream->NextBufferNext;
	}

	ConsumeNextBuffer(Stream);
}

internal void
SwitchToTempBuffer(buffered_reader *Stream)
{
	smi SizeToCopy = Stream->Size - Stream->Next;

	if (SizeToCopy > 0)
	{
		if (Stream->Base == Stream->TempBufferBase)
		{
			Stream->NextBufferMark -= SizeToCopy;
		}

		MoveMemory(&Stream->Base[Stream->Next], SizeToCopy, Stream->TempBufferBase);
	}

	Stream->Size = SizeToCopy;
	Stream->Base = Stream->TempBufferBase;
	Stream->Next = 0;
}

internal void
CopyStartOfNextBufferToTemp(buffered_reader *Stream)
{
	smi SizeToFill = Stream->TempBufferSize - Stream->Size;
	smi DataToCopy = Stream->NextBufferSize - Stream->NextBufferNext;
	smi SizeToCopy = Min(SizeToFill, DataToCopy);

	if (SizeToCopy > 0)
	{
		CopyMemory(&Stream->NextBufferBase[Stream->NextBufferNext], SizeToCopy, &Stream->TempBufferBase[Stream->Size]);
	}

	if (Stream->NextBufferNext == 0)
	{
		Stream->NextBufferMark = Stream->Size;
	}

	Stream->Size += SizeToCopy;
	Stream->NextBufferNext += SizeToCopy;
}

internal s32
RefillReader(buffered_reader *Stream)
{
	s32 State = BufferedStreamState_Success;

	REFILL_READER_CR_BEGIN();

	if (Stream->Next >= Stream->Mark)
	{
		REFILL_READER_REFILL_NEXT_BUFFER(1);

		if (CanSwitchToNextBufferDirectly(Stream))
		{
			SwitchToNextBuffer(Stream);
		}
		else
		{
			SwitchToTempBuffer(Stream);

			while (Stream->Size < Stream->TempBufferSize)
			{
				CopyStartOfNextBufferToTemp(Stream);

				if (Stream->NextBufferNext == Stream->NextBufferSize)
				{
					ConsumeNextBuffer(Stream);

					REFILL_READER_REFILL_NEXT_BUFFER(2);
				}
			}
		}

		Stream->Mark = Stream->Size - Stream->RefillSize;
		Stream->State = 0;
	}

	REFILL_READER_CR_END();

RefillReaderCRReturn:

	return State;
}

internal void
SwitchToTempBuffer(buffered_writer *Stream)
{
	Stream->NextBufferNext = Stream->Next;

	Stream->Base = Stream->TempBufferBase;
	Stream->Size = Stream->TempBufferSize;
	Stream->Next = 0;
}

internal void
CopyTempBufferToNextBuffer(buffered_writer *Stream)
{
	smi DataToCopy = Stream->Size - Stream->Next;
	smi SizeToFill = Stream->NextBufferSize - Stream->NextBufferNext;
	smi SizeToCopy = Min(DataToCopy, SizeToFill);

	CopyMemory(&Stream->Base[Stream->Next], SizeToCopy, &Stream->NextBufferBase[Stream->NextBufferNext]);

	Stream->Next += SizeToCopy;
	Stream->NextBufferNext += SizeToCopy;
}

internal s32
RefillWriter(buffered_writer *Stream)
{
	s32 State = BufferedStreamState_Success;

	REFILL_WRITER_CR_BEGIN();

	if (Stream->Next >= Stream->Mark)
	{
		if (Stream->Base != Stream->TempBufferBase && Stream->Base)
		{
			SwitchToTempBuffer(Stream);
		}
		else
		{
			Stream->Size = Stream->Next;
			Stream->Next = 0;

			while (Stream->Next < Stream->Size)
			{
				CopyTempBufferToNextBuffer(Stream);

				if (Stream->NextBufferNext == Stream->NextBufferSize)
				{
					REFILL_WRITER_REFILL_NEXT_BUFFER(1);
				}
			}

			if (Stream->NextBufferNext + Stream->RefillSize < Stream->NextBufferSize)
			{
				Stream->Base = Stream->NextBufferBase;
				Stream->Size = Stream->NextBufferSize;
				Stream->Next = Stream->NextBufferNext;
			}
			else
			{
				Stream->Size = Stream->TempBufferSize;
				Stream->Next = 0;
			}
		}

		Stream->Mark = Stream->Size - Stream->RefillSize;
		Stream->State = 0;
	}

	REFILL_WRITER_CR_END();

RefillWriterCRReturn:

	return State;
}

#endif
