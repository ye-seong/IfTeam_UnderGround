// Copyright Epic Games, Inc. All Rights Reserved.

#include "IfProjectGameMode.h"
#include "IfProjectHUD.h"
#include "IfProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AIController.h"
#include "AICharacter.h"
#include "Kismet/GamePlayStatics.h"
#include "TankerAICharacter.h"
#include "DealerAICharacter.h"
#include "HealerAICharacter.h"
#include "IfProjectGameState.h"
#include "EngineUtils.h"
#include "IfCaptureZone.h"

AIfProjectGameMode::AIfProjectGameMode()
    : Super()
{
    // 기본 Pawn 없애기
    DefaultPawnClass = nullptr;

    // use our custom HUD class
    HUDClass = AIfProjectHUD::StaticClass();

    // AIControllerClass를 설정
    AIControllerClass = AAIController::StaticClass();  // 기본 AIController 클래스를 설정

    // 점령
    PrimaryActorTick.bCanEverTick = true;
    CaptureProgress = 0.0f;
    bIsContested = false;
    TeamInControl = -1;
}

float AIfProjectGameMode::GetCaptureProgress() const
{
    return CaptureProgress;
}

void AIfProjectGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 모든 점령 지점 수집
    for (TActorIterator<AIFCaptureZone> It(GetWorld()); It; ++It)
    {
        FCaptureZoneStatus NewStatus;
        NewStatus.Zone = *It;
        CaptureZoneStatusList.Add(NewStatus);
    }

    GameTimer = 0.0f;
    CurrentCaptureIndex = 0;
}

void AIfProjectGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateCapturePoint(DeltaTime);
    CaptureZoneTick(DeltaTime);
}


void AIfProjectGameMode::CaptureZoneTick(float DeltaTime)
{
    //GameTimer += DeltaTime;

    //int32 TeamA_Score = 0;
    //int32 TeamB_Score = 0;

    //for (FCaptureZoneStatus& Status : CaptureZoneStatusList)
    //{
    //    if (!Status.Zone) continue;

    //    // 순서 제한
    //    if (Status.Zone->CaptureOrderIndex > CurrentCaptureIndex)
    //        continue;

    //    FName Team = Status.Zone->GetCapturingTeam();
    //    float Progress = Status.Zone->GetCaptureProgress();

    //    if (Team == "TeamA" && Progress >= CaptureTimePerZone && !Status.bCapturedByTeamA)
    //    {
    //        Status.bCapturedByTeamA = true;
    //        UE_LOG(LogTemp, Warning, TEXT("Team A point %s OCCUPIED!"), *Status.Zone->ZoneName.ToString());

    //        if (Status.Zone->CaptureOrderIndex == CurrentCaptureIndex)
    //            CurrentCaptureIndex++;
    //    }
    //    else if (Team == "TeamB" && Progress >= CaptureTimePerZone && !Status.bCapturedByTeamB)
    //    {
    //        Status.bCapturedByTeamB = true;
    //        UE_LOG(LogTemp, Warning, TEXT("Team B point %s OCCUPIED!"), *Status.Zone->ZoneName.ToString());

    //        if (Status.Zone->CaptureOrderIndex == CurrentCaptureIndex)
    //            CurrentCaptureIndex++;
    //    }

    //    if (Status.bCapturedByTeamA) TeamA_Score++;
    //    if (Status.bCapturedByTeamB) TeamB_Score++;
    //}

    //// 승리 조건: 3개 전부 점령 시
    //if (TeamA_Score >= 3)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Team A WINS!"));
    //    // TODO: 게임 종료 처리
    //}
    //else if (TeamB_Score >= 3)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Team B WINS!"));
    //    // TODO: 게임 종료 처리
    //}

    //// 제한 시간 도달 시 점령 수 비교
    //if (GameTimer >= MaxGameTime)
    //{
    //    if (TeamA_Score > TeamB_Score)
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("Time out Team A WINS!"));
    //    }
    //    else if (TeamB_Score > TeamA_Score)
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("Time out Team B WINS!"));
    //    }
    //    else
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("DRAW!"));
    //    }

    //    // TODO: 게임 종료 처리
    //}
}

void AIfProjectGameMode::SpawnPlayerCharacter(AController* Controller, EJobType JobType, FString name)
{
    if (!Controller) return;

    TSubclassOf<APawn> SelectedClass = nullptr;

    switch (JobType)
    {
    case EJobType::Tanker:
        SelectedClass = TankerClass;
        break;
    case EJobType::Dealer:
        SelectedClass = DealerClass;
        break;
    case EJobType::Healer:
        SelectedClass = HealerClass;
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Is None"));
        return;
    }

    FVector SpawnLocation(0, 0, 300);  // 원하는 위치로 조정
    FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    APawn* NewPawn = GetWorld()->SpawnActor<APawn>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);

    ABaseCharacter* BaseChar = Cast<ABaseCharacter>(NewPawn);
    BaseChar->TeamID = 0;
    BaseChar->NickName = name;

    SpawnAICharacters(JobType, SpawnLocation);

    if (NewPawn)
    {
        Players.Add(NewPawn);
        Controller->Possess(NewPawn);
        UE_LOG(LogTemp, Log, TEXT("Spawn And Prossess Success"));
    }

    SetSpawnPosition();

}

void AIfProjectGameMode::SpawnAICharacters(EJobType PlayerJobType, FVector PlayerLocation)
{
    UWorld* World = GetWorld();
    if (!World) return;

    // 1. 적 팀 고정 3명 + 아군 2명 구성
    TArray<EJobType> AllJobs = { EJobType::Tanker, EJobType::Dealer, EJobType::Healer };
    TArray<EJobType> JobList = { EJobType::Tanker, EJobType::Dealer, EJobType::Healer };

    for (EJobType Job : AllJobs)
    {
        if (Job != PlayerJobType)
        {
            JobList.Add(Job); // 이건 안전함! AllJobs는 고정이고, JobList를 수정함
        }
    }

    for (int32 i = 0; i < JobList.Num(); ++i)
    {
        EJobType Job = JobList[i];
        FVector SpawnLocation = PlayerLocation + FVector(i * 300.f, 0.f, 0.f);
        TSubclassOf<APawn> SelectedClass = nullptr;

        switch (Job)
        {
        case EJobType::Tanker:  SelectedClass = TankerAIClass; break;
        case EJobType::Dealer:  SelectedClass = DealerAIClass; break;
        case EJobType::Healer:  SelectedClass = HealerAIClass; break;
        }

        if (!SelectedClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("JobType %d: No class assigned"), (int32)Job);
            continue;
        }

        // 2. 스폰
        FActorSpawnParameters SpawnParams;
        APawn* SpawnedPawn = World->SpawnActor<APawn>(SelectedClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

        if (!SpawnedPawn) continue;

        // 3. JobType, TeamID 설정
        AAICharacter* AIChar = Cast<AAICharacter>(SpawnedPawn);
        if (AIChar)
        {
            AIChar->JobType = Job;
            AIChar->TeamID = (i < 3) ? 1 : 0;
            if (AIChar->TeamID == 0) {
                AIChar->NickName = GetJobTypeString(Job) + "_Bot";
            }
            else if (AIChar->TeamID == 1) {
                AIChar->NickName = GetJobTypeString(Job) + "_EnemyBot";
            }
            Players.Add(AIChar);

            AIChar->TargetTags.Empty(); // 기존 태그 초기화
            if (AIChar->TeamID == 0)  // 아군 팀
            {
                AIChar->TargetTags.Add("0_root1");
                AIChar->TargetTags.Add("0_root2");
                AIChar->TargetTags.Add("0_root3");
            }
            else if (AIChar->TeamID == 1)  // 적 팀
            {
                AIChar->TargetTags.Add("1_root1");
                AIChar->TargetTags.Add("1_root2");
                AIChar->TargetTags.Add("1_root3");
            }
        }

        // 4. AIController 자동 설정 (이미 ControllerClass 지정 + AutoPossessAI 설정돼 있다면 생략 가능)
        AAIController* AICon = Cast<AAIController>(SpawnedPawn->GetController());
        if (!AICon)
        {
            AICon = World->SpawnActor<AAIController>(AIControllerClass, SpawnLocation, FRotator::ZeroRotator);
            if (AICon)
            {
                AICon->Possess(SpawnedPawn);
            }
        }

        // 5. 로그 확인
        UE_LOG(LogTemp, Warning, TEXT("Spawned: %s, JobType: %d"), *SpawnedPawn->GetName(), (int32)Job);
    }
}

FString AIfProjectGameMode::GetJobTypeString(EJobType JobType) {
    const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EJobType"), true);
    if (!EnumPtr) return FString("Invalid");

    return EnumPtr->GetNameStringByValue((int64)JobType);
}

TArray<FVector> AIfProjectGameMode::GetAStartPosition()
{
    TArray<AActor*>FoundActors;
    TArray<FVector>ActorsLoc;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("0_Spawn")), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        ActorsLoc.Add(Actor->GetActorLocation());
    }
    return ActorsLoc;
}

TArray<FVector> AIfProjectGameMode::GetBStartPosition()
{
    TArray<AActor*>FoundActors;
    TArray<FVector>ActorsLoc;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("1_Spawn")), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        ActorsLoc.Add(Actor->GetActorLocation());
    }
    return ActorsLoc;
}

void AIfProjectGameMode::SetSpawnPosition()
{
    TArray<FVector> A_Spawn = GetAStartPosition();
    TArray<FVector> B_Spawn = GetBStartPosition();

    int32 A_num = 0;
    int32 B_num = 0;

    for (APawn* Pawn : Players)
    {
        if (!Pawn) continue;

        ABaseCharacter* BaseChar = Cast<ABaseCharacter>(Pawn);
        if (BaseChar)
        {
            int32 Team = BaseChar->TeamID;
            if (Team == 0)
            {
                Pawn->SetActorLocation(A_Spawn[A_num]);
                BaseChar->SpawnPosition = A_Spawn[A_num];
                A_num++;
            }
            else if (Team == 1)
            {
                Pawn->SetActorLocation(B_Spawn[B_num]);
                BaseChar->SpawnPosition = B_Spawn[B_num];
                B_num++;
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("IsNone"));
            }
        }
    }
}

void AIfProjectGameMode::UpdateCapturePoint(float DeltaTime)
{
    if (bIsContested)
    {
        return;
    }

    if (TeamInControl != -1)
    {
        CaptureProgress += DeltaTime;
        if (CaptureProgress >= CaptureTime)
        {
            UE_LOG(LogTemp, Warning, TEXT("Team %d Wins!"), TeamInControl);
        }
    }
}

