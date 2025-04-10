// Fill out your copyright notice in the Description page of Project Settings.


#include "IfHUDWidget.h"
#include "Components/VerticalBox.h"
#include "KillLogEntry.h"
#include "BaseCharacter.h"


void UIfHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // HUD 초기화 작업
    InitializeHUD();
}

void UIfHUDWidget::InitializeHUD()
{
    // 여기에 HUD와 관련된 초기화 코드 작성
    // 예: 킬로그 박스를 화면에 추가하는 코드 등
    if (KillLogBox)
    {
        // KillLogBox 초기화 예시
        KillLogBox->ClearChildren();  // 기존 항목 초기화
    }
}

void UIfHUDWidget::AddKillLog(ABaseCharacter* KillerCharacter, ABaseCharacter* VictimCharacter)
{
    if (!KillLogBox || !KillLogEntryClass) return;

    UKillLogEntry* Entry = CreateWidget<UKillLogEntry>(this, KillLogEntryClass);

    FString Killer = KillerCharacter->NickName;
    FString Victim = VictimCharacter->NickName;
    int32 KillerID = KillerCharacter->TeamID;
    int32 VictimID = VictimCharacter->TeamID;

    if (Entry)
    {
        Entry->Setup(Killer, Victim, KillerID, VictimID);
        KillLogBox->AddChild(Entry);
    }
}
