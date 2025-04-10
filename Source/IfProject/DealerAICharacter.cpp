// Fill out your copyright notice in the Description page of Project Settings.


#include "DealerAICharacter.h"

ADealerAICharacter::ADealerAICharacter()
{
    JobType = EJobType::Dealer;
	MaxHP = 200;
	CurrentHP = MaxHP;
	NormalAttackDamage = 30.0f;
}

void ADealerAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// 타이머로 반복 실행만 설정함
	this->GetWorldTimerManager().SetTimer(DashSkillLoopHandle, this, &ADealerAICharacter::AIDashSkillTick, 3.0f, true);
	this->GetWorldTimerManager().SetTimer(StunSkillLoopHandle, this, &ADealerAICharacter::AIStunSkillTick, 5.0f, true);
	this->GetWorldTimerManager().SetTimer(UltimateSkillLoopHandle, this, &ADealerAICharacter::AIDealerUltimateSkillTick, 10.0f, true);
}

void ADealerAICharacter::Tick(float DeltaTime)
{
    AAICharacter::Tick(DeltaTime); 
	if (CurrentDashCooldown > 0.f)
	{
		CurrentDashCooldown -= DeltaTime;

	}
	if (CurrentStunCooldown > 0.f)
	{
		CurrentStunCooldown -= DeltaTime;

	}
}

void ADealerAICharacter::AIDashSkillTick()
{
	if (bIsUsingSkill || CurrentDashCooldown > 0.f)
		return;

	bIsUsingSkill = true;

	PerformDashSkill(CurrentDashCooldown, DashCooldown);

	GetWorld()->GetTimerManager().SetTimer(
		SkillBusyHandle,
		[this]() { bIsUsingSkill = false; },
		0.3f, 
		false
	);
}

void ADealerAICharacter::AIStunSkillTick()
{
	if (!ShouldUseStun() || bIsUsingSkill || CurrentDashCooldown > 0.f)
		return;

	bIsUsingSkill = true;

	PerformStunSkill(CurrentStunCooldown, StunCooldown);

	GetWorld()->GetTimerManager().SetTimer(
		SkillBusyHandle,
		[this]() { bIsUsingSkill = false; },
		2.0f,
		false
	);

}

void ADealerAICharacter::AIDealerUltimateSkillTick()
{
	if (!ShouldUseUltimate() || bIsUsingSkill || CurrentDashCooldown > 0.f)
		return;
	bIsUsingSkill = true;

	PerformDealerUltimateSkill(UltimateDelayHandle);


	GetWorld()->GetTimerManager().SetTimer(
		SkillBusyHandle,
		[this]() { bIsUsingSkill = false; },
		1.2f,
		false
	);
}

bool ADealerAICharacter::ShouldUseStun()
{
	if (bIsDead) return false;

	TArray<AActor*> NearbyCharacters;
	for (AActor* Character : NearbyCharacters)
	{
		ABaseCharacter* Target = Cast<ABaseCharacter>(Character);
		if (!Target) continue;

		if (Target == this) continue;

		if (Target->TeamID != this->TeamID)
		{
			float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
			if (Distance <= 700.f) 
			{
				return true;
			}
		}
	}

	return false;
}

bool ADealerAICharacter::ShouldUseUltimate()
{
	if (bIsDead) return false;

	TArray<AActor*> NearbyCharacters;
	for (AActor* Character : NearbyCharacters)
	{
		ABaseCharacter* Target = Cast<ABaseCharacter>(Character);
		if (!Target) continue;

		if (Target == this) continue;

		if (Target->TeamID != this->TeamID)
		{
			float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
			if (Distance <= 700.f) 
			{
				return true;
			}
		}
	}

	return false;
}