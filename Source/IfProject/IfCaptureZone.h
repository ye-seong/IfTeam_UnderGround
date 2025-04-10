// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseCharacter.h"
#include "IFCaptureZone.generated.h"

UCLASS()
class AIFCaptureZone : public AActor
{
    GENERATED_BODY()

public:
    AIFCaptureZone();

protected:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    // 점령 관련
    float CaptureTime = 100.0f; // 점령 완료까지 필요한 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    float SubCaptureTime = 3.0f; // 자신의 팀 거점으로 만들기 까지 필요한 시간
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    float TeamA_Gauge = 0;  // 0팀 현재 거점 게이지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    float TeamB_Gauge = 0;  // 1팀 현재 거점 게이지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    float TeamA_SubGauge = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    float TeamB_SubGauge = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    int32 CurrentCapturingTeam = 2;  // 현재 거점을 가지고 있는 팀

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    bool IsEnd = false;

    void UpdateCaptureInfo(float DeltaTime);
    void UpdateCapture(float DeltaTime);
    void UpdateSubCapture(float DeltaTime);

    UFUNCTION()
    void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    bool IsCloseMatch();

    void GetOverlappingPlayers(TArray<UPrimitiveComponent*> Components);

    TArray<ABaseCharacter*> OverlappingPlayers;

    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CaptureBox;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Capture")
    int32 CaptureOrderIndex;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Capture")
    FName ZoneName;
};

