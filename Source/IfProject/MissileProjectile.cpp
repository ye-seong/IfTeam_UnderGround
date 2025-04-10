#include "MissileProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

AMissileProjectile::AMissileProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    // �ݸ��� ����
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(10.f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->SetNotifyRigidBodyCollision(true);
    //CollisionComponent->OnComponentHit.AddDynamic(this, &AMissileProjectile::OnHit);
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMissileProjectile::OnOverlapBegin);
    RootComponent = CollisionComponent;

    // �޽�
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // �̵�
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
        ABaseCharacter* OtherCharacter = Cast<ABaseCharacter>(OtherActor); // OtherActor�� ABaseCharacter���� Ȯ��

        if (OtherCharacter)
        {
            ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner()); // ������ ĳ���� ��������

            if (OwnerCharacter)
            {
                int32 MyTeamID = OwnerCharacter->TeamID; // �ڱ� �ڽ��� TeamID ��������
                int32 OtherTeamID = OtherCharacter->TeamID; // ��ģ ĳ������ TeamID ��������

                if (MyTeamID != OtherTeamID)  // �ڱ� �ڽŰ� �ٸ� ���̸� ����
                {
                    // ������ ���� ������ ó��
                    UE_LOG(LogTemp, Warning, TEXT("Missile hit enemy: %s"), *OtherActor->GetName());
                    OtherActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
                    Destroy();  // �̻��� ����
                }
            }
        }
    }
}