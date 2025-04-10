// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "Engine/Engine.h"
#include "HealerCharacter.generated.h"

UCLASS()
class IFPROJECT_API AHealerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHealerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	float TeleportHealSkillCooldown = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	float ShieldHealSkillCooldown = 15.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
		float CurrentTeleportHealSkillCooldown = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoolTime")
		float CurrentShieldHealSkillCooldown = 0.f;

	ABaseCharacter* TargetAlly = nullptr;
	FVector TargetMoveLocation;

	bool bIsMovingToTarget = false;
	FTimerHandle HealBoostTimerHandle;

	float UltimateDuration = 3.0f;
	bool bWantsToCastHealShield = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ManualReload();
	void UseTeleportHealSkill();
	void UseShieldHealSkill();
	void UseHealerUltimateSkill();
};
