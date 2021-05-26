#ifndef TI_SEM_RAY_RANDOM_H
#define TI_SEM_RAY_RANDOM_H

struct lcg32
{
	u32 Prev;
};

internal lcg32
LCG32(u32 Seed)
{
	lcg32 Result;

	Result.Prev = Seed;
	
	return Result;
}

internal u32
RandomNextU32(lcg32 *Random)
{
	u32 Result = 32310901 * Random->Prev + 29943829;

	Random->Prev = Result;

	return Result;
}

internal u32
RandomNextU32Bounded(lcg32 *Random, u32 Bound)
{
	u32 Result;
	u32 Threshold = (u32)(-(s32)Bound) % Bound;

	for (;;) 
	{
		u32 Sample = RandomNextU32(Random);

		if (Sample >= Threshold)
		{
			Result = Sample % Bound;
			break;
		}
	}

	return Result;
}

internal s32
RandomNextS32InRange(lcg32 *Random, s32 Min, s32 Max)
{
	u32 Bound = (u32)(Max - Min) + 1;
	u32 Sample = RandomNextU32Bounded(Random, Bound);
	s32 Result = Min + Sample;

	return Result;
}

internal f32
RandomNextF32(lcg32 *Random)
{
	u32 Max = (1 << 25);

	u32 Sample = RandomNextU32Bounded(Random, Max + 1);
	f32 Result = (f32)Sample / (f32)Max;

	return Result;
}

internal f32
RandomNextF32Bilateral(lcg32 *Random)
{
	s32 Max = (1 << 25);
	s32 Min = -Max;

	s32 Sample = RandomNextS32InRange(Random, Min, Max);
	f32 Result = (f32)Sample / (f32)Max;

	return Result;
}

#endif