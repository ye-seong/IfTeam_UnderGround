// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "HealerAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class IFPROJECT_API AHealerAICharacter : public AAICharacter
{
	GENERATED_BODY()

public:
	AHealerAICharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
	void AITeleportHealSkillTick()override;
	void AIShieldHealSkillTick()override;
	void AIHealerUltimateSkillTick()override;

	bool ShouldUseTeleportHeal();
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	float TeleportHealCooldown = 10.f;
	float ShieldHealCooldown = 15.f;

	float CurrentTeleportHealCooldown = 0.f;
	float CurrentShieldHealCooldown = 0.f;

	/*ABaseCharacter* TargetAlly = nullptr;
	FVector TargetMoveLocation;*/

	/*bool bIsMovingToTarget = false;*/
	FTimerHandle HealBoostTimerHandle;

	float UltimateDuration = 3.0f;


	bool bWantsToCastHealShield = false;
	FTimerHandle TeleportHealSkillLoopHandle;
	FTimerHandle SHealShieldSkillLoopHandle;
	FTimerHandle UHealerUltimateSkillLoopHandle;

	FTimerHandle SkillBusyHandle;

};
