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
        UE_LOG(LogTemp, Warning, TEXT("ã�� �±� ����: %s"), *Actor->GetName());
    }
    if (ActorToSpawn)  // Ŭ������ ����� ���Դ��� Ȯ��
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