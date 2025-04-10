// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPointActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnPointActor::ASpawnPointActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnPointActor::BeginPlay()
{
	Super::BeginPlay();
	//SpawnActorAtPoint();
}

// Called every frame
void ASpawnPointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnPointActor::SpawnActorAtPoint()
{
    TArray < AActor* > TaggedActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Spawn"), TaggedActors);

    for (AActor* Actor : TaggedActors)
    {
        UE_LOG(LogTemp, Warning, TEXT("찾은 태그 액터: %s"), *Actor->GetName());
    }
    if (ActorToSpawn)  // 클래스가 제대로 들어왔는지 확인
    {
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(ActorToSpawn, GetActorLocation(), GetActorRotation(), SpawnParams);

        UE_LOG(LogTemp, Warning, TEXT("success! %s"), *ActorToSpawn->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("failed!"));
    }
}