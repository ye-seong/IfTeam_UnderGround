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

// AI 현재 상태
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle UMETA(DisplayName = "Idle"),              // 대기 상태
	Searching UMETA(DisplayName = "Searching"),    // 적을 찾는 상태
	NeedHeal UMETA(DisplayName = "NeedHeal"),
	NeedUlti UMETA(DisplayName = "NeedUlti"),
	Attacking UMETA(DisplayName = "Attacking"),    // 공격 상태
	UsingSkillA UMETA(DisplayName = "Using Skill A"), // 스킬 A 사용 중
	UsingSkillB UMETA(DisplayName = "Using Skill B"), // 스킬 B 사용 중
	UsingUltimate UMETA(DisplayName = "Using Ultimate"), // 궁극기 사용 중
	Dead UMETA(DisplayName = "Dead")               // 죽은 상태
};


UCLASS()
class IFPROJECT_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AAICharacter();

	// 직업에 맞는 캐릭터 클래스 참조
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

	FVector TargetLocation = FVector(-390.f, -60.f, 200.f);	// 임의의 목표

	void MoveToTaggedActor();


	UPROPERTY(EditAnywhere, Category = "AI")
	AActor* ATeamTarget;

	UPROPERTY(EditAnywhere, Category = "AI")
	AActor* BTeamTarget;

	AActor* CurrentTargetActor = nullptr;

	//ai캐릭터 동시스킬 예방
	UPROPERTY(BlueprintReadOnly)
	bool bIsUsingSkill = false;

	void MoveRandomlyInRange();

public:
	virtual void Tick(float DetaTime) override;

	// 탱커
	virtual void AIBarrierSkillTick() {}
	virtual void AIShockStrikeSkillTick() {}
	virtual void AITankerUltimateSkillTick() {}

	// 딜러
	virtual void AIDashSkillTick() {}
	virtual void AIStunSkillTick() {}
	virtual void AIDealerUltimateSkillTick() {}

	// 힐러
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
