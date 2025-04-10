#pragma once

#include "CoreMinimal.h"
#include "IfProjectCharacter.h" // �θ� Ŭ���� ���
#include "JobType.h"
#include "Gun.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "MissileProjectile.h"
#include "BubbleVisualShieldActor.h"
#include "BaseCharacter.generated.h"

UCLASS()
class IFPROJECT_API ABaseCharacter : public AIfProjectCharacter
{
    GENERATED_BODY()

public:
    ABaseCharacter();

    virtual void OnFire() override;

    virtual void Tick(float DeltaTime) override;

    // �г���
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Nickname")
    FString NickName;

    // ���� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Job")
    EJobType JobType;

    // �� ���� (0�� �Ʊ�, 1�� ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    int32 TeamID = 0;

    // ü��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 200.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHP;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AGun> GunClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ultimate")
    int32 UltiNum = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate")
    int32 MaxUltiNum = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
    AGun* Gun;

    // ���ظ� �� ĳ���͸� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    ABaseCharacter* LastHitByChar;
    
    ABaseCharacter* TargetEnemy;

    float LastFireTime;

    float FireRate;

    FVector SpawnPosition;

    // �ǰ� ó�� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Stats")
    virtual void ReceiveDamage(float DamageAmount);

    UPROPERTY(BlueprintReadOnly, Category = "Health")
    bool bIsDead = false;

    UPROPERTY(BlueprintReadWrite, Category = "Defense")
    bool bUnderTankerUltimate = false;

    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    TSubclassOf<AActor> BubbleShieldVisualClass;

    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    TSubclassOf<class ABarrierActor> BarrierClass; // ������ �溮 Ŭ����

    UFUNCTION(BlueprintCallable, Category = "Stats")
    virtual void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    virtual void Revive();

    UFUNCTION(BlueprintCallable, Category = "Role")
    virtual bool IsHealer() const { return false; }

    float GetBUnderTankerUltimate() const { return bUnderTankerUltimate; }
    void SetBUnderTankerUltimate(bool Value) { bUnderTankerUltimate = Value; }

    UFUNCTION(BlueprintCallable, Category = "Healing")
    virtual void ReceiveHealing(float HealAmount);

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AMissileProjectile> DealerUltimateMissileClass;

    UFUNCTION()
    void AddUltiNum(int32 Amount);

    UPROPERTY()
    bool bIsMovingToTarget = false;

    UPROPERTY()
    ABaseCharacter* TargetAlly = nullptr;

    UPROPERTY()
    FVector TargetLocation = FVector::ZeroVector;

    /* ���� �Լ� */

    // ��Ŀ
    virtual void PerformBarrierSkill(float& CurCooldown, float CoolTime, float Distance, TSubclassOf<ABarrierActor> BarrierClassInput);
    virtual void PerformShockStrikeSkill(float& CurrCooldown, float CoolTime, float ImpactRadius, float ImpactDamage);
    virtual void PerformTankerUltimateSkill(float Duration);

    // ����
    virtual void PerformDashSkill(float& CurrCooldown, float CoolTime);
    virtual void PerformStunSkill(float& CurrCooldown, float CoolTime);
    void DisableCharacter(float Duration);
    virtual void PerformDealerUltimateSkill(FTimerHandle& UltimateDelayHandle);
    void LaunchMissileAtTarget(AActor* Target);

    // ����
    ABaseCharacter* FindClosestAlly();
    virtual void PerformTeleportHealSkill(float& CurrCooldown, float HealerCoolDown);
    //virtual void PerformTeleportHealSkillTick(FVector TargetLocation, ABaseCharacter* character, bool& bIsMovingToTarget, ABaseCharacter* TargetAlly, float DeltaTime);
     /** ??????? ?? ??? ??? Tick */
    void PerformTeleportHealSkillTick(ABaseCharacter* Character, float DeltaTime);


    ABaseCharacter* FindFurthestAlly();
    virtual void PerformHealShieldSkill(float& CurrCooldown, float UltimateDuration, FTimerHandle HealSpeedTimerHandle, float DeltaTime);
    void ResetHealSpeed();
    virtual void PerformHealerUltimateSkill();
    void UltimateRevive();
    
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    void KillLog(AActor* Killer);

    FTimerHandle BuffTimerHandle;
    FTimerHandle ShieldTimerHandle;

    FVector  ReviveLocation;
    FTimerHandle ReviveTimerHandle;


    UPROPERTY()
    ABubbleVisualShieldActor* CurrentShieldVisual = nullptr;

    float GetNormalAttackDamage() const { return NormalAttackDamage; }
protected:
    virtual void BeginPlay() override;

    virtual void HandleDeath();

    //void Respawn();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float NormalAttackDamage = 20.f;


private:

    // ��Ȱ Ÿ�̸� �ڵ鷯
    FTimerHandle RespawnTimerHandle;
};



