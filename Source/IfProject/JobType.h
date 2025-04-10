#pragma once

#include "CoreMinimal.h"
#include "JobType.generated.h"

UENUM(BlueprintType)
enum class EJobType : uint8
{
	None	UMETA(DisplayName = "None"),
	Tanker	UMETA(DisplayName = "Tanker"),
	Dealer	UMETA(DisplayName = "Dealer"),
	Healer	UMETA(DisplayName = "Healer")
};
