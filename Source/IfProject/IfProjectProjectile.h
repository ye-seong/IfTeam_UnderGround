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
    // ���Ǿ� �ݸ��� ������Ʈ�Դϴ�.
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    USphereComponent* CollisionComponent;

    // �߻�ü �̵� ������Ʈ�Դϴ�.
    UPROPERTY(VisibleAnywhere, Category = Movement)
    UProjectileMovementComponent* ProjectileMovementComponent;

    // �߻�ü �޽�
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    UStaticMeshComponent* ProjectileMeshComponent;

    // �߻�ü ��Ƽ����
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

    //ĳ������ �Ѿ� ������ �ٸ��� �ϱ� ���ؼ�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float NormalAttackDamage = 0.f;
    void SetDamage(float NewDamage);

    void SetHomingTarget(AActor* Target); 
    void LaunchProjectile(FVector Direction, float Speed);

    UProjectileMovementComponent* GetProjectileMovement() const;
};

