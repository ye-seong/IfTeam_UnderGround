#include "BaseCharacter.h"
#include "EngineUtils.h"
#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BarrierActor.h"
#include "BubbleVisualShieldActor.h"
#include "DrawDebugHelpers.h"
#include "MissileProjectile.h"
#include "TimerManager.h"
#include "IfProjectGameState.h"
#include "IfProjectPlayerController.h"
#include "AIController.h"
#include "AICharacter.h"



ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentHP = MaxHP;
    bReplicates = true;
    bAlwaysRelevant = true;
    GetCharacterMovement()->SetIsReplicated(true);
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = 1200.f;

    if (TeamID == -1)
    {
        if (IsPlayerControlled())
            TeamID = 0;
        else
            TeamID = 1; // 예시값
    }

    // 게임 시작 위치 저장
    ReviveLocation = GetActorLocation();

    //CurrentHP = MaxHP;
    FireRate = 0.5f;
    LastFireTime = -FireRate;

    if (GunClass)
    {
        Gun = GetWorld()->SpawnActor<AGun>(GunClass);
        if (Gun)
        {
            Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GripPoint"));
            Gun->SetOwner(this);
        }
    }
}

void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseCharacter::ReceiveDamage(float DamageAmount)
{
    if (bIsDead) return;
    
    //if (bUnderTankerUltimate)
    //{
    //    DamageAmount *= 0.5f;  // 50% ????
    //}
    if (CurrentShieldVisual && IsValid(CurrentShieldVisual) || bUnderTankerUltimate)
    {
        DamageAmount *= 0.5f;  // 50% 감소
        bUnderTankerUltimate = false;
    }
    CurrentHP -= DamageAmount;

    //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("%s CurrentHP :%d"), *GetName(), CurrentHP));

    if (CurrentHP <= 0.f)
    {
        LastHitByChar = this;
        if (LastHitByChar)
        {
            AIfProjectPlayerController* PC = Cast<AIfProjectPlayerController>(LastHitByChar->GetController());
            if (PC)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("!!!!!!!!!!!!!!!!!!!!!!!Get pc!!!!!!!!!!!!!!!!!!!!!!"));
            }
        }
        CurrentHP = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());

        
        HandleDeath();
    }
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f; // ? 사망 시 무시
    FString CauserName = DamageCauser ? DamageCauser->GetName() : TEXT("Unknown");

    ReceiveDamage(DamageAmount);  // 내가 만든 커스텀 함수 호출
    return DamageAmount;
}

void ABaseCharacter::OnFire()
{
    if (bIsDead || !Gun) return;
    FString Msg = FString::Printf(TEXT("bIsReloading: %s"), Gun->bIsReloading ? TEXT("true") : TEXT("false"));
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Msg);
    // 여기서 조건만 체크: 탄약 있고, 리로딩 아니면
    if (Gun->CurrBulletNum > 0 && !Gun->bIsReloading)
    {
        Super::OnFire();   // 실제 Projectile 생성
        Gun->Fire();       // 탄약 차감, 리로딩 체크는 내부에서
    }
    else if (Gun->CurrBulletNum <= 0 && !Gun->bIsReloading)
    {
        // 탄약 없음 + 아직 리로딩 안 걸려 있으면 리로딩 시작
        Gun->Fire();  // 내부에서 bIsReloading = true + 타이머 설정
    }
    // 그 외 (이미 리로딩 중)는 아무것도 안 함
}

void ABaseCharacter::Heal(float HealAmount)
{
    if (bIsDead) return;

    CurrentHP += HealAmount;
    if (CurrentHP > MaxHP)
        CurrentHP = MaxHP;

    GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT("Healed: +%.0f / HP: %.0f"), HealAmount, CurrentHP));
}

void ABaseCharacter::Revive()
{
    if (!bIsDead) return;


    bIsDead = false;

    CurrentHP = MaxHP;

    // 위치 복원
    SetActorLocation(SpawnPosition); // 필요 시 별도 저장

    // 다시 보이게
    SetActorHiddenInGame(false);

    // 충돌/입력/이동 복원
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // 메시
    if (GetMesh()) GetMesh()->SetVisibility(true, true);
    if (Gun) {
        Gun->SetActorHiddenInGame(false);
        Gun->SetActorEnableCollision(true);
        Gun->SetActorTickEnabled(true);
    }

    if (AAICharacter* AIChar = Cast<AAICharacter>(this))
    {
        AIChar->CurrentTagIndex = 0;  // 부활 시 인덱스 초기화
    }

    if (!IsPlayerControlled())
    {
        AController* MyController = GetController();
        if (!MyController)
        {
            SpawnDefaultController();
        }
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s has been revived."), *GetName()));
}


void ABaseCharacter::HandleDeath()
{
    if (bIsDead) return;

    bIsDead = true;  // ? 여기서만 true로 설정

    // 모든 타이머 제거
    GetWorldTimerManager().ClearAllTimersForObject(this);

    GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red,
        FString::Printf(TEXT("[DEATH] %s has died."), *GetName()));

    ReviveLocation = GetActorLocation();

    // 입력 막기 (플레이어)
    DisableInput(nullptr);

    // 이동 막기
    GetCharacterMovement()->DisableMovement();

    // 충돌 막기
    SetActorEnableCollision(false);

    // 액터 전체 숨기기
    SetActorHiddenInGame(true);

    // 메시 숨기기
    if (GetMesh())
    {
        GetMesh()->SetVisibility(false, true);
    }

    // 총기 비활성화
    if (Gun)
    {
        Gun->SetActorHiddenInGame(true);
        Gun->SetActorEnableCollision(false);
        Gun->SetActorTickEnabled(false);
    }

    // Tick 멈춤
    SetActorTickEnabled(false);

    // AI 언포제스
    if (!IsPlayerControlled())
    {
        AController* AIController = GetController();
        if (AIController)
        {
            AIController->StopMovement();
            AIController->UnPossess();
        }
    }
    else {
        // 플레이어 입력 다시 연결
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            EnableInput(PC);
        }
    }
    if (CurrentShieldVisual && IsValid(CurrentShieldVisual))
    {
        CurrentShieldVisual->Destroy();
        CurrentShieldVisual = nullptr;
    }
    

    // ?? 10초 뒤 자동 부활 타이머 시작
    GetWorldTimerManager().SetTimer(ReviveTimerHandle, this, &ABaseCharacter::Revive, 10.f, false);
}

void ABaseCharacter::ReceiveHealing(float HealAmount)
{
    CurrentHP += HealAmount;
    if (CurrentHP > MaxHP)
    {
        CurrentHP = MaxHP;
    }

    //GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,
    //    FString::Printf(TEXT("%s healed: %.1f / %.1f"), *GetName(), CurrentHP, MaxHP));
}

void ABaseCharacter::AddUltiNum(int32 Amount)
{
    // 궁극기 게이지를 누적하되, 0 ~ MaxUltimateCount 범위를 넘지 않도록 Clamp
    this->UltiNum = FMath::Clamp(this->UltiNum + Amount, 0, this->MaxUltiNum);
}

/* 공통 함수 */

// 탱커
void ABaseCharacter::PerformBarrierSkill(float& CurCooldown, float CoolTime, float Distance, TSubclassOf<ABarrierActor> BarrierClassInput)
{
    if (bIsDead) return;
    // 이미 쿨타임 중이거나 클래스 지정이 안 되어 있으면 리턴
    if (CurCooldown > 0.f || !BarrierClassInput)
    {
        /* GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Barrier Skill is on cooldown!"));*/
        return;
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
        FString::Printf(TEXT("[Tanker Skill1] %s"), *GetName()));

    /*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TankerCharacter's BarrierSkill FUNCTION start~"));*/
    // 캐릭터 앞에 방벽 생성할 위치 계산
    // 캐릭터 전방으로 BarrierDistance만큼 떨어진 위치에 방벽 생성
    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * Distance + FVector(0, 0, 50.f); // Z축으로 위로 50cm;
    FRotator SpawnRotation = GetActorRotation();

    // 방벽 생성
    FActorSpawnParameters SpawnParams;
    ABarrierActor* Barrier = GetWorld()->SpawnActor<ABarrierActor>(BarrierClassInput, SpawnLocation, SpawnRotation, SpawnParams);
    if (Barrier)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Barrier is true~"));
        // 방벽에게 팀 ID 전달 (캐릭터의 PlayerState에서 가져옴)
        Barrier->SetTeamID(TeamID);
    }

    // 사용 불가로 설정하고 쿨타임 타이머 시작
    CurCooldown = CoolTime;
}

void ABaseCharacter::PerformShockStrikeSkill(float &CurrCooldown, float CoolTime, float ImpactRadius, float ImpactDamage)
{
    if (bIsDead) return;
    // 쿨타임
    if (CurrCooldown > 0.f) return;

    CurrCooldown = CoolTime;

    UE_LOG(LogTemp, Warning, TEXT("Is Shock Strike!!"));

    // 캐릭터 현재 위치를 기준으로 충격파 중심 설정
    FVector Origin = GetActorLocation();

    //// 충격파 탐지 결과 저장용 배열
    TArray<FHitResult> HitResults;

    // 충격파 범위(구체 형태)
    FCollisionShape Sphere = FCollisionShape::MakeSphere(ImpactRadius);

    // 반경 내에서 Pawn 채널로 충돌 검사 (즉, 캐릭터들만 탐지)
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Origin,
        Origin + FVector(0.1f),       // 정지 상태에서 Sweep
        FQuat::Identity,              // 회전 없음
        ECC_Pawn,                     // Pawn 채널만 탐색 (캐릭터들)
        Sphere
    );

    TSet<AActor*> ProcessedActors;

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            ABaseCharacter* Target = Cast<ABaseCharacter>(Hit.GetActor());

            if (Target
                && Target != this
                && !Target->bIsDead
                && Target->TeamID != this->TeamID
                && !ProcessedActors.Contains(Target))
            {
                Target->ReceiveDamage(ImpactDamage);
                ProcessedActors.Add(Target);

                /* GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
                     FString::Printf(TEXT("[%s] Shock skill use! Target: %s / Damage: %.1f"),
                         *GetName(), *Target->GetName(), ImpactDamage));*/
            }
        }
    }

    DrawDebugSphere(GetWorld(), Origin, ImpactRadius, 16, FColor::Red, false, 2.0f);
}


void ABaseCharacter::PerformTankerUltimateSkill(float Duration)
{
    if (bIsDead) return;
    if (UltiNum < 3)
    {
        /*GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
            FString::Printf(TEXT("current ultiNum : %d"), UltiNum));*/
        return;
    }

    if (UltiNum == 3) {

        // 1. 기존 쉴드 제거 (팀ID 확인해서 자신 팀의 것만 제거)
        for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
        {
            if (It->TeamID == this->TeamID && !It->bIsDead)
            {
                // ĳ????? ?????? ??? ???? ????
                if (It->CurrentShieldVisual && IsValid(It->CurrentShieldVisual))
                {
                    It->CurrentShieldVisual->Destroy();
                    It->CurrentShieldVisual = nullptr;
                }
            }
        
        }

        // 2. 쉴드 생성 및 저장
        for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
        {
            if (It->TeamID == this->TeamID)
            {
                It->SetBUnderTankerUltimate(true);

                if (BubbleShieldVisualClass)
                {
                    FVector SpawnLocation = It->GetActorLocation() + FVector(0, 0, 100.f);
                    FRotator SpawnRotation = FRotator::ZeroRotator;

                    ABubbleVisualShieldActor* ShieldVisual = GetWorld()->SpawnActor<ABubbleVisualShieldActor>(
                        BubbleShieldVisualClass, SpawnLocation, SpawnRotation);

                    if (ShieldVisual)
                    {
                        ShieldVisual->AttachToActor(*It, FAttachmentTransformRules::KeepWorldTransform);
                        It->CurrentShieldVisual = ShieldVisual;  // 참조 저장
                        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
                            FString::Printf(TEXT("[TankerUltimateSkill] : %s"), *GetName()));
                    }
                }
            }
        }

        // 3. 일정 시간 후 버블막 제거 및 상태 복구
        FTimerHandle Handle;
        GetWorld()->GetTimerManager().SetTimer(Handle, [this]()
            {
                for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
                {
                    if (It->TeamID == this->TeamID)
                    {
                        It->SetBUnderTankerUltimate(false);

                        // 저장된 쉴드 제거
                        if (It->CurrentShieldVisual && IsValid(It->CurrentShieldVisual))
                        {
                            It->CurrentShieldVisual->Destroy();
                            It->CurrentShieldVisual = nullptr;
                        }
                    }
                }

            }, Duration, false);

        UltiNum = 0;
    }

}


// 딜러
void ABaseCharacter::PerformDashSkill(float& CurrCooldown, float CoolTime)
{
    // 쿨타임
    if (CurrCooldown > 0.f || !GetCharacterMovement()->IsMovingOnGround()) return;

    CurrCooldown = CoolTime;

    UE_LOG(LogTemp, Warning, TEXT("Dealer Dash Skill"));

    // 현재 캐릭터가 바라보는 방향
    FVector ForwardDirection = GetActorForwardVector();

    // 대시 힘 (길이 조절 가능)
    // 
    //float DashStrength = 8000.0f;

    // LaunchCharacter: 캐릭터를 특정 방향으로 튕겨나가게 함 (XY 방향만 쓰고 Z는 0)
    LaunchCharacter(ForwardDirection * 27000.f, true, false);

}

void ABaseCharacter::PerformStunSkill(float& CurrCooldown, float CoolTime)
{
    if (bIsDead || CurrCooldown > 0.f) return;
    CurrCooldown = CoolTime;

    float Radius = 1000.f;             // 탐지 반경
    float ConeAngle = 180.f;           // 부채꼴 각도 (도 단위)
    FVector MyLocation = GetActorLocation();
    FVector MyForward = GetActorForwardVector();

    DrawDebugCone(GetWorld(), MyLocation, MyForward, Radius,
        FMath::DegreesToRadians(ConeAngle * 0.5f),
        FMath::DegreesToRadians(ConeAngle * 0.5f),
        12, FColor::Yellow, false, 1.0f);

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 구형 범위 탐색
    bool bFound = GetWorld()->OverlapMultiByChannel(
        Overlaps,
        MyLocation,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

    bool bHitSomething = false;

    if (bFound)
    {
        for (auto& Result : Overlaps)
        {
            ABaseCharacter* Target = Cast<ABaseCharacter>(Result.GetActor());
            if (!Target || Target == this || Target->bIsDead) continue;

            FVector ToTarget = (Target->GetActorLocation() - MyLocation).GetSafeNormal();
            float Dot = FVector::DotProduct(MyForward, ToTarget);
            float CosThreshold = FMath::Cos(FMath::DegreesToRadians(ConeAngle * 0.5f));

            if (Dot >= CosThreshold)  // 부채꼴 안에 있음
            {
                Target->DisableCharacter(2.0f);
                bHitSomething = true;

                GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
                    FString::Printf(TEXT("[ConeStun Skill] Target: %s"), *Target->GetName()));
                //break;  // 한 명만 스턴할 거면 break, 여러 명이면 제거해도 됨
            }
        }
    }

    if (!bHitSomething)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
            TEXT("[ConeStun Skill] No Target in Cone"));
    }

}

void ABaseCharacter::DisableCharacter(float Duration)
{
    DisableInput(nullptr);  // 모든 입력 끊기

    GetWorldTimerManager().SetTimer(
        BuffTimerHandle,
        [this]()
        {
            EnableInput(nullptr);  // 2초 후 다시 입력 허용
        },
        Duration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Disable character control: %s"), *GetName());
}

void ABaseCharacter::PerformDealerUltimateSkill(FTimerHandle& UltimateDelayHandle)
{
    if (bIsDead) return;
    if (UltiNum < 3) return;

    if (UltiNum == 3) {

        // 1. 하늘로 점프
        LaunchCharacter(FVector(0, 0, 2000), true, true);

        // 2. 1초 후 자동 조준 → 미사일 발사
        GetWorld()->GetTimerManager().SetTimerForNextTick([this, &UltimateDelayHandle]()
            {
                GetWorld()->GetTimerManager().SetTimer(
                    UltimateDelayHandle,
                    [this]()
                    {
                        // 주변 적 탐색
                        FVector MyLocation = GetActorLocation();
                        float MinDist = FLT_MAX;
                        ABaseCharacter* ClosestTarget = nullptr;

                        for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
                        {
                            ABaseCharacter* Target = *It;
                            if (Target != this && !Target->IsPendingKill())
                            {
                                float Dist = FVector::Dist2D(Target->GetActorLocation(), MyLocation);  // Z 무시
                                if (Dist < 1500.f && Dist < MinDist)
                                {
                                    ClosestTarget = Target;
                                    MinDist = Dist;
                                }
                            }
                        }

                        if (ClosestTarget && Gun && Gun->GetProjectileClass())
                        {
                            FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);
                            FRotator SpawnRotation = (ClosestTarget->GetActorLocation() - SpawnLocation).Rotation();

                            FActorSpawnParameters Params;
                            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                            LaunchMissileAtTarget(ClosestTarget);

                        }
                        else
                        {

                            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
                                TEXT("[Dealer UltimateSkill] Target : No Target → Faild Missile Launch"));

                        }

                    },
                    1.0f, false  // 1초 후 실행
                );
            });

    }
    UltiNum = 0;
    }


void ABaseCharacter::LaunchMissileAtTarget(AActor* Target)
{
    if (bIsDead) return;
    if (!Gun || !Target) return;

    // 총알 클래스 가져오기
    TSubclassOf<AMissileProjectile> TempProjectileClass = Gun->GetProjectileClass();
    if (!TempProjectileClass) return;

    // 발사 위치 및 방향 설정
    FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);  // 머리 위 약간
    FRotator SpawnRotation = (Target->GetActorLocation() - SpawnLocation).Rotation();

    // 스폰 파라미터
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 총알(미사일) 생성
    AMissileProjectile* Projectile = GetWorld()->SpawnActor<AMissileProjectile>(
        DealerUltimateMissileClass, SpawnLocation, SpawnRotation, Params);

    // 유도 타겟 설정 + 데미지 설정
    if (Projectile)
    {
        Projectile->SetOwner(this);
        Projectile->SetDamage(70.0f);              // 궁극기 데미지
        Projectile->SetHomingTarget(Target);       // 유도 대상 설정

        UE_LOG(LogTemp, Warning, TEXT("Missile Launch Completed! : %s"), *Target->GetName());
    }
}

// 힐러
ABaseCharacter* ABaseCharacter::FindClosestAlly()
{
    ABaseCharacter* ClosestAlly = nullptr;
    float MinDistance = 1000.f;

    for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
    {
        ABaseCharacter* Other = *It;

        if (!IsValid(Other) || Other == this || Other->bIsDead) continue;  // ??? ???, ????? ???

        if (Other->TeamID != this->TeamID) continue;     // ?? ????? ????

        float Distance = FVector::Dist(this->GetActorLocation(), Other->GetActorLocation());

        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestAlly = Other;
        }
    }

    return ClosestAlly;
}

void ABaseCharacter::PerformTeleportHealSkill(float& CurrCooldown, float HealerCoolDown)
{
    if (bIsDead) return;
    if (CurrCooldown > 0.f) return;

    ABaseCharacter* FoundAlly = FindClosestAlly();

    if (FoundAlly)
    {
        TargetAlly = FoundAlly;
        TargetLocation = FoundAlly->GetActorLocation();
        bIsMovingToTarget = true;
        CurrCooldown = HealerCoolDown;
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("[HealSkill1] NO TEAM MEMBER"));
        CurrCooldown = 0;
    }
}

void ABaseCharacter::PerformTeleportHealSkillTick(ABaseCharacter* Character, float DeltaTime)
{

    if (!bIsMovingToTarget) return;

    Character->SetActorHiddenInGame(true);
    Character->SetActorEnableCollision(false);
    Character->SetActorTickEnabled(false); 

    Character->SetActorLocation(TargetLocation);

    Character->SetActorHiddenInGame(false);
    Character->SetActorEnableCollision(true);
    Character->SetActorTickEnabled(true);

    if (TargetAlly)
    {
        TargetAlly->ReceiveHealing(30.f);
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("INSTANT HEAL COMPLETE!"));
    }

    bIsMovingToTarget = false;
}

ABaseCharacter* ABaseCharacter::FindFurthestAlly()
{
	ABaseCharacter* FurthestAlly = nullptr;

	float MaxDistance = 0.0f;

	// 월드에 존재하는 모든 ABaseCharacter 찾기
	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), AllCharacters);

	for (AActor* Actor : AllCharacters)
	{
		ABaseCharacter* OtherChar = Cast<ABaseCharacter>(Actor);
		if (!OtherChar || OtherChar == this || OtherChar->bIsDead)
			continue;

		// 같은 팀인지 확인
		if (OtherChar->TeamID != this->TeamID)
			continue;

		// 거리 계산
		float Distance = FVector::Dist(this->GetActorLocation(), OtherChar->GetActorLocation());
		if (Distance > MaxDistance)
		{
			MaxDistance = Distance;
			FurthestAlly = OtherChar;
		}
	}

	return FurthestAlly; // 없을 경우 nullptr 반환
}
void ABaseCharacter::PerformHealShieldSkill(float& CurrCooldown, float Duration, FTimerHandle HealSpeedTimerHandle, float DeltaTime)
{
    if (bIsDead) return;
    if (CurrCooldown > 0.f) return;

    ABaseCharacter* FoundAlly = FindFurthestAlly();
    if (!FoundAlly) return;

    if (FoundAlly->CurrentShieldVisual && IsValid(FoundAlly->CurrentShieldVisual))
    {
        FoundAlly->CurrentShieldVisual->Destroy();
        FoundAlly->CurrentShieldVisual = nullptr;
    }

    ABubbleVisualShieldActor* ShieldVisual = nullptr;
    if (BubbleShieldVisualClass)
    {
        FVector SpawnLocation = FoundAlly->GetActorLocation() + FVector(0, 0, 100.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        ShieldVisual = GetWorld()->SpawnActor<ABubbleVisualShieldActor>(BubbleShieldVisualClass, SpawnLocation, SpawnRotation);
        if (ShieldVisual)
        {
            ShieldVisual->AttachToActor(FoundAlly, FAttachmentTransformRules::KeepWorldTransform);
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("[HealSkill2]ShieldVisual COMPLETE!"));
            FoundAlly->CurrentShieldVisual = ShieldVisual;


        }
    }
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
        FString::Printf(TEXT("Before Speed: %.1f"), FoundAlly->GetCharacterMovement()->MaxWalkSpeed));
    float OriginalSpeed = FoundAlly->GetCharacterMovement()->MaxWalkSpeed;
    FoundAlly->GetCharacterMovement()->MaxWalkSpeed *= 1.8f;
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
        FString::Printf(TEXT("New Speed: %.1f (Old: %.1f)"), FoundAlly->GetCharacterMovement()->MaxWalkSpeed, OriginalSpeed));
    GetWorldTimerManager().SetTimer(HealSpeedTimerHandle, [FoundAlly, OriginalSpeed]()
        {
            if (FoundAlly)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("[HealSkill2]SPEED UP AND DOWN!"));
                FoundAlly->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
                //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Healer Speed Reset"));
            }

            if (FoundAlly->CurrentShieldVisual && IsValid(FoundAlly->CurrentShieldVisual))
            {
                FoundAlly->CurrentShieldVisual->Destroy();
                FoundAlly->CurrentShieldVisual = nullptr;
            }

        }, Duration, false);

    CurrCooldown = 10;
}


void ABaseCharacter::ResetHealSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed /= 2.5;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Healer Speed Reset"));
}

void ABaseCharacter::PerformHealerUltimateSkill()
{
	if (UltiNum < 3) return;
	 
	int32 RevivedCount = 0;

	for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
	{
		if (*It == this || It->TeamID != TeamID || !It->bIsDead) continue;

		It->UltimateRevive();
		break;
	}

	UltiNum = 0;
}

void ABaseCharacter::UltimateRevive()
{
    if (!bIsDead) return;

    // ?? 자동 부활 타이머 제거 (힐러가 먼저 부활시키는 경우 중복 방지)
    GetWorldTimerManager().ClearTimer(ReviveTimerHandle);


    bIsDead = false;

    CurrentHP = MaxHP;

    // 위치 복원
    SetActorLocation(ReviveLocation); // 필요 시 별도 저장

    // 다시 보이게
    SetActorHiddenInGame(false);

    // 충돌/입력/이동 복원
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // 메시
    if (GetMesh()) GetMesh()->SetVisibility(true, true);
    if (Gun) {
        Gun->SetActorHiddenInGame(false);
        Gun->SetActorEnableCollision(true);
        Gun->SetActorTickEnabled(true);
    }


    if (!IsPlayerControlled())
    {
        AController* MyController = GetController();
        if (!MyController)
        {
            SpawnDefaultController();
        }
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s has been revived. (UltimateRevive)"), *GetName()));
}