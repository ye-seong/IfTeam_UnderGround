// Copyright Epic Games, Inc. All Rights Reserved.

#include "IfProjectProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BarrierActor.h"

AIfProjectProjectile::AIfProjectProjectile()
{
    // 충돌 컴포넌트 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 오버랩만 발생하도록 설정
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComponent->SetNotifyRigidBodyCollision(true); // 물리적 충돌 이벤트를 받기 위해
    CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AIfProjectProjectile::OnOverlapBegin);  // 오버랩 이벤트
    CollisionComponent->CanCharacterStepUpOn = ECB_No;
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);

    CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // 적군(Pawn)과 오버랩
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);  // 장애물 등과 오버랩
    CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);  // 자신과는 충돌하지 않음
    CollisionComponent->SetGenerateOverlapEvents(true);
    // 루트 컴포넌트로 설정
    RootComponent = CollisionComponent;

    // 발사체 이동 컴포넌트 설정
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
    ProjectileMovementComponent->InitialSpeed = 3000.f;
    ProjectileMovementComponent->MaxSpeed = 3000.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;  // 튕기지 않게 설정
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

    // 메시 컴포넌트 생성 (선택적)
    ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMeshComponent->SetupAttachment(RootComponent);

    // 메시 설정 (선택적)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
    if (Mesh.Succeeded())
    {
        ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
    }
    ProjectileMeshComponent->SetVisibility(true);
    ProjectileMeshComponent->SetWorldScale3D(FVector(0.3f));

    // 3초 후 파괴
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

    //  캐릭터 충돌
    if (ABaseCharacter* TargetChar = Cast<ABaseCharacter>(OtherActor))
    {

        if (ShooterTeamID != -1 && ShooterTeamID != TargetChar->TeamID)
        {
            TargetChar->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
        }

        Destroy();
        return;
    }

    //  방벽 충돌
    if (ABarrierActor* Barrier = Cast<ABarrierActor>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("--------------Barrier overlap called by % s"), *ShooterName);
        if (ShooterTeamID != -1 && ShooterTeamID != Barrier->GetTeamID())
        {
            Barrier->TakeBarrierDamage(Damage);

            // 누가 쐈는지 확인용 출력
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow,
                FString::Printf(TEXT("[Barrier] Hit by: %s"), *ShooterName));
        }

        Destroy();
        return;
    }

    // 기타 (예: 벽, 구조물)
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