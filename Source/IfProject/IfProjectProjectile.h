// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IfProjectProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config=Game)
class AIfProjectProjectile : public AActor
{
    GENERATED_BODY()

public:
    AIfProjectProjectile();

    int32 ShooterTeamID = -1;
    FORCEINLINE void SetShooterTeamID(int32 InTeamID) { ShooterTeamID = InTeamID; }
    FORCEINLINE float GetDamage() const { return Damage; }

protected:
    // 스피어 콜리전 컴포넌트입니다.
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    USphereComponent* CollisionComponent;

    // 발사체 이동 컴포넌트입니다.
    UPROPERTY(VisibleAnywhere, Category = Movement)
    UProjectileMovementComponent* ProjectileMovementComponent;

    // 발사체 메시
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    UStaticMeshComponent* ProjectileMeshComponent;

    // 발사체 머티리얼
    UPROPERTY(VisibleDefaultsOnly, Category = Movement)
    UMaterialInstanceDynamic* ProjectileMaterialInstance;

    FTimerHandle BuffTimerHandle;

    void FireInDirection(const FVector& ShootDirection);

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    float Damage = 20.0f;

    UPROPERTY()
    FString ShooterName;

    //캐릭마다 총알 데미지 다르게 하기 위해서
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float NormalAttackDamage = 0.f;
    void SetDamage(float NewDamage);

    void SetHomingTarget(AActor* Target); 
    void LaunchProjectile(FVector Direction, float Speed);

    UProjectileMovementComponent* GetProjectileMovement() const;
};

