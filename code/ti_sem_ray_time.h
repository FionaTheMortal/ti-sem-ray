#ifndef TI_SEM_RAY_TIME_H
#define TI_SEM_RAY_TIME_H

struct calendar_time
{
	s32 Second;
	s32 Minute;
	s32 Hour;
	s32 Day;
	s32 Month;
	s32 Year;
};

internal calendar_time
GetLocalCalendarTime()
{
	time_t Now = time(NULL);

	tm *LocalTime = localtime(&Now);

	calendar_time Result;

	Result.Second = LocalTime->tm_sec;
	Result.Minute = LocalTime->tm_min;
	Result.Hour = LocalTime->tm_hour;
	Result.Day = LocalTime->tm_mday;
	Result.Month = LocalTime->tm_mon + 1;
	Result.Year = LocalTime->tm_year + 1900;

	return Result;
}

internal s32
FormatFilenameTimestamp(c8 *Buffer, s32 BufferSize, calendar_time *Time)
{
	s32 Result = sprintf_s(Buffer, BufferSize, "%d-%02d-%02d-%02d%02d%02d", Time->Year, Time->Month, Time->Day, Time->Hour, Time->Minute, Time->Second);

	return Result;
}

#endif
