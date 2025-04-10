#include "BP_UserInterface.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UBP_UserInterface::AddKillLog(FString KillMessage)
{
    if (KillLogBox)
    {
        // ų �α� �׸��� �����Ͽ� KillLogBox�� �߰�
        UTextBlock* NewKillLogText = NewObject<UTextBlock>(this);
        NewKillLogText->SetText(FText::FromString(KillMessage));
        KillLogBox->AddChild(NewKillLogText);  // KillLogBox�� ų �α� �׸� �߰�
    }
}