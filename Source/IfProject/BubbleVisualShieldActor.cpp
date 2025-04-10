#include "BubbleVisualShieldActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"

ABubbleVisualShieldActor::ABubbleVisualShieldActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// ��Ʈ �� ����
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	// ���� �޽� ���� �� ����
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(RootComponent);
	ShieldMesh->SetRelativeScale3D(FVector(0.7f));
	ShieldMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABubbleVisualShieldActor::BeginPlay()
{
	Super::BeginPlay();

	// ���� �ð� �� ����
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		SetLifeSpan(LifeTime);
		});
}
