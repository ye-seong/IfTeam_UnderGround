#include "BP_UserInterface.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UBP_UserInterface::AddKillLog(FString KillMessage)
{
    if (KillLogBox)
    {
        // 킬 로그 항목을 생성하여 KillLogBox에 추가
        UTextBlock* NewKillLogText = NewObject<UTextBlock>(this);
        NewKillLogText->SetText(FText::FromString(KillMessage));
        KillLogBox->AddChild(NewKillLogText);  // KillLogBox에 킬 로그 항목 추가
    }
}