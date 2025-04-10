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
    // ���ε� ���̸� �׳� ����
    if (bIsReloading) return;

    // ź�� ���� �� ���ε��� �� �� ����
    if (CurrBulletNum <= 0)
    {
        if (!bIsReloading)  // �� �̰� ��� ��� ������ Ȯ���ϰ�
        {
            bIsReloading = true;

            GetWorld()->GetTimerManager().SetTimer(
                ReloadTimerHandle, this, &AGun::FinishReload, 2.0f, false);
        }
        return;
    }

    // ���� �߻�
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
        // �߻� ��ġ �� ���� ���
        FVector FireStartLocation = this->GetActorLocation() + this->GetActorForwardVector() * 100.0f;
        FVector FireDirection = Enemy->GetActorLocation() - FireStartLocation;
        FireDirection.Normalize();  // ���� ���͸� ����ȭ

        // �߻�ü ����
        FActorSpawnParameters SpawnParams;
        AIfProjectProjectile* Projectile = GetWorld()->SpawnActor<AIfProjectProjectile>(ProjectileClass, FireStartLocation, FireDirection.Rotation(), SpawnParams);

        if (Projectile)
        {
            // �߻�ü�� ��ǥ �������� �߻�
            Projectile->LaunchProjectile(FireDirection, 3000.0f);  // 3000�� �ӵ� ����
            ABaseCharacter* OwnerChar = Cast<ABaseCharacter>(GetOwner());
            if (OwnerChar)
            {
                Projectile->SetDamage(OwnerChar->GetNormalAttackDamage());  // ?????? ????
                Projectile->ShooterTeamID = OwnerChar->TeamID;
                Projectile->ShooterName = OwnerChar->GetName();
                Projectile->SetOwner(OwnerChar);  // ?? ????! ��????? Owner?? ????!
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

