// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "AIController.h"
#include "JobType.h"
#include "DealerCharacter.h"
#include "TankerCharacter.h"
#include "HealerCharacter.h"
#include "AICharacter.generated.h"

// AI ���� ����
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle UMETA(DisplayName = "Idle"),              // ��� ����
	Searching UMETA(DisplayName = "Searching"),    // ���� ã�� ����
	NeedHeal UMETA(DisplayName = "NeedHeal"),
	NeedUlti UMETA(DisplayName = "NeedUlti"),
	Attacking UMETA(DisplayName = "Attacking"),    // ���� ����
	UsingSkillA UMETA(DisplayName = "Using Skill A"), // ��ų A ��� ��
	UsingSkillB UMETA(DisplayName = "Using Skill B"), // ��ų B ��� ��
	UsingUltimate UMETA(DisplayName = "Using Ultimate"), // �ñر� ��� ��
	Dead UMETA(DisplayName = "Dead")               // ���� ����
};


UCLASS()
class IFPROJECT_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AAICharacter();

	// ������ �´� ĳ���� Ŭ���� ����
	ADealerCharacter* DealerCharacterRef;
	ATankerCharacter* TankerCharacterRef;
	AHealerCharacter* HealerCharacterRef;

	EAIState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<FName> TargetTags;

	int32 CurrentTagIndex = 0;


protected:
	virtual void BeginPlay() override;

	ABaseCharacter* TargetEnemy;

	bool DetectEnemies();

	void StartFighting(ABaseCharacter* Enemy);

	FVector TargetLocation = FVector(-390.f, -60.f, 200.f);	// ������ ��ǥ

	void MoveToTaggedActor();


	UPROPERTY(EditAnywhere, Category = "AI")
	AActor* ATeamTarget;

	UPROPERTY(EditAnywhere, Category = "AI")
	AActor* BTeamTarget;

	AActor* CurrentTargetActor = nullptr;

	//aiĳ���� ���ý�ų ����
	UPROPERTY(BlueprintReadOnly)
	bool bIsUsingSkill = false;

	void MoveRandomlyInRange();

public:
	virtual void Tick(float DetaTime) override;

	// ��Ŀ
	virtual void AIBarrierSkillTick() {}
	virtual void AIShockStrikeSkillTick() {}
	virtual void AITankerUltimateSkillTick() {}

	// ����
	virtual void AIDashSkillTick() {}
	virtual void AIStunSkillTick() {}
	virtual void AIDealerUltimateSkillTick() {}

	// ����
	virtual void AITeleportHealSkillTick() {}
	virtual void AIShieldHealSkillTick() {}
	virtual void AIHealerUltimateSkillTick() {}

	void StopMovement();

	void MoveToTarget(FVector Location);

	void FindHealPack();
	void FindUltimateItem();

	/*FVector TargetLoc;*/



	//void AvoidObstacleOrJump();
	//bool ShouldJumpToReachTarget(FVector TargetLocation);
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	//	bool bFromSweep, const FHitResult& SweepResult);
	//bool ShouldJump(AActor* OtherActor);


private:
	FTimerHandle TimerHandle;
};
