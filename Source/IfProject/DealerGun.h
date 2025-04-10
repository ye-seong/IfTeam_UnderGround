// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "DealerGun.generated.h"

/**
 * 
 */
UCLASS()
class IFPROJECT_API ADealerGun : public AGun
{
	GENERATED_BODY()
	
public:
	ADealerGun();

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;
};
