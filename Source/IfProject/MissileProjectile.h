#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class AMissileProjectile : public AActor
{
    GENERATED_BODY()

public:
    AMissileProjectile();

    void SetHomingTarget(AActor* NewTarget);
    void SetDamage(float NewDamage);

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* MovementComponent;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

private:
    UPROPERTY(VisibleAnywhere)
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;


    float Damage = 70.0f;
};
