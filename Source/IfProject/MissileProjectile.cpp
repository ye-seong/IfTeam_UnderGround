#include "MissileProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

AMissileProjectile::AMissileProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    // 콜리전 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(10.f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->SetNotifyRigidBodyCollision(true);
    //CollisionComponent->OnComponentHit.AddDynamic(this, &AMissileProjectile::OnHit);
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMissileProjectile::OnOverlapBegin);
    RootComponent = CollisionComponent;

    // 메시
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 이동
    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
    MovementComponent->UpdatedComponent = CollisionComponent;
    MovementComponent->InitialSpeed = 3000.f;
    MovementComponent->MaxSpeed = 3000.f;
    MovementComponent->bRotationFollowsVelocity = true;
    MovementComponent->bIsHomingProjectile = true;


    InitialLifeSpan = 5.0f;
}

void AMissileProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AMissileProjectile::SetHomingTarget(AActor* NewTarget)
{
    if (!NewTarget) return;
    USceneComponent* TargetComponent = NewTarget->GetRootComponent();
    if (!TargetComponent) return;

    MovementComponent->bIsHomingProjectile = true;
    MovementComponent->HomingTargetComponent = TargetComponent;
    MovementComponent->HomingAccelerationMagnitude = 10000.f;
}

void AMissileProjectile::SetDamage(float NewDamage)
{
    Damage = NewDamage;
}

void AMissileProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        OtherActor->TakeDamage(
            Damage,
            FDamageEvent(),
            GetInstigatorController(),
            this
        );

        Destroy();
        UE_LOG(LogTemp, Warning, TEXT("Missile Hit Target: %s"), *OtherActor->GetName());
    }
}

void AMissileProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        ABaseCharacter* OtherCharacter = Cast<ABaseCharacter>(OtherActor); // OtherActor가 ABaseCharacter인지 확인

        if (OtherCharacter)
        {
            ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner()); // 소유자 캐릭터 가져오기

            if (OwnerCharacter)
            {
                int32 MyTeamID = OwnerCharacter->TeamID; // 자기 자신의 TeamID 가져오기
                int32 OtherTeamID = OtherCharacter->TeamID; // 겹친 캐릭터의 TeamID 가져오기

                if (MyTeamID != OtherTeamID)  // 자기 자신과 다른 팀이면 적군
                {
                    // 적군일 때만 데미지 처리
                    UE_LOG(LogTemp, Warning, TEXT("Missile hit enemy: %s"), *OtherActor->GetName());
                    OtherActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
                    Destroy();  // 미사일 제거
                }
            }
        }
    }
}