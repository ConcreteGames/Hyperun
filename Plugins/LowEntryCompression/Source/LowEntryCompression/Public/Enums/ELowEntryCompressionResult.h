#pragma once


#include "Engine.h"
#include "Core.h"
#include "CoreUObject.h"

#include "ELowEntryCompressionResult.generated.h"


UENUM()
enum class ELowEntryCompressionResult
{
	Success = 0,
	Failed = 1
};
