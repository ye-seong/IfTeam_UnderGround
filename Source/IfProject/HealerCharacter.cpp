// Fill out your copyright notice in the Description page of Project Settings.


#include "HealerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealerGun.h"

AHealerCharacter::AHealerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	GetCharacterMovement()->SetIsReplicated(true);
	GunClass = AHealerGun::StaticClass();

	MaxHP = 200;
	CurrentHP = MaxHP;
	NormalAttackDamage = 10.0f;
}

void AHealerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHealerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTeleportHealSkillCooldown > 0.f)
		CurrentTeleportHealSkillCooldown -= DeltaTime;

	if (CurrentShieldHealSkillCooldown > 0.f)
		CurrentShieldHealSkillCooldown -= DeltaTime;

	if (bIsMovingToTarget)
	{
		PerformTeleportHealSkillTick(this,  DeltaTime);
	}

	if (bWantsToCastHealShield)
	{
		bWantsToCastHealShield = false;

		// 여기에서 DeltaTime 넘기면서 실행
		PerformHealShieldSkill(CurrentShieldHealSkillCooldown, UltimateDuration, HealBoostTimerHandle, DeltaTime);
	}
}

void AHealerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AHealerCharacter::ManualReload);
	PlayerInputComponent->BindAction("Skill1", IE_Pressed, this, &AHealerCharacter::UseTeleportHealSkill);
	PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &AHealerCharacter::UseShieldHealSkill);
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &AHealerCharacter::UseHealerUltimateSkill);
}

void AHealerCharacter::ManualReload()
{
	if (Gun) Gun->ManualReload();
}

void AHealerCharacter::UseTeleportHealSkill()
{
	PerformTeleportHealSkill(CurrentTeleportHealSkillCooldown, TeleportHealSkillCooldown);
}

void AHealerCharacter::UseShieldHealSkill()
{
	bWantsToCastHealShield = true; // 플래그만 켬
}

void AHealerCharacter::UseHealerUltimateSkill()
{
	PerformHealerUltimateSkill();
}