// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "Engine/Engine.h"
#include "DealerCharacter.generated.h"

UCLASS()
class IFPROJECT_API ADealerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// 생성자
	ADealerCharacter();

	// 매 프레임마다 호출
	virtual void Tick(float DeltaTime) override;

	// 입력 바인딩
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ManualReload();
	// 스킬들
	void UseDashSkill();
	void UseStunSkill();
	void UseUltimateSkill();

protected:
	virtual void BeginPlay() override;

	// 쿨다운 관련
	float DashCooldown = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentDashCooldown = 0.f;

	float StunCooldown = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentStunCooldown = 0.f;

private:
	// 내부 전용 타이머 핸들
	FTimerHandle UltimateDelayHandle;
};