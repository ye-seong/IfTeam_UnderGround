// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrierActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "IfProjectProjectile.h"

// Sets default values
ABarrierActor::ABarrierActor()
{
	PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    // �浹 ����
 /*   CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    CollisionBox->SetGenerateOverlapEvents(true);*/
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

    //�Ѿ� �浹
    CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
    //����� �溮 ���
    CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    //�ٸ� �޽� ����
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ABarrierActor::BeginPlay()
{
	Super::BeginPlay();
	
    // �ʱ� ü�� ����
    CurrentHealth = MaxHealth;

    // ��Ƽ���� ����
    if (NormalMaterial && Mesh)
    {
        Mesh->SetMaterial(0, NormalMaterial);
    }


    // ?? ���� �߰�!
   /* CollisionBox->OnComponentHit.AddDynamic(this, &ABarrierActor::OnBarrierHit);*/
    // ������ �̺�Ʈ ���ε�
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABarrierActor::OnBarrierBeginOverlap);

    // ���� �ð� �� �ı� Ÿ�̸� ����
    GetWorld()->GetTimerManager().SetTimer(
        DestroyTimerHandle,
        this,
        &ABarrierActor::DestroyBarrier,
        LifeTime,
        false
    );
}

void ABarrierActor::TakeBarrierDamage(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
        FString::Printf(TEXT("Barrier HP: %.1f / Max: %.1f"), CurrentHealth, MaxHealth));

    // ���� ���·� ���� ����
    if (CurrentHealth < MaxHealth * 0.8f && DamagedMaterial && Mesh)
    {
        Mesh->SetMaterial(0, DamagedMaterial);
    }


    // ü���� 0 ���ϸ� �ı�
    if (CurrentHealth <= 0.f)
    {
        DestroyBarrier();
    }
}


void ABarrierActor::DestroyBarrier()
{
    if (bIsDead) return;
    bIsDead = true;
    Destroy();
}

void ABarrierActor::OnBarrierBeginOverlap(UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AIfProjectProjectile* Projectile = Cast<AIfProjectProjectile>(OtherActor);
    if (!Projectile)
    {
        return;
    }

    // �Ʊ� �Ѿ��̸� ���� (������ ����)
    if (Projectile->ShooterTeamID != TeamID)
    {
        TakeBarrierDamage(Projectile->GetDamage());

    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,
            TEXT("[Barrier] Friendly fire ignored."));
    }
    Projectile->Destroy();

}

//void ABarrierActor::OnBarrierHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
//    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//    AIfProjectProjectile* Projectile = Cast<AIfProjectProjectile>(OtherActor);
//    if (!Projectile) return;
//
//    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
//        FString::Printf(TEXT("Barrier TeamID: %d / Projectile TeamID: %d"), TeamID, Projectile->ShooterTeamID));
//
//    if (Projectile->ShooterTeamID != TeamID)
//    {
//        TakeBarrierDamage(Projectile->GetDamage());
//        Projectile->Destroy();
//    }
//}
