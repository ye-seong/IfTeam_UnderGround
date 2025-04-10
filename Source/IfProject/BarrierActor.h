// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarrierActor.generated.h"

UCLASS()
class IFPROJECT_API ABarrierActor : public AActor
{
    GENERATED_BODY()

public:
    ABarrierActor();

    // �� ���п� ID (0 = ����, 1 = ��� ��)
    UPROPERTY(BlueprintReadOnly)
    int32 TeamID;

    // �ִ� ü��
    UPROPERTY(EditAnywhere, Category = "Barrier Stats")
    float MaxHealth =120.f;

    // ���� ü��
    UPROPERTY(VisibleAnywhere, Category = "Barrier Stats")
    float CurrentHealth;

    // ���� �ð�
    UPROPERTY(EditAnywhere, Category = "Barrier Stats")
    float LifeTime =8.f;

    // �浹 �ڽ�
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;

    // �ð��� �޽�
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* Mesh;

    // �ܺο��� �������� ���� �� ȣ��
    void TakeBarrierDamage(float DamageAmount);

    // ��Ƽ���� ����
    UPROPERTY(EditAnywhere, Category = "Material")
    UMaterialInstance* NormalMaterial;

    UPROPERTY(EditAnywhere, Category = "Material")
    UMaterialInstance* DamagedMaterial;


    // �溮�� �� ID ����
    void SetTeamID(int32 NewTeamID)
    {
        TeamID = NewTeamID;
    }

    // �溮�� �� ID ��ȯ
    int32 GetTeamID() const
    {
        return TeamID;
    }

protected:
    virtual void BeginPlay() override;

    // �溮 �ı� ó��
    void DestroyBarrier();

    // ���� �ð� Ÿ�̸� �ڵ�
    FTimerHandle DestroyTimerHandle;

    // ������ ó�� �Լ�
    UFUNCTION()
    void OnBarrierBeginOverlap(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

   /* UFUNCTION()
    void OnBarrierHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/

    bool bIsDead = false;
};
