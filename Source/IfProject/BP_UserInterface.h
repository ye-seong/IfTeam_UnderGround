// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BP_UserInterface.generated.h"

/**
 * 
 */
UCLASS()
class IFPROJECT_API UBP_UserInterface : public UUserWidget
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Kill Log")
    void AddKillLog(FString KillMessage);

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (BindWidget))
    class UVerticalBox* KillLogBox;
};
