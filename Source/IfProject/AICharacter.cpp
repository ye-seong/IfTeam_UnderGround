// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacter.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "JobType.h"
#include "TankerCharacter.h"
#include "DealerCharacter.h"
#include "HealerCharacter.h"
#include "Components/CapsuleComponent.h"  // UCapsuleComponent ����
#include "NavigationSystem.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AAICharacter::AAICharacter()
{
    // �⺻ ����
    PrimaryActorTick.bCanEverTick = true;
    AIControllerClass = AAIController::StaticClass();
    CurrentState = EAIState::Searching;
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);  // ���� ���� ä�ο��� ��ħ
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
    //        AICon->Possess(this);  // AIController�� AI ĳ���͸� �����ϵ��� ����
    //    }
    //}

    if (TeamID == 0)  // A��
    {
        TargetTags.Add("0_root1");
        TargetTags.Add("0_root2");
        TargetTags.Add("0_root3");
    }
    else if (TeamID == 1)  // B��
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
                AIBarrierSkillTick();          // �溮 ����
                AIShockStrikeSkillTick();      // ��� ��Ÿ
                AITankerUltimateSkillTick();   // �ñر�
                break;

            case EJobType::Dealer:
                AIDashSkillTick();             // �뽬
                AIStunSkillTick();             // ����
                AIDealerUltimateSkillTick();   // �ñر�
                break;

            case EJobType::Healer:
                AITeleportHealSkillTick();     // �ڷ���Ʈ ��
                AIShieldHealSkillTick();       // ����
                AIHealerUltimateSkillTick();   // �ñر�
                break;

            default:
                break;
            }
            StartFighting(TargetEnemy);             // �� �߻�
        }
        break;
    }
    //AvoidObstacleOrJump();  // ��ֹ� ȸ�� �� ���� ���� ó��
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
    // AIController�� ��� �̵� ó��
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
    QueryParams.AddIgnoredActor(this);  // �ڱ� �ڽ��� ����

    bool bHit = GetWorld()->SweepMultiByChannel(HitResults, MyLocation, MyLocation, FQuat::Identity, ECC_Pawn, CollisionShape, QueryParams);

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            ABaseCharacter* OtherCharacter = Cast<ABaseCharacter>(Hit.GetActor());
            if (OtherCharacter && OtherCharacter->TeamID != this->TeamID && !OtherCharacter->bIsDead)   // ������ ���
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
    if (!Enemy || Enemy->bIsDead) return;  // << �̰� �� �߰�!
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
        //    // ��ǥ ������ �����ϸ� ���ο� ���� ��ǥ ����
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
    // X, Y ���� ���� ����
    float RandomX = FMath::RandRange(4300.0f, 7300.0f);
    float RandomY = FMath::RandRange(4000.0f, 6000.0f);

    // Z���� ���� ��ġ�� �����ϰų� ���ϴ� ������ ���� (��: �ٴ� ��ġ)
    float RandomZ = GetActorLocation().Z;

    // ���� ��ġ ����
    FVector RandomLocation(RandomX, RandomY, RandomZ);

    //TargetLoc = RandomLocation;

    // AIController�� ��� �̵�
    AAIController* AICon = Cast<AAIController>(GetController());
    if (AICon)
    {
        AICon->MoveToLocation(RandomLocation);
    }

    
}













//void AAICharacter::AvoidObstacleOrJump()
//{
//    FVector CurrentLocation = GetActorLocation();
//    FVector ForwardVector = GetActorForwardVector();  // AI�� ���� ����
//
//    // ��ǥ �������� ���ϴ� ����
//    FVector TargetLocation = GetTargetLocation();
//    FVector DirectionToTarget = (TargetLocation - CurrentLocation).GetSafeNormal();
//
//    // AI�� �̵��� ��ο� ��ֹ��� ���� ���
//    FHitResult HitResult;
//    FVector Start = CurrentLocation;
//    FVector End = CurrentLocation + ForwardVector * 200.f;  // 200 ���� �������� ����ĳ��Ʈ
//
//    // ����ĳ��Ʈ�� ��ֹ� ����
//    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
//    {
//        // ��ֹ��� �߰ߵǾ��� ��, ���� ���̸� ����
//        FVector HitLocation = HitResult.ImpactPoint;
//        float WallHeight = HitLocation.Z - CurrentLocation.Z;
//
//        // ���� ���� ������ �������� Ȯ��
//        if (WallHeight > 200.f)  // ������ �ʹ� ������ ����
//        {
//            Jump();  // ���� ���� ����
//        }
//        else
//        {
//            // ���� ���ϱ� ���� ���� ������ ������ ȸ��
//            FVector AvoidanceDirection = (CurrentLocation - HitLocation).GetSafeNormal();  // �� �ݴ� �������� ���� ���
//
//            // ȸ�� �� AI�� ȸ�� (�ε巴�� ȸ��)
//            FRotator TargetRotation = FRotationMatrix::MakeFromX(AvoidanceDirection).Rotator();  // ȸ�� �������� ȸ��
//            FRotator CurrentRotation = GetActorRotation();
//            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f);  // ȸ�� �ε巴��
//            SetActorRotation(NewRotation);  // AI ȸ�� ����
//
//            // ȸ�� �� �̵� (�̵� ���� ����)
//            FVector NewLocation = CurrentLocation + AvoidanceDirection * 100.f;  // 100 ���� ��ŭ ȸ��
//            SetActorLocation(NewLocation);  // ������ ȸ��
//
//            // ȸ�� �� ��ǥ �������� �缳���Ͽ� �ٽ� ��ǥ�� ���� �̵� (�ε巴��)
//            FVector DirectionToMove = (TargetLocation - GetActorLocation()).GetSafeNormal();
//            SetActorRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());  // ��ǥ �������� ȸ��
//            FVector NextLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, GetWorld()->GetDeltaSeconds(), 5.0f);  // �ε巴�� �̵�
//            SetActorLocation(NextLocation);  // ��ǥ�� ���ؼ� �̵�
//        }
//    }
//    else
//    {
//        // ��ֹ��� ������ ��ǥ ��ġ�� �̵�
//        SetActorLocation(CurrentLocation + DirectionToTarget * 200.f);  // ��ǥ�� ���ؼ� �̵�
//    }
//}
//
//bool AAICharacter::ShouldJumpToReachTarget(FVector TargetLocation)
//{
//    FVector Start = GetActorLocation();
//    FVector End = TargetLocation;
//
//    // ����ĳ��Ʈ�� ��ǥ �������� ��ֹ� ����
//    FHitResult HitResult;
//    FCollisionQueryParams CollisionParams;
//    CollisionParams.AddIgnoredActor(this); // AI ĳ���� �ڽ��� ����
//
//    // ��ǥ �������� ����ĳ��Ʈ
//    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
//
//    // ��ǥ �������� ��ֹ��� �ִٸ� ������ �ʿ䰡 ���� �� ����
//    if (bHit)
//    {
//        // ��ֹ��� ���̿� ��ǥ ��ġ������ �Ÿ� ���
//        FVector HitLocation = HitResult.ImpactPoint;
//        float WallHeight = HitLocation.Z - Start.Z;
//
//        // ��ǥ���� ��ֹ��� ���̰� ������ �� �ִ� ������ �ִٸ� ����
//        if (WallHeight <= 200.f)  // 200 ���� ���Ϸ� ���� ����
//        {
//            return true;  // ������ �ʿ�
//        }
//    }
//
//    return false;  // ������ �ʿ����� ����
//}
//
//void AAICharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
//    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//    if (OtherActor && OtherActor != this)
//    {
//        // ���� �ε����� �� ȸ�� ����
//        FVector AvoidanceDirection = (GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();
//        FVector NewLocation = GetActorLocation() + AvoidanceDirection * 100.f;  // ���� ���ϴ� �������� �̵�
//
//        // �� ���� üũ �� ���� ���� ����
//        if (ShouldJump(OtherActor))
//        {
//            Jump();  // ���� ����
//        }
//        else
//        {
//            // ȸ�� ����
//            SetActorLocation(NewLocation);
//        }
//    }
//}
//
//bool AAICharacter::ShouldJump(AActor* OtherActor)
//{
//    // �� ���� ��� (�浹 �������� AI ĳ���ͱ����� ���� ����)
//    FVector WallLocation = OtherActor->GetActorLocation();
//    float WallHeight = WallLocation.Z - GetActorLocation().Z;
//
//    return WallHeight <= 200.f;  // �� ���̰� 200 ������ �� ����
//}