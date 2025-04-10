// Fill out your copyright notice in the Description page of Project Settings.


#include "IfProjectGameState.h"
#include "Net/UnrealNetwork.h"

AIfProjectGameState::AIfProjectGameState()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentObjectiveIndex = 0;
}

void AIfProjectGameState::SetCaptureProgress(int32 Index, float Progress, FName Team)
{
    if (!Objectives.IsValidIndex(Index)) return;

    FObjectiveStatus& Obj = Objectives[Index];

    // 진행 중인 팀이 맞는지 확인
    if (Obj.CapturingTeam != Team && Obj.CaptureProgress > 0.f)
    {
        // 팀 변경 시 초기화 (예: 점령 중인데 반대 팀 들어옴)
        Obj.CaptureProgress = 0.f;
        Obj.CapturingTeam = NAME_None;
        Obj.State = EObjectiveState::Contested;
        return;
    }

    // 팀 지정
    Obj.CapturingTeam = Team;
    Obj.CaptureProgress = FMath::Clamp(Progress, 0.f, 1.f);
    Obj.State = EObjectiveState::Capturing;

    // 점령 완료 시
    if (Obj.CaptureProgress >= 1.f)
    {
        Obj.State = EObjectiveState::Captured;
        Obj.CapturingTeam = Team;

        // 다음 거점 열기
        CurrentObjectiveIndex++;
        if (Objectives.IsValidIndex(CurrentObjectiveIndex))
        {
            Objectives[CurrentObjectiveIndex].State = EObjectiveState::Capturing;
        }
    }
}

void AIfProjectGameState::UpdateObjectiveState(int32 Index, EObjectiveState NewState)
{
    if (Objectives.IsValidIndex(Index))
    {
        Objectives[Index].State = NewState;
    }
}

bool AIfProjectGameState::IsAllObjectivesCaptured() const
{
    for (const FObjectiveStatus& Obj : Objectives)
    {
        if (Obj.State != EObjectiveState::Captured)
        {
            return false;
        }
    }
    return true;
}

void AIfProjectGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AIfProjectGameState, Objectives);
    DOREPLIFETIME(AIfProjectGameState, AttackingTeam);
    DOREPLIFETIME(AIfProjectGameState, DefendingTeam);
    DOREPLIFETIME(AIfProjectGameState, CurrentObjectiveIndex);
}

void AIfProjectGameState::AdvanceToNextObjective()
{
    CurrentObjectiveIndex++;
}



