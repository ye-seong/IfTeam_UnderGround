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
	float BarrierCooldown = 10.f; // ��Ÿ��
	float CurrentBarrierCooldown = 0.f;
	float BarrierDistance = 200.f; // ĳ���� �տ� ��ȯ�� �Ÿ�

	float ShockStrikeCooldown = 10.f;
	float CurrentShockStrikeCooldown = 0.f;
	float ImpactRadius = 400.0f;
	float ImpactDamage = 20.0f;

	/* �ñر� ������ ������ BaseCharacter���� ó�� */
	float UltimateDuration = 10.0f;	// �ñر� ���� �ð�

	FTimerHandle BarrierCooldownHandle; // ��Ÿ�� ������ �ڵ�


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
