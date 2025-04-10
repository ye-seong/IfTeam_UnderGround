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
	// ������
	ABubbleVisualShieldActor();

protected:
	// ���� ���� ��
	virtual void BeginPlay() override;

	// ���� �޽�
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ShieldMesh;

	// ��Ʈ ��
	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootScene;

	// �ڵ� ���ű��� ���� �ð�
	UPROPERTY(EditAnywhere, Category = "Visual")
	float LifeTime = 5.f;
};