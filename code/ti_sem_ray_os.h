#ifndef TI_SEM_RAY_OS_H
#define TI_SEM_RAY_OS_H

#include <stdio.h>

#define OS_FILE_OFFSET_APPEND (-1)

struct os_file
{
	void *Handle;
};

enum os_file_op
{
	OSFileOp_OpenExisting,
	OSFileOp_OpenExistingOrCreate,
	OSFileOp_CreateOrOverwrite
};

internal b32
OSOpenFile(os_file *Result, const c8 *Filename, s32 Operation)
{
	b32 Success = false;

	c8 *Mode = 0;

	switch (Operation)
	{
		case OSFileOp_OpenExisting:
		{
			Mode = "r+b";
		} break;
		case OSFileOp_OpenExistingOrCreate:
		{
			Mode = "a+b";
		} break;
		case OSFileOp_CreateOrOverwrite:
		{
			Mode = "w+b";
		} break;
	}

	FILE *FileHandle = fopen(Filename, Mode);

	if (FileHandle)
	{
		Result->Handle = FileHandle;
		
		Success = true;
	}

	return Success;
}

internal b32
OSCloseFile(os_file *File)
{
	b32 Success = false;

	int FCloseError = fclose((FILE *)File->Handle);

	if (FCloseError)
	{
		Success = false;
	}

	return Success;
}

internal b32
OSWriteFile(os_file *File, void *Data, smi Size, smi Offset)
{
	b32 Success = false;

	smi FSeekOffset = Offset;
	int FSeekPosition = SEEK_SET;

	if (Offset == OS_FILE_OFFSET_APPEND)
	{
		FSeekOffset = 0;
		FSeekPosition = SEEK_END;
	}

	int FSeekError = fseek((FILE *)File->Handle, FSeekOffset, FSeekPosition);

	if (!FSeekError)
	{
		int ElementCount = fwrite(Data, Size, 1, (FILE *)File->Handle);

		if (ElementCount == 1)
		{
			Success = true;
		}
	}

	return Success;
}

#endif
