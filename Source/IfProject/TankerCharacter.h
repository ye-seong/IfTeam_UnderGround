// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "Engine/Engine.h"
#include "TankerCharacter.generated.h"

UCLASS()
class IFPROJECT_API ATankerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATankerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float BarrierCooldown = 10.f; // 쿨타임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentBarrierCooldown = 0.f; // 남은 쿨타임 (0이하시 사용 가능)
	float BarrierDistance = 200.f; // 캐릭터 앞에 소환할 거리

	float ShockStrikeCooldown = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentShockStrikeCooldown = 0.f;
	float ImpactRadius = 400.0f;
	float ImpactDamage = 20.0f;

	/* 궁극기 게이지 관련은 BaseCharacter에서 처리 */
	float UltimateDuration = 10.0f;	// 궁극기 지속 시간

	FTimerHandle BarrierCooldownHandle; // 쿨타임 관리용 핸들

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ManualReload();
	void UseBarrierSkill();
	void UseShockStrikeSkill();
	void UseUltimateSkill(); //탱커의 궁극기 스킬
};
