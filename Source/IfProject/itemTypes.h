// ItemTypes.h
#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    HP UMETA(DisplayName = "Health Item"),
    Ultimate UMETA(DisplayName = "Ultimate Item")
};
