// Fill out your copyright notice in the Description page of Project Settings.


#include "TankerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TankerGun.h"
#include "BarrierActor.h"

// Sets default values
ATankerCharacter::ATankerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 멀티플레이 복제 설정
	bReplicates = true;
	bAlwaysRelevant = true;
	GetCharacterMovement()->SetIsReplicated(true);

	GunClass = ATankerGun::StaticClass(); // 탱커 전용 총

	MaxHP = 250;
	CurrentHP = MaxHP;
	NormalAttackDamage = 20.0f;
}

// Called when the game starts or when spawned
void ATankerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATankerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 탱커 전용 Tick 로직
	// 방벽 쿨타임 감소
	if (CurrentBarrierCooldown > 0.f)
	{
		CurrentBarrierCooldown -= DeltaTime;
	}
	if (CurrentShockStrikeCooldown > 0.f)
	{
		CurrentShockStrikeCooldown -= DeltaTime;
	}
}

// Called to bind functionality to input
void ATankerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATankerCharacter::ManualReload);
	PlayerInputComponent->BindAction("Skill1", IE_Pressed, this, &ATankerCharacter::UseBarrierSkill);
	PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &ATankerCharacter::UseShockStrikeSkill);
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ATankerCharacter::UseUltimateSkill);
}

void ATankerCharacter::ManualReload()
{
	if (Gun) Gun->ManualReload();
}

// 기존 탱커의 방벽 세우기 스킬
void ATankerCharacter::UseBarrierSkill()
{
	PerformBarrierSkill(CurrentBarrierCooldown, BarrierCooldown, BarrierDistance, BarrierClass);
}

void ATankerCharacter::UseShockStrikeSkill()
{
	PerformShockStrikeSkill(CurrentShockStrikeCooldown, ShockStrikeCooldown, ImpactRadius, ImpactDamage);
}
void ATankerCharacter::UseUltimateSkill()
{
	PerformTankerUltimateSkill(UltimateDuration);
}