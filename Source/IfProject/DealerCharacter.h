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
	// ������
	ADealerCharacter();

	// �� �����Ӹ��� ȣ��
	virtual void Tick(float DeltaTime) override;

	// �Է� ���ε�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ManualReload();
	// ��ų��
	void UseDashSkill();
	void UseStunSkill();
	void UseUltimateSkill();

protected:
	virtual void BeginPlay() override;

	// ��ٿ� ����
	float DashCooldown = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentDashCooldown = 0.f;

	float StunCooldown = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentStunCooldown = 0.f;

private:
	// ���� ���� Ÿ�̸� �ڵ�
	FTimerHandle UltimateDelayHandle;
};