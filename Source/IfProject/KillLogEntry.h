#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogEntry.generated.h"


UCLASS()
class IFPROJECT_API UKillLogEntry : public UUserWidget
{
	GENERATED_BODY()
	
protected:


public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void Setup(const FString& Killer, const FString& Victim, const int32 KillerID, const int32 VictimID);
};
