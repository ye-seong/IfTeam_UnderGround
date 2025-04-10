// Fill out your copyright notice in the Description page of Project Settings.


#include "DealerGun.h"

ADealerGun::ADealerGun()
{
    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    RootComponent = GunMesh;
    MaxBulletNum = 30;
    CurrBulletNum = MaxBulletNum;
    }