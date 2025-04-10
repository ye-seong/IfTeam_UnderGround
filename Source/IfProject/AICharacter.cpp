// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacter.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "JobType.h"
#include "TankerCharacter.h"
#include "DealerCharacter.h"
#include "HealerCharacter.h"
#include "Components/CapsuleComponent.h"  // UCapsuleComponent 포함
#include "NavigationSystem.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AAICharacter::AAICharacter()
{
    // 기본 설정
    PrimaryActorTick.bCanEverTick = true;
    AIControllerClass = AAIController::StaticClass();
    CurrentState = EAIState::Searching;
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);  // 적의 감지 채널에만 겹침
}

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();
    AController* CurrentController = GetController();
    UE_LOG(LogTemp, Display, TEXT("Current controller: %s"),
        CurrentController ? *CurrentController->GetName() : TEXT("None"));

    AAIController* AICon = Cast<AAIController>(CurrentController);
    UE_LOG(LogTemp, Display, TEXT("Is AIController?: %s"),
        AICon ? TEXT("Yes") : TEXT("No"));

    if (!IsPlayerControlled() && !CurrentController)
    {
        AAIController* NewController = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
        if (NewController)
        {
            NewController->Possess(this);
            UE_LOG(LogTemp, Display, TEXT("New AIController created and possessing"));
        }
    }
    //if (!IsPlayerControlled())
    //{
    //    AAIController* AICon = GetWorld()->SpawnActor<AAIController>(AIControllerClass, GetActorLocation(), GetActorRotation());
    //    if (AICon)
    //    {
    //        AICon->Possess(this);  // AIController가 AI 캐릭터를 제어하도록 설정
    //    }
    //}

    if (TeamID == 0)  // A팀
    {
        TargetTags.Add("0_root1");
        TargetTags.Add("0_root2");
        TargetTags.Add("0_root3");
    }
    else if (TeamID == 1)  // B팀
    {
        TargetTags.Add("1_root1");
        TargetTags.Add("1_root2");
        TargetTags.Add("1_root3");
    }
    //CurrentTargetActor = nullptr;

    CurrentTagIndex = 0;

}

void AAICharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    switch (CurrentState)
    {
    case EAIState::Idle:
        StopMovement();
        break;

    case EAIState::Searching:
        MoveToTaggedActor();

        if (DetectEnemies())
        {
            CurrentState = EAIState::Attacking;
        }
        break;
    //case EAIState::NeedHeal:
    //    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NeedHeal!!!!!!!!!!!!!!"));
    //    //FindHealPack();
    //    if (CurrentHP > MaxHP / 2)
    //    {
    //        CurrentState = EAIState::Attacking;
    //    }
    //    break;
    case EAIState::Attacking:
        if (!TargetEnemy || TargetEnemy->bIsDead)
        {
            TargetEnemy = nullptr;
            CurrentState = EAIState::Searching;
            break;
        }

        if (!DetectEnemies())
        {
            CurrentState = EAIState::Searching;
        }
        else if (CurrentHP <= MaxHP / 2)
        {
            CurrentState = EAIState::NeedHeal;
        }
        else
        {
            switch (JobType)
            {

            case EJobType::Tanker:
                AIBarrierSkillTick();          // 방벽 생성
                AIShockStrikeSkillTick();      // 충격 강타
                AITankerUltimateSkillTick();   // 궁극기
                break;

            case EJobType::Dealer:
                AIDashSkillTick();             // 대쉬
                AIStunSkillTick();             // 스턴
                AIDealerUltimateSkillTick();   // 궁극기
                break;

            case EJobType::Healer:
                AITeleportHealSkillTick();     // 텔레포트 힐
                AIShieldHealSkillTick();       // 쉴드
                AIHealerUltimateSkillTick();   // 궁극기
                break;

            default:
                break;
            }
            StartFighting(TargetEnemy);             // 총 발사
        }
        break;
    }
    //AvoidObstacleOrJump();  // 장애물 회피 및 점프 로직 처리
}



void AAICharacter::StopMovement()
{
    AAIController* AICon = Cast<AAIController>(GetController());
    if (AICon)
    {
        AICon->StopMovement();
    }
}

void AAICharacter::MoveToTarget(FVector Location)
{
    // AIController로 경로 이동 처리
    AAIController* AICon = Cast<AAIController>(GetController());
    if (AICon && GetController()->IsFollowingAPath() == false)
    {
        AICon->MoveToLocation(Location, -1, true, true, true, false, 0, true);
    }
}

bool AAICharacter::DetectEnemies()
{
    FVector MyLocation = GetActorLocation();
    float DetectionRadius = 1000.0f;

    TArray<FHitResult> HitResults;
    FCollisionShape CollisionShape = FCollisionShape::MakeSphere(DetectionRadius);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);  // 자기 자신은 제외

    bool bHit = GetWorld()->SweepMultiByChannel(HitResults, MyLocation, MyLocation, FQuat::Identity, ECC_Pawn, CollisionShape, QueryParams);

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            ABaseCharacter* OtherCharacter = Cast<ABaseCharacter>(Hit.GetActor());
            if (OtherCharacter && OtherCharacter->TeamID != this->TeamID && !OtherCharacter->bIsDead)   // 적군일 경우
            {
                if (!TargetEnemy)
                {
                    TargetEnemy = OtherCharacter;
                    return true;
                }
                return true;
            }
        }
    }
    return false;
}

void AAICharacter::StartFighting(ABaseCharacter* Enemy)
{
    if (!Enemy || Enemy->bIsDead) return;  // << 이거 꼭 추가!
    if (GetWorld()->GetTimeSeconds() - LastFireTime >= FireRate)
    {
        if (Gun)
        {
            Gun->AIFire(Enemy);
            LastFireTime = GetWorld()->GetTimeSeconds();
        }
    }

    //FString LogMessage = FString::Printf(TEXT("Enemy TeamID: %d, Job: %d and My TeamID: %d, Job: %d Fighting!!"),
    //    Enemy->TeamID, (int32)Enemy->JobType, this->TeamID, (int32)this->JobType);

 /*   UE_LOG(LogTemp, Warning, TEXT("%s"), *LogMessage);*/
}

void AAICharacter::MoveToTaggedActor()
{
    if (CurrentTagIndex >= TargetTags.Num())
    {
        UE_LOG(LogTemp, Display, TEXT("%s: all tag good"), *GetName());
        
        //if (FVector::Dist(GetActorLocation(), TargetLoc) < 200.f)
        //{
        //    // 목표 지점에 도달하면 새로운 랜덤 목표 설정
        //    MoveRandomlyInRange();
        //}
        return;
    }
    /*FString DebugMsg1 = FString::Printf(TEXT("CurrentTagIndex : %d"), CurrentTagIndex);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, DebugMsg1);*/

    FName CurrentTag = TargetTags[CurrentTagIndex];
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), CurrentTag, FoundActors);

    if (FoundActors.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: tag '%s' don't exist actors"), *GetName(), *CurrentTag.ToString());
        return;
    }

    AActor* ClosestActor = nullptr;
    FVector MyLocation = GetActorLocation();

    for (AActor* Actor : FoundActors)
    {
        AAIController* MyController = Cast<AAIController>(GetController());

        if (MyController)
        {
            CurrentTargetActor = Actor;
            float DistanceToTarget = FVector::Dist(GetActorLocation(), CurrentTargetActor->GetActorLocation());
            if (DistanceToTarget < 200.f)
            {
                CurrentTagIndex++;
                CurrentTargetActor = nullptr;
            }
            else {
                MyController->MoveToLocation(Actor->GetActorLocation());
            }
        }

        //CurrentTargetActor = ClosestActor;
    }

}

void AAICharacter::FindHealPack()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("HealPack"), FoundActors);

    if (FoundActors.Num() == 0)
        return;

    AActor* ClosestPack = nullptr;
    float ClosestDistance = 0.f;

    for (AActor* Actor : FoundActors)
    {
        if (!IsValid(Actor)) continue;

        float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestPack = Actor;
        }
    }
    AAIController* MyController = Cast<AAIController>(GetController());

    if (MyController)
    {
        MyController->MoveToLocation(ClosestPack->GetActorLocation());
    }
}


void AAICharacter::FindUltimateItem()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("UltimateItem"), FoundActors);

    if (FoundActors.Num() == 0)
        return;

    AActor* ClosestItem = nullptr;
    float ClosestDistance = 0.f;

    for (AActor* Actor : FoundActors)
    {
        if (!IsValid(Actor)) continue;

        float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestItem = Actor;
        }
    }
    AAIController* MyController = Cast<AAIController>(GetController());

    if (MyController)
    {
        MyController->MoveToLocation(ClosestItem->GetActorLocation());
    }
}

void AAICharacter::MoveRandomlyInRange()
{
    // X, Y 축의 랜덤 범위
    float RandomX = FMath::RandRange(4300.0f, 7300.0f);
    float RandomY = FMath::RandRange(4000.0f, 6000.0f);

    // Z축은 현재 위치를 유지하거나 원하는 값으로 설정 (예: 바닥 위치)
    float RandomZ = GetActorLocation().Z;

    // 랜덤 위치 생성
    FVector RandomLocation(RandomX, RandomY, RandomZ);

    //TargetLoc = RandomLocation;

    // AIController를 얻어 이동
    AAIController* AICon = Cast<AAIController>(GetController());
    if (AICon)
    {
        AICon->MoveToLocation(RandomLocation);
    }

    
}













//void AAICharacter::AvoidObstacleOrJump()
//{
//    FVector CurrentLocation = GetActorLocation();
//    FVector ForwardVector = GetActorForwardVector();  // AI의 전방 벡터
//
//    // 목표 지점으로 향하는 방향
//    FVector TargetLocation = GetTargetLocation();
//    FVector DirectionToTarget = (TargetLocation - CurrentLocation).GetSafeNormal();
//
//    // AI가 이동할 경로에 장애물이 있을 경우
//    FHitResult HitResult;
//    FVector Start = CurrentLocation;
//    FVector End = CurrentLocation + ForwardVector * 200.f;  // 200 유닛 전방으로 레이캐스트
//
//    // 레이캐스트로 장애물 감지
//    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
//    {
//        // 장애물이 발견되었을 때, 벽의 높이를 측정
//        FVector HitLocation = HitResult.ImpactPoint;
//        float WallHeight = HitLocation.Z - CurrentLocation.Z;
//
//        // 벽이 점프 가능한 높이인지 확인
//        if (WallHeight > 200.f)  // 문턱이 너무 높으면 점프
//        {
//            Jump();  // 점프 동작 실행
//        }
//        else
//        {
//            // 벽을 피하기 위해 몸을 옆으로 돌려서 회피
//            FVector AvoidanceDirection = (CurrentLocation - HitLocation).GetSafeNormal();  // 벽 반대 방향으로 벡터 계산
//
//            // 회피 후 AI의 회전 (부드럽게 회전)
//            FRotator TargetRotation = FRotationMatrix::MakeFromX(AvoidanceDirection).Rotator();  // 회피 방향으로 회전
//            FRotator CurrentRotation = GetActorRotation();
//            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f);  // 회전 부드럽게
//            SetActorRotation(NewRotation);  // AI 회전 설정
//
//            // 회피 후 이동 (이동 범위 제한)
//            FVector NewLocation = CurrentLocation + AvoidanceDirection * 100.f;  // 100 유닛 만큼 회피
//            SetActorLocation(NewLocation);  // 옆으로 회피
//
//            // 회피 후 목표 방향으로 재설정하여 다시 목표를 향해 이동 (부드럽게)
//            FVector DirectionToMove = (TargetLocation - GetActorLocation()).GetSafeNormal();
//            SetActorRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());  // 목표 방향으로 회전
//            FVector NextLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, GetWorld()->GetDeltaSeconds(), 5.0f);  // 부드럽게 이동
//            SetActorLocation(NextLocation);  // 목표로 향해서 이동
//        }
//    }
//    else
//    {
//        // 장애물이 없으면 목표 위치로 이동
//        SetActorLocation(CurrentLocation + DirectionToTarget * 200.f);  // 목표로 향해서 이동
//    }
//}
//
//bool AAICharacter::ShouldJumpToReachTarget(FVector TargetLocation)
//{
//    FVector Start = GetActorLocation();
//    FVector End = TargetLocation;
//
//    // 레이캐스트로 목표 지점까지 장애물 감지
//    FHitResult HitResult;
//    FCollisionQueryParams CollisionParams;
//    CollisionParams.AddIgnoredActor(this); // AI 캐릭터 자신을 무시
//
//    // 목표 지점까지 레이캐스트
//    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
//
//    // 목표 지점까지 장애물이 있다면 점프할 필요가 있을 수 있음
//    if (bHit)
//    {
//        // 장애물의 높이와 목표 위치까지의 거리 계산
//        FVector HitLocation = HitResult.ImpactPoint;
//        float WallHeight = HitLocation.Z - Start.Z;
//
//        // 목표까지 장애물의 높이가 점프할 수 있는 범위에 있다면 점프
//        if (WallHeight <= 200.f)  // 200 유닛 이하로 점프 가능
//        {
//            return true;  // 점프가 필요
//        }
//    }
//
//    return false;  // 점프가 필요하지 않음
//}
//
//void AAICharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
//    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//    if (OtherActor && OtherActor != this)
//    {
//        // 벽에 부딪혔을 때 회피 동작
//        FVector AvoidanceDirection = (GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();
//        FVector NewLocation = GetActorLocation() + AvoidanceDirection * 100.f;  // 벽을 피하는 방향으로 이동
//
//        // 벽 높이 체크 후 점프 여부 결정
//        if (ShouldJump(OtherActor))
//        {
//            Jump();  // 점프 동작
//        }
//        else
//        {
//            // 회피 동작
//            SetActorLocation(NewLocation);
//        }
//    }
//}
//
//bool AAICharacter::ShouldJump(AActor* OtherActor)
//{
//    // 벽 높이 계산 (충돌 지점에서 AI 캐릭터까지의 높이 차이)
//    FVector WallLocation = OtherActor->GetActorLocation();
//    float WallHeight = WallLocation.Z - GetActorLocation().Z;
//
//    return WallHeight <= 200.f;  // 벽 높이가 200 이하일 때 점프
//}