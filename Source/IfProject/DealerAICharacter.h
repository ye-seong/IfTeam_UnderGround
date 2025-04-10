// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "DealerAICharacter.generated.h"

/**
 *
 */
UCLASS()
class IFPROJECT_API ADealerAICharacter : public AAICharacter
{
	GENERATED_BODY()

public:
	ADealerAICharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	void AIDashSkillTick() override;
	void AIStunSkillTick() override;
	void AIDealerUltimateSkillTick() override;

protected:
	// 쿨다운 관련
	float DashCooldown = 10.f;
	float CurrentDashCooldown = 0.f;

	float StunCooldown = 10.f;
	float CurrentStunCooldown = 0.f;

	bool ShouldUseDash();
	bool ShouldUseStun();
	bool ShouldUseUltimate();

private:
	// 내부 전용 타이머 핸들
	FTimerHandle UltimateDelayHandle;

	FTimerHandle DashSkillLoopHandle;
	FTimerHandle StunSkillLoopHandle;
	FTimerHandle UltimateSkillLoopHandle;


	// 동시스킬 막기 위한 타이머
	FTimerHandle SkillBusyHandle;
};
