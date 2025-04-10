// Fill out your copyright notice in the Description page of Project Settings.


#include "DealerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DealerGun.h"

// Sets default values
ADealerCharacter::ADealerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
	GetCharacterMovement()->SetIsReplicated(true);

	GunClass = ADealerGun::StaticClass(); // µô·¯ Àü¿ë ÃÑ

	MaxHP = 200;
	CurrentHP = MaxHP;
	NormalAttackDamage = 30.0f;

}

// Called when the game starts or when spawned
void ADealerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADealerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentDashCooldown > 0.f)
	{
		CurrentDashCooldown -= DeltaTime;
	}
	if (CurrentStunCooldown > 0.f)
	{
		CurrentStunCooldown -= DeltaTime;
	}
}

// Called to bind functionality to input
void ADealerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ADealerCharacter::ManualReload);
	PlayerInputComponent->BindAction("Skill1", IE_Pressed, this, &ADealerCharacter::UseDashSkill);
	PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &ADealerCharacter::UseStunSkill);
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ADealerCharacter::UseUltimateSkill);

}

void ADealerCharacter::ManualReload()
{
	if (Gun) Gun->ManualReload();
}

void ADealerCharacter::UseDashSkill()
{
	PerformDashSkill(CurrentDashCooldown, DashCooldown);
}

void ADealerCharacter::UseStunSkill()
{
	PerformStunSkill(CurrentStunCooldown, StunCooldown);
}

void ADealerCharacter::UseUltimateSkill()
{
	PerformDealerUltimateSkill(UltimateDelayHandle);
}