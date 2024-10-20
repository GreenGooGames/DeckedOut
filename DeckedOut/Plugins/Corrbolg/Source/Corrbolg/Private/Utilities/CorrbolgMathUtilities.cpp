// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/CorrbolgMathUtilities.h"

int FCorrbolgMathUtilities::Wrap(const int Value, const int Min, const int Max)
{
	if (Max == Min)
	{
		return Min;
	}

	int32 Range = Max - Min + 1;
	if (Range == 0)
	{
		return Min;
	}

	int32 Result = (Value - Min) % Range;

	if (Result < 0)
	{
		Result += Range;
	}

	return Result + Min;
}
