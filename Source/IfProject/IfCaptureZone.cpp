// Fill out your copyright notice in the Description page of Project Settings.


#include "IFCaptureZone.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacter.h"
#include "IfProjectGameState.h"
#include "Engine/Engine.h"

AIFCaptureZone::AIFCaptureZone()
{
    PrimaryActorTick.bCanEverTick = true;

    // 캡처 박스 컴포넌트 생성
    CaptureBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CaptureBox"));
    RootComponent = CaptureBox;

    CaptureBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CaptureBox->SetCollisionObjectType(ECC_WorldDynamic);
    CaptureBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    CaptureBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    CaptureBox->SetGenerateOverlapEvents(true);

    /*CaptureProgress = 0.0f;
    CaptureOrderIndex = 0;*/
}

void AIFCaptureZone::Tick(float DeltaTime)
{
    if (!IsEnd) {
        UpdateCapture(DeltaTime);
        UpdateSubCapture(DeltaTime);
        UpdateCaptureInfo(DeltaTime);

        //FString DebugMsg1 = FString::Printf(TEXT("Gauge : %.1f, SubGauge : %.1f, Team: %d"), TeamA_Gauge, TeamA_SubGauge, CurrentCapturingTeam);
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, DebugMsg1);
        //FString DebugMsg2 = FString::Printf(TEXT("Gauge : %.1f, SubGauge : %.1f, Team: %d"), TeamB_Gauge, TeamB_SubGauge, CurrentCapturingTeam);
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DebugMsg2);
    }
}

void AIFCaptureZone::BeginPlay()
{
    Super::BeginPlay();

    if (CaptureBox)
    {
        CaptureBox->OnComponentBeginOverlap.AddDynamic(this, &AIFCaptureZone::OnPlayerBeginOverlap);
        CaptureBox->OnComponentEndOverlap.AddDynamic(this, &AIFCaptureZone::OnPlayerEndOverlap);
    }
}

void AIFCaptureZone::UpdateCaptureInfo(float DeltaTime)
{
    AIfProjectGameState* GS = GetWorld()->GetGameState<AIfProjectGameState>();
    if (GS)
    {
        GS->CurrentZoneAGauge = TeamA_Gauge;
        GS->CurrentZoneBGauge = TeamB_Gauge;
        GS->CurrentZoneASubGauge = TeamA_SubGauge;
        GS->CurrentZoneBSubGauge = TeamB_SubGauge;
        GS->CapturingTeam = CurrentCapturingTeam;

    }
}
void AIFCaptureZone::UpdateCapture(float DeltaTime)
{
    if (TeamA_Gauge >= CaptureTime || TeamB_Gauge >= CaptureTime)
    {
        AIfProjectGameState* GS = GetWorld()->GetGameState<AIfProjectGameState>();

        IsEnd = true;
        GS->Gameover = true;
        return;
    }
    
    if (CurrentCapturingTeam == 0)
    {
        if (IsCloseMatch() && TeamA_Gauge >= 99) return;
        TeamA_Gauge += DeltaTime;
    }
    else if (CurrentCapturingTeam == 1)
    {
        TeamB_Gauge += DeltaTime;
    }
}

void AIFCaptureZone::UpdateSubCapture(float DeltaTime)
{
    TArray<int32> IDs;

    // 현재 거점에 있는 플레이어들 TeamID를 모두 가져옴
    for (ABaseCharacter* player : OverlappingPlayers) {
        IDs.Add(player->TeamID);
    }

    // 거점에 아무도 없을 때
    if (IDs.Num() == 0) {
        switch (CurrentCapturingTeam) {
        case (0):
            if (TeamB_SubGauge > 0) TeamB_SubGauge -= DeltaTime;
            break;
        case (1):
            if (TeamA_SubGauge > 0) TeamA_SubGauge -= DeltaTime;
            break;
        default:
            if (TeamB_SubGauge > 0) TeamB_SubGauge -= DeltaTime;
            else if (TeamA_SubGauge > 0) TeamA_SubGauge -= DeltaTime;
            break;
        }
        return;
    }

    // 한 팀만 있을 때 SubGauge가 오르도록 함
    if (IsCloseMatch())
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("IsCloseMatch!!!!!!!!!!!!!"));
    }
    else
    {
        switch (CurrentCapturingTeam) {
        case (0):                                                                
            if (IDs[0] == 0) {
                if (TeamB_SubGauge > 0) TeamB_SubGauge -= DeltaTime;
            }
            else if (IDs[0] == 1) {
                TeamB_SubGauge += DeltaTime;
            }
            break;
        case (1):
            if (IDs[0] == 0) {
                TeamA_SubGauge += DeltaTime;
            }
            else if (IDs[0] == 1) {
                if (TeamA_SubGauge > 0) TeamA_SubGauge -= DeltaTime;
            }
            break;
        default:
            if (IDs[0] == 0) {
                if (TeamB_SubGauge > 0) TeamB_SubGauge -= DeltaTime;
                else TeamA_SubGauge += DeltaTime;
            }
            else if (IDs[0] == 1) {
                if (TeamA_SubGauge > 0) TeamA_SubGauge -= DeltaTime;
                else TeamB_SubGauge += DeltaTime;
            }
            break;
        }
    }

    if (TeamA_SubGauge >= SubCaptureTime) {
        CurrentCapturingTeam = 0;
        TeamA_SubGauge = 0;
    }
    if (TeamB_SubGauge >= SubCaptureTime) {
        CurrentCapturingTeam = 1;
        TeamB_SubGauge = 0;
    }

}

// 플레이어가 들어올 때
void AIFCaptureZone::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    TArray<UPrimitiveComponent*> OverlappingComponents;
    CaptureBox->GetOverlappingComponents(OverlappingComponents);

    GetOverlappingPlayers(OverlappingComponents);
};

// 플레이어가 나갈 때
void AIFCaptureZone::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABaseCharacter* character = Cast<ABaseCharacter>(OtherActor);
    if (character && OverlappingPlayers.Contains(character))
    {
        OverlappingPlayers.Remove(character);
    }
}

// 현재 두 팀이 모두 거점에 있는지 없는지
bool AIFCaptureZone::IsCloseMatch()
{
    TArray<int32> A_IDs;
    TArray<int32> B_IDs;

    for (ABaseCharacter* character : OverlappingPlayers) {
        if (character->TeamID == 0) A_IDs.Add(0);
        else if (character->TeamID == 1) B_IDs.Add(1);
    }
    
    if (A_IDs.Num() > 0 && B_IDs.Num() > 0) return true;
    else return false;
}

void AIFCaptureZone::GetOverlappingPlayers(TArray<UPrimitiveComponent*> Components)
{
    TArray<int32> IDs;
    TArray<ABaseCharacter*> characters;

    for (UPrimitiveComponent* Comp : Components) {
        if (Comp) {
            ABaseCharacter* character = Cast<ABaseCharacter>(Comp->GetOwner());
            if (character) {
                characters.Add(character);
            }
        }
    }

    OverlappingPlayers = characters;
}

