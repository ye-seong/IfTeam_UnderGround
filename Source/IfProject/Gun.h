// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class ABaseCharacter;

#include "IfProjectProjectile.h"
#include "Gun.generated.h"

UCLASS()
class IFPROJECT_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

	void AIFire(ABaseCharacter* Enemy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AIfProjectProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	int32 CurrBulletNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	int32 MaxBulletNum = 30;

	bool bIsReloading = false;

	void FinishReload();

	void ManualReload();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	FTimerHandle ReloadTimerHandle;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TSubclassOf<class AIfProjectProjectile> GetProjectileClass() const { return ProjectileClass; }
};
