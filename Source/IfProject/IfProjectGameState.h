// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IfProjectGameState.generated.h"

UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
    Locked,
    Capturing,
    Contested,
    Captured
};

USTRUCT(BlueprintType)
struct FObjectiveStatus
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadOnly)
    FName ZoneName;

    UPROPERTY(BlueprintReadOnly)
    float CaptureProgress = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    EObjectiveState State = EObjectiveState::Locked;

    UPROPERTY(BlueprintReadOnly)
    FName CapturingTeam;
};

/**
 * ������ġ�� ������ ���� ����
 */
UCLASS()
class AIfProjectGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AIfProjectGameState();

    UPROPERTY(BlueprintReadOnly)
    float CurrentZoneAGauge;

    UPROPERTY(BlueprintReadOnly)
    float CurrentZoneBGauge;

    UPROPERTY(BlueprintReadOnly)
    float CurrentZoneASubGauge;

    UPROPERTY(BlueprintReadOnly)
    float CurrentZoneBSubGauge;

    UPROPERTY(BlueprintReadOnly)
    int32 CapturingTeam;

    UPROPERTY(BlueprintReadOnly)
    bool Gameover;

    /** ������� �����ؾ� �� ���� ����Ʈ (��: A, B) */
    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<FObjectiveStatus> Objectives;

    /** ���� ������ �̸� (��: "TeamA") */
    UPROPERTY(Replicated, BlueprintReadOnly)
    FName AttackingTeam;

    /** ���� ������ �̸� (��: "TeamB") */
    UPROPERTY(Replicated, BlueprintReadOnly)
    FName DefendingTeam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
    FName ZoneName;

    /** ���� ���� ���� �ε��� (0 = A ����, 1 = B ����) */
    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 CurrentObjectiveIndex;

    UFUNCTION(BlueprintCallable)
    int32 GetCurrentObjectiveIndex() const { return CurrentObjectiveIndex; }

    UFUNCTION(BlueprintCallable)
    void AdvanceToNextObjective();  // ���� �Ϸ� �� ���� ���� Ȱ��ȭ

    /** ���� ���� ���� Ȯ�ο� */
    UFUNCTION(BlueprintCallable)
    bool IsAllObjectivesCaptured() const;

    /** ���ɷ� ���� �Լ� */
    void SetCaptureProgress(int32 Index, float Progress, FName Team);

    /** ���� ���� �Լ� */
    void UpdateObjectiveState(int32 Index, EObjectiveState NewState);

    /** Replication ���� */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};


