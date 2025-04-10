// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Engine/World.h"
#include "IfprojectProjectile.h"
#include "BaseCharacter.h"


// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    ProjectileClass = AIfProjectProjectile::StaticClass();
    CurrBulletNum = MaxBulletNum;
}

void AGun::Fire()
{
    // 리로딩 중이면 그냥 무시
    if (bIsReloading) return;

    // 탄약 없을 때 리로딩은 딱 한 번만
    if (CurrBulletNum <= 0)
    {
        if (!bIsReloading)  // ★ 이거 사실 없어도 되지만 확실하게
        {
            bIsReloading = true;

            GetWorld()->GetTimerManager().SetTimer(
                ReloadTimerHandle, this, &AGun::FinishReload, 2.0f, false);
        }
        return;
    }

    // 정상 발사
    CurrBulletNum--;
}

void AGun::FinishReload()
{
    CurrBulletNum = MaxBulletNum;
    bIsReloading = false;
}

void AGun::ManualReload()
{
    if (bIsReloading || CurrBulletNum == MaxBulletNum) return;

    bIsReloading = true;
    GetWorld()->GetTimerManager().SetTimer(
        ReloadTimerHandle, this, &AGun::FinishReload, 2.0f, false);
}

void AGun::AIFire(ABaseCharacter* Enemy)
{
    if (Enemy)
    {
        // 발사 위치 및 방향 계산
        FVector FireStartLocation = this->GetActorLocation() + this->GetActorForwardVector() * 100.0f;
        FVector FireDirection = Enemy->GetActorLocation() - FireStartLocation;
        FireDirection.Normalize();  // 방향 벡터를 정규화

        // 발사체 생성
        FActorSpawnParameters SpawnParams;
        AIfProjectProjectile* Projectile = GetWorld()->SpawnActor<AIfProjectProjectile>(ProjectileClass, FireStartLocation, FireDirection.Rotation(), SpawnParams);

        if (Projectile)
        {
            // 발사체를 목표 방향으로 발사
            Projectile->LaunchProjectile(FireDirection, 3000.0f);  // 3000은 속도 설정
            ABaseCharacter* OwnerChar = Cast<ABaseCharacter>(GetOwner());
            if (OwnerChar)
            {
                Projectile->SetDamage(OwnerChar->GetNormalAttackDamage());  // ?????? ????
                Projectile->ShooterTeamID = OwnerChar->TeamID;
                Projectile->ShooterName = OwnerChar->GetName();
                Projectile->SetOwner(OwnerChar);  // ?? ????! ĳ????? Owner?? ????!
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("1111!"));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No target enemy detected."));
    }
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

