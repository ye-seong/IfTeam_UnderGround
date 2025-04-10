// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BubbleVisualShieldActor.generated.h"

UCLASS()
class IFPROJECT_API ABubbleVisualShieldActor : public AActor
{
	GENERATED_BODY()

public:
	// 생성자
	ABubbleVisualShieldActor();

protected:
	// 게임 시작 시
	virtual void BeginPlay() override;

	// 쉴드 메시
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ShieldMesh;

	// 루트 씬
	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootScene;

	// 자동 제거까지 지속 시간
	UPROPERTY(EditAnywhere, Category = "Visual")
	float LifeTime = 5.f;
};