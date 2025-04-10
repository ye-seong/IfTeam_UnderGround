// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "TankerAICharacter.generated.h"

/**
 *
 */
UCLASS()
class IFPROJECT_API ATankerAICharacter : public AAICharacter
{
	GENERATED_BODY()

public:
	ATankerAICharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void AIBarrierSkillTick() override;

	virtual void AIShockStrikeSkillTick() override;

	virtual void AITankerUltimateSkillTick() override;

protected:
	float BarrierCooldown = 10.f; // 쿨타임
	float CurrentBarrierCooldown = 0.f;
	float BarrierDistance = 200.f; // 캐릭터 앞에 소환할 거리

	float ShockStrikeCooldown = 10.f;
	float CurrentShockStrikeCooldown = 0.f;
	float ImpactRadius = 400.0f;
	float ImpactDamage = 20.0f;

	/* 궁극기 게이지 관련은 BaseCharacter에서 처리 */
	float UltimateDuration = 10.0f;	// 궁극기 지속 시간

	FTimerHandle BarrierCooldownHandle; // 쿨타임 관리용 핸들


	FTimerHandle AIBarrierSkillTickLoopHandle;
	FTimerHandle AIShockStrikeSkillLoopHandle;
	FTimerHandle AITankerUltimateSkillLoopHandle;

	FTimerHandle SkillBusyHandle;

	float BarrierSkillCooldown = 0.f;
	FTimerHandle BarrierResetHandle;
	bool bHasActiveBarrier = false;
	bool bHasActiveShock = false;

	bool ShouldUseBarrier();
	bool ShouldUseShockStrike();
};
