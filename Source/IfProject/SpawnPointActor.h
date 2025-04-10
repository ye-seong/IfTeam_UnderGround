// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPointActor.generated.h"

UCLASS()
class IFPROJECT_API ASpawnPointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPointActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 에디터에서 설정 가능한 스폰할 클래스 변수
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> ActorToSpawn;  // 스폰할 액터 클래스

	UFUNCTION(BlueprintCallable)
	void SpawnActorAtPoint();  // 실제 스폰 함수

};
