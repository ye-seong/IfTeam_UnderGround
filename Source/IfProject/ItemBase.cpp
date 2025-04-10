// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseCharacter.h"
#include "TimerManager.h"

AItemBase::AItemBase()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->SetSphereRadius(50.f);
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
}

void AItemBase::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionComponent && !CollisionComponent->OnComponentBeginOverlap.IsAlreadyBound(this, &AItemBase::OnOverlapBegin))
    {
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
    }
}


void AItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABaseCharacter* Character = Cast<ABaseCharacter>(OtherActor);
    UE_LOG(LogTemp, Warning, TEXT("HEY!! Overlapped with: %s"), *OtherActor->GetName());
    if (!Character)
    {
        return; // ĳ���Ͱ� �ƴϸ� ����
    }

    // ������ ȿ�� ���� �õ�
    if (ApplyItemEffect(Character))
    {
        // �������� ����� �浹 ��Ȱ��ȭ (�ı����� �ʰ� ����)
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);

        // ����� Ÿ�̸� ����
        GetWorldTimerManager().SetTimer(RespawnHandle, this, &AItemBase::RespawnItem, RespawnTime, false);
    }
}

bool AItemBase::ApplyItemEffect(ABaseCharacter* Character)
{
    if (!Character)
        return false;

    switch (this->ItemType)
    {
    case EItemType::HP:
    {
        float CurrentHealth = Character->CurrentHP;
        float MaxHealth = 200.0f;

        if (CurrentHealth >= MaxHealth)
            return false;

        float HealAmount = FMath::Min(HealthValue, MaxHealth - CurrentHealth);
        Character->Heal(HealAmount);
        return true;
    }

    case EItemType::Ultimate:
    {
        float CurrentGauge = Character->UltiNum;
        float MaxGauge = Character->MaxUltiNum;

        if (CurrentGauge >= MaxGauge)
            return false;

        float AddAmount = FMath::Min(UltimateGaugeValue, MaxGauge - CurrentGauge);
        Character->AddUltiNum(AddAmount);
        return true;
    }
    }

    return false;
}

void AItemBase::RespawnItem()
{
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);

    // �ݸ��� �缳�� (��¥ �߿�)
    if (CollisionComponent)
    {
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
        CollisionComponent->SetSphereRadius(50.f); // Ȥ�ö� �ٲ���� ��� ���

        // ������ �̺�Ʈ�� �� �ɷ� ���� ��� ����ε�
        if (!CollisionComponent->OnComponentBeginOverlap.IsAlreadyBound(this, &AItemBase::OnOverlapBegin))
        {
            CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
            UE_LOG(LogTemp, Warning, TEXT("Overlap binding complate"));
        }
    }

    // ���� ������
    switch (this->ItemType)
    {
    case EItemType::HP:
        MeshComponent->SetMaterial(0, HP_Material);
        break;
    case EItemType::Ultimate:
        MeshComponent->SetMaterial(0, Ultimate_Material);
        break;
    }
}
