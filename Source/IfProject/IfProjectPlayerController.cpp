#include "IfProjectPlayerController.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
//#include "BP_UserInterface.h"


void AIfProjectPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AIfProjectPlayerController::NotifyKillLog(ABaseCharacter* KillerCharacter, ABaseCharacter* VictimCharacter)
{
    //if (BPUserInterfaceWidget)
    //{
    //    UBP_UserInterface* UserInterface = Cast<UBP_UserInterface>(BPUserInterfaceWidget);
    //    if (UserInterface)
    //    {
    //        FString KillMessage = FString::Printf(TEXT("%s killed %s"), *KillerCharacter->GetName(), *VictimCharacter->GetName());
    //        UserInterface->AddKillLog(KillMessage);  // 킬로그 UI에 전달
    //    }
    //}
}