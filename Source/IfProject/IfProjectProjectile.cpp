// Copyright Epic Games, Inc. All Rights Reserved.

#include "IfProjectProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BarrierActor.h"

AIfProjectProjectile::AIfProjectProjectile()
{
    // �浹 ������Ʈ ����
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // �������� �߻��ϵ��� ����
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComponent->SetNotifyRigidBodyCollision(true); // ������ �浹 �̺�Ʈ�� �ޱ� ����
    CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AIfProjectProjectile::OnOverlapBegin);  // ������ �̺�Ʈ
    CollisionComponent->CanCharacterStepUpOn = ECB_No;
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);

    CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // ����(Pawn)�� ������
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);  // ��ֹ� ��� ������
    CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);  // �ڽŰ��� �浹���� ����
    CollisionComponent->SetGenerateOverlapEvents(true);
    // ��Ʈ ������Ʈ�� ����
    RootComponent = CollisionComponent;

    // �߻�ü �̵� ������Ʈ ����
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
    ProjectileMovementComponent->InitialSpeed = 3000.f;
    ProjectileMovementComponent->MaxSpeed = 3000.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;  // ƨ���� �ʰ� ����
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

    // �޽� ������Ʈ ���� (������)
    ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMeshComponent->SetupAttachment(RootComponent);

    // �޽� ���� (������)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
    if (Mesh.Succeeded())
    {
        ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
    }
    ProjectileMeshComponent->SetVisibility(true);
    ProjectileMeshComponent->SetWorldScale3D(FVector(0.3f));

    // 3�� �� �ı�
    InitialLifeSpan = 10.0f;
}

void AIfProjectProjectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AIfProjectProjectile::LaunchProjectile(FVector Direction, float Speed)
{
    ProjectileMovementComponent->Velocity = Direction * Speed;
}

void AIfProjectProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

    //  ĳ���� �浹
    if (ABaseCharacter* TargetChar = Cast<ABaseCharacter>(OtherActor))
    {

        if (ShooterTeamID != -1 && ShooterTeamID != TargetChar->TeamID)
        {
            TargetChar->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
        }

        Destroy();
        return;
    }

    //  �溮 �浹
    if (ABarrierActor* Barrier = Cast<ABarrierActor>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("--------------Barrier overlap called by % s"), *ShooterName);
        if (ShooterTeamID != -1 && ShooterTeamID != Barrier->GetTeamID())
        {
            Barrier->TakeBarrierDamage(Damage);

            // ���� ������ Ȯ�ο� ���
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow,
                FString::Printf(TEXT("[Barrier] Hit by: %s"), *ShooterName));
        }

        Destroy();
        return;
    }

    // ��Ÿ (��: ��, ������)
    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue,
        FString::Printf(TEXT("[Projectile] Hit other object: %s"), *OtherActor->GetName()));

    Destroy();
}

void AIfProjectProjectile::SetDamage(float NewDamage)
{
    NormalAttackDamage = NewDamage;
}

void AIfProjectProjectile::SetHomingTarget(AActor * Target)
{
    if (!Target) return;

    USceneComponent* TargetComponent = Target->GetRootComponent();
    if (!TargetComponent) return;

    ProjectileMovementComponent->bIsHomingProjectile = true;
    ProjectileMovementComponent->HomingTargetComponent = TargetComponent;

    UE_LOG(LogTemp, Warning, TEXT("Guided Target Setting: %s"), *Target->GetName());
}

UProjectileMovementComponent* AIfProjectProjectile::GetProjectileMovement() const
{
    return ProjectileMovementComponent;
}