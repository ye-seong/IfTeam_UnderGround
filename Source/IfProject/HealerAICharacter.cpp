// Fill out your copyright notice in the Description page of Project Settings.


#include "HealerAICharacter.h"
#include "Kismet/GameplayStatics.h"

AHealerAICharacter::AHealerAICharacter()
{
    JobType = EJobType::Healer;
	MaxHP = 200;
	CurrentHP = MaxHP;
	NormalAttackDamage = 10.0f;
}

void AHealerAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// 타이머로 반복 실행만 설정함
	this->GetWorldTimerManager().SetTimer(TeleportHealSkillLoopHandle, this, &AHealerAICharacter::AITeleportHealSkillTick, 3.0f, true);
	this->GetWorldTimerManager().SetTimer(SHealShieldSkillLoopHandle, this, &AHealerAICharacter::AIShieldHealSkillTick, 5.0f, true);
	this->GetWorldTimerManager().SetTimer(UHealerUltimateSkillLoopHandle, this, &AHealerAICharacter::AIHealerUltimateSkillTick, 10.0f, true);
}

void AHealerAICharacter::Tick(float DeltaTime) {
	AAICharacter::Tick(DeltaTime); // ? 명시적으로 부모 클래스의 Tick 호출
	if (CurrentTeleportHealCooldown > 0.f)
	{
		CurrentTeleportHealCooldown -= DeltaTime;

	}
	if (CurrentShieldHealCooldown > 0.f)
	{
		CurrentShieldHealCooldown -= DeltaTime;

	}

	if (bIsMovingToTarget && ShouldUseTeleportHeal())
	{
		PerformTeleportHealSkillTick(this, DeltaTime);
	}

	if (bWantsToCastHealShield)
	{
		bWantsToCastHealShield = false;
		bIsUsingSkill = true;
		PerformHealShieldSkill(CurrentShieldHealCooldown, UltimateDuration, HealBoostTimerHandle, DeltaTime);

		GetWorld()->GetTimerManager().SetTimer(
			SkillBusyHandle,
			[this]() { bIsUsingSkill = false; },
			2.0f,
			false
		);
	}
}

void AHealerAICharacter::AITeleportHealSkillTick()
{
	if (bIsUsingSkill || CurrentTeleportHealCooldown > 0.f) return;

	bIsUsingSkill = true;


	PerformTeleportHealSkill(CurrentTeleportHealCooldown, TeleportHealCooldown);


	GetWorld()->GetTimerManager().SetTimer(
		SkillBusyHandle,
		[this]() { bIsUsingSkill = false; },
		1.0f, 
		false
	);
}

void AHealerAICharacter::AIShieldHealSkillTick()
{
	if (bIsUsingSkill || CurrentShieldHealCooldown > 0.f) return;

	bWantsToCastHealShield = true;
}

void AHealerAICharacter::AIHealerUltimateSkillTick()
{
	if (bIsUsingSkill || UltiNum < 3) return;

	bIsUsingSkill = true;
	PerformHealerUltimateSkill();
	GetWorld()->GetTimerManager().SetTimer(
		SkillBusyHandle,
		[this]() { bIsUsingSkill = false; },
		3.0f,
		false
	);
}

bool AHealerAICharacter::ShouldUseTeleportHeal()
{
	if (bIsDead) return false;

	bool bLowHPAllies = false;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(Actor);
		if (BaseCharacter && BaseCharacter->TeamID == this->TeamID && !BaseCharacter->bIsDead && BaseCharacter->CurrentHP < BaseCharacter->MaxHP * 0.3f)
		{
			bLowHPAllies = true;
			break;
		}
	}
	return false;
}

