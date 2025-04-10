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

    // 충돌 설정
 /*   CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    CollisionBox->SetGenerateOverlapEvents(true);*/
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

    //총알 충돌
    CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
    //사람은 방벽 통과
    CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    //다른 메시 무시
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ABarrierActor::BeginPlay()
{
	Super::BeginPlay();
	
    // 초기 체력 설정
    CurrentHealth = MaxHealth;

    // 머티리얼 설정
    if (NormalMaterial && Mesh)
    {
        Mesh->SetMaterial(0, NormalMaterial);
    }


    // ?? 여기 추가!
   /* CollisionBox->OnComponentHit.AddDynamic(this, &ABarrierActor::OnBarrierHit);*/
    // 오버랩 이벤트 바인딩
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABarrierActor::OnBarrierBeginOverlap);

    // 지속 시간 후 파괴 타이머 시작
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

    // 피해 상태로 재질 변경
    if (CurrentHealth < MaxHealth * 0.8f && DamagedMaterial && Mesh)
    {
        Mesh->SetMaterial(0, DamagedMaterial);
    }


    // 체력이 0 이하면 파괴
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

    // 아군 총알이면 무시 (데미지 없음)
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
