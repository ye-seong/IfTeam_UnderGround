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

    // 팀 구분용 ID (0 = 레드, 1 = 블루 등)
    UPROPERTY(BlueprintReadOnly)
    int32 TeamID;

    // 최대 체력
    UPROPERTY(EditAnywhere, Category = "Barrier Stats")
    float MaxHealth =120.f;

    // 현재 체력
    UPROPERTY(VisibleAnywhere, Category = "Barrier Stats")
    float CurrentHealth;

    // 지속 시간
    UPROPERTY(EditAnywhere, Category = "Barrier Stats")
    float LifeTime =8.f;

    // 충돌 박스
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;

    // 시각용 메시
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* Mesh;

    // 외부에서 데미지를 받을 때 호출
    void TakeBarrierDamage(float DamageAmount);

    // 머티리얼 설정
    UPROPERTY(EditAnywhere, Category = "Material")
    UMaterialInstance* NormalMaterial;

    UPROPERTY(EditAnywhere, Category = "Material")
    UMaterialInstance* DamagedMaterial;


    // 방벽의 팀 ID 설정
    void SetTeamID(int32 NewTeamID)
    {
        TeamID = NewTeamID;
    }

    // 방벽의 팀 ID 반환
    int32 GetTeamID() const
    {
        return TeamID;
    }

protected:
    virtual void BeginPlay() override;

    // 방벽 파괴 처리
    void DestroyBarrier();

    // 지속 시간 타이머 핸들
    FTimerHandle DestroyTimerHandle;

    // 오버랩 처리 함수
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
