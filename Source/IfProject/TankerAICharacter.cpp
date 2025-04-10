// Fill out your copyright notice in the Description page of Project Settings.


#include "TankerAICharacter.h"
#include "BarrierActor.h"
#include "Kismet/GameplayStatics.h"

ATankerAICharacter::ATankerAICharacter()
{
    JobType = EJobType::Tanker;
    MaxHP = 250;
    CurrentHP = MaxHP;
    NormalAttackDamage = 20.0f;
}

void ATankerAICharacter::BeginPlay()
{
    Super::BeginPlay();

    // 타이머로 반복 실행만 설정함
    this->GetWorldTimerManager().SetTimer(AIBarrierSkillTickLoopHandle, this, &ATankerAICharacter::AIBarrierSkillTick, 3.0f, true);
    this->GetWorldTimerManager().SetTimer(AIShockStrikeSkillLoopHandle, this, &ATankerAICharacter::AIShockStrikeSkillTick, 5.0f, true);
    this->GetWorldTimerManager().SetTimer(AITankerUltimateSkillLoopHandle, this, &ATankerAICharacter::AITankerUltimateSkillTick, 10.0f, true);
}

void ATankerAICharacter::Tick(float DeltaTime)
{
    AAICharacter::Tick(DeltaTime);

    if (CurrentBarrierCooldown > 0.f)
    {
        CurrentBarrierCooldown -= DeltaTime;
    }
    
    if (CurrentShockStrikeCooldown > 0.f)
    {
        CurrentShockStrikeCooldown -= DeltaTime;
    }
}

void ATankerAICharacter::AIBarrierSkillTick()
{
    if (bIsUsingSkill || CurrentBarrierCooldown > 0.f) return;

    if (ShouldUseBarrier())
    {
        PerformBarrierSkill(
            CurrentBarrierCooldown,
            BarrierCooldown,
            BarrierDistance,
            BarrierClass
        );

        bIsUsingSkill = true;

        PerformBarrierSkill(CurrentBarrierCooldown, BarrierCooldown, BarrierDistance, BarrierClass);

        GetWorld()->GetTimerManager().SetTimer(
            SkillBusyHandle,
            [this]() { bIsUsingSkill = false; },
            0.5f,
            false
        );
    }
}

void ATankerAICharacter::AIShockStrikeSkillTick()
{
    if (bIsUsingSkill || CurrentShockStrikeCooldown > 0.f) return;

    if (ShouldUseShockStrike())
    {
        PerformShockStrikeSkill(
            CurrentShockStrikeCooldown, 
            ShockStrikeCooldown,        
            ImpactRadius,               
            ImpactDamage              
        );

        bIsUsingSkill = true;

        PerformShockStrikeSkill(CurrentShockStrikeCooldown, ShockStrikeCooldown, ImpactRadius, ImpactDamage);

        GetWorld()->GetTimerManager().SetTimer(
            SkillBusyHandle,
            [this]() { bIsUsingSkill = false; },
            1.0f,
            false
        );
    }

}
void ATankerAICharacter::AITankerUltimateSkillTick()
{
    if (bIsUsingSkill || UltiNum < 3) return;

    bIsUsingSkill = true;

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(Actor);
        if (BaseCharacter && BaseCharacter->TeamID == this->TeamID && !BaseCharacter->bIsDead && BaseCharacter->CurrentHP < BaseCharacter->MaxHP * 0.3f)
        {
            PerformTankerUltimateSkill(UltimateDuration);
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        SkillBusyHandle,
        [this]() { bIsUsingSkill = false; },
        1.5f,
        false
    );
}

bool ATankerAICharacter::ShouldUseBarrier()
{
    TArray<AActor*> NearbyCharacters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), NearbyCharacters);

    if (bHasActiveBarrier)
        return false;

    for (AActor* Character : NearbyCharacters)
    {
        ABaseCharacter* Target = Cast<ABaseCharacter>(Character);
        if (!Target) continue;

        if (Target == this) continue;

        if (Target->TeamID != this->TeamID)
        {
            float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
            if (Distance <= 1400.f)  
            {
                return true;
            }
        }
    }

    return false;
}

bool ATankerAICharacter::ShouldUseShockStrike()
{
    TArray<AActor*> NearbyCharacters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), NearbyCharacters);

    if (bHasActiveShock)
        return false;


    for (AActor* Character : NearbyCharacters)
    {
        ABaseCharacter* Target = Cast<ABaseCharacter>(Character);
        if (!Target) continue;


        if (Target == this) continue;

        if (Target->TeamID != this->TeamID)
        {
            float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
            if (Distance <= 600.f)
            {
                return true;
            }
        }
    }

    return false;
}