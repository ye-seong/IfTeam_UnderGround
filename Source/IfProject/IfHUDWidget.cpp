// Fill out your copyright notice in the Description page of Project Settings.


#include "IfHUDWidget.h"
#include "Components/VerticalBox.h"
#include "KillLogEntry.h"
#include "BaseCharacter.h"


void UIfHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // HUD �ʱ�ȭ �۾�
    InitializeHUD();
}

void UIfHUDWidget::InitializeHUD()
{
    // ���⿡ HUD�� ���õ� �ʱ�ȭ �ڵ� �ۼ�
    // ��: ų�α� �ڽ��� ȭ�鿡 �߰��ϴ� �ڵ� ��
    if (KillLogBox)
    {
        // KillLogBox �ʱ�ȭ ����
        KillLogBox->ClearChildren();  // ���� �׸� �ʱ�ȭ
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
