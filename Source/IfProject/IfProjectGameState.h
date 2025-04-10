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
 * 오버워치식 점령전 게임 상태
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

    /** 순서대로 점령해야 할 거점 리스트 (예: A, B) */
    UPROPERTY(Replicated, BlueprintReadOnly)
    TArray<FObjectiveStatus> Objectives;

    /** 현재 공격팀 이름 (예: "TeamA") */
    UPROPERTY(Replicated, BlueprintReadOnly)
    FName AttackingTeam;

    /** 현재 수비팀 이름 (예: "TeamB") */
    UPROPERTY(Replicated, BlueprintReadOnly)
    FName DefendingTeam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
    FName ZoneName;

    /** 현재 점령 중인 인덱스 (0 = A 지점, 1 = B 지점) */
    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 CurrentObjectiveIndex;

    UFUNCTION(BlueprintCallable)
    int32 GetCurrentObjectiveIndex() const { return CurrentObjectiveIndex; }

    UFUNCTION(BlueprintCallable)
    void AdvanceToNextObjective();  // 점령 완료 시 다음 거점 활성화

    /** 게임 종료 조건 확인용 */
    UFUNCTION(BlueprintCallable)
    bool IsAllObjectivesCaptured() const;

    /** 점령률 설정 함수 */
    void SetCaptureProgress(int32 Index, float Progress, FName Team);

    /** 상태 설정 함수 */
    void UpdateObjectiveState(int32 Index, EObjectiveState NewState);

    /** Replication 설정 */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};


