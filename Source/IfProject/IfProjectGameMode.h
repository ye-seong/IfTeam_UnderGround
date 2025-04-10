// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JobType.h"
#include "AIController.h"
#include "AICharacter.h"
#include "IfProjectGameMode.generated.h"

USTRUCT()
struct FCaptureZoneStatus
{
	GENERATED_BODY();

	UPROPERTY()
	class AIFCaptureZone* Zone;

	UPROPERTY()
	bool bCapturedByTeamA = false;

	UPROPERTY()
	bool bCapturedByTeamB = false;
};

UCLASS(minimalapi)
class AIfProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIfProjectGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnClasses")
	TSubclassOf<APawn> TankerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnClasses")
	TSubclassOf<APawn> DealerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnClasses")
	TSubclassOf<APawn> HealerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<AAICharacter> TankerAIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<AAICharacter> DealerAIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<AAICharacter> HealerAIClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AAIController> AIControllerClass;

	TArray<APawn*> Players;

	// ���� ���� ����
	UFUNCTION(BlueprintCallable)
	void SpawnPlayerCharacter(AController* Controller, EJobType JobType, FString name);

	// AI ���� �Լ�
	UFUNCTION(BlueprintCallable)
	virtual void SpawnAICharacters(EJobType JobType, FVector PlayerLocation);

	FString GetJobTypeString(EJobType JobType);

	// �Ʊ� ���� ��ġ
	TArray<FVector> GetAStartPosition();

	// ���� ���� ��ġ
	TArray<FVector> GetBStartPosition();

	void SetSpawnPosition();

	// ����
	bool bIsContested;
	int TeamInControl;
	const float CaptureTime = 30.0f; // 30�� ���� �����ϸ� �¸�

	void UpdateCapturePoint(float DeltaTime);

	float GetCaptureProgress() const;

	void CaptureZoneTick(float DeltaTime);

protected:
	UPROPERTY()
	TArray<FCaptureZoneStatus> CaptureZoneStatusList;

	UPROPERTY(EditAnywhere, Category = "Capture")
	float CaptureTimePerZone = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Capture")
	float MaxGameTime = 180.0f;

	UPROPERTY(EditAnywhere, Category = "Capture")
	float WinDelayAfterCapture = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Capture")
	int32 CaptureOrderIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Capture")
	FName ZoneName = "A";

	UPROPERTY(VisibleAnywhere, Category = "Capture")
	float CaptureProgress = 0.0f;

	int32 CurrentCaptureIndex = 0;

	float GameTimer = 0.0f;
};



