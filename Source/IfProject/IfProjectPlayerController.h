// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IfHUDWidget.h"
#include "BaseCharacter.h"
#include "IfProjectPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class IFPROJECT_API AIfProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay();

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UIfHUDWidget* HUDWidget;  // HUD À§Á¬ ÂüÁ¶

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> BPUserInterfaceClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* BPUserInterfaceWidget;

	UFUNCTION(BlueprintCallable, Category = "KillLog")
	void NotifyKillLog(ABaseCharacter* KillerCharacter, ABaseCharacter* VictimCharacter);

private:
	
};
