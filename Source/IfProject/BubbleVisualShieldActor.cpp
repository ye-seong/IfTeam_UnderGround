#include "BubbleVisualShieldActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"

ABubbleVisualShieldActor::ABubbleVisualShieldActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 루트 씬 생성
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	// 쉴드 메시 생성 및 설정
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(RootComponent);
	ShieldMesh->SetRelativeScale3D(FVector(0.7f));
	ShieldMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABubbleVisualShieldActor::BeginPlay()
{
	Super::BeginPlay();

	// 일정 시간 후 제거
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		SetLifeSpan(LifeTime);
		});
}
