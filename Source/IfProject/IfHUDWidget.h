// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogEntry.h"
#include "BaseCharacter.h"
#include "IfHUDWidget.generated.h"

class UVerticalBox;
class UKillLogEntry;

UCLASS()
class IFPROJECT_API UIfHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddKillLog(ABaseCharacter* KillerCharacter, ABaseCharacter* VictimCharacter);

protected:
	virtual void NativeConstruct() override;
	void InitializeHUD();

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* KillLogBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KillLog")
	TSubclassOf<UKillLogEntry> KillLogEntryClass;
	
};
