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

	float BarrierCooldown = 10.f; // ��Ÿ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentBarrierCooldown = 0.f; // ���� ��Ÿ�� (0���Ͻ� ��� ����)
	float BarrierDistance = 200.f; // ĳ���� �տ� ��ȯ�� �Ÿ�

	float ShockStrikeCooldown = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
	float CurrentShockStrikeCooldown = 0.f;
	float ImpactRadius = 400.0f;
	float ImpactDamage = 20.0f;

	/* �ñر� ������ ������ BaseCharacter���� ó�� */
	float UltimateDuration = 10.0f;	// �ñر� ���� �ð�

	FTimerHandle BarrierCooldownHandle; // ��Ÿ�� ������ �ڵ�

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ManualReload();
	void UseBarrierSkill();
	void UseShockStrikeSkill();
	void UseUltimateSkill(); //��Ŀ�� �ñر� ��ų
};
