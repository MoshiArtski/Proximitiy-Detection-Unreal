#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "GameplayTagContainer.h"
#include "ProximityDetectionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProximityEvent, AActor*, DetectedActor, bool, IsEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProximityEndEvent, AActor*, DetectedActor);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROXIMITYCHECK_API UProximityDetectionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UProximityDetectionComponent();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Proximity")
    TObjectPtr<USphereComponent> DetectionSphere;

    UPROPERTY(BlueprintAssignable, Category = "Proximity")
    FProximityEvent OnProximityDetected;

    UPROPERTY(BlueprintAssignable, Category = "Proximity")
    FProximityEndEvent OnProximityEnded;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
    float DetectionRange = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
    float CheckTimeInterval = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
    int32 TeamNumber;

    // This container will hold all team-related tags.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
    FGameplayTagContainer TeamTags;

    UFUNCTION(BlueprintCallable, Category = "Proximity")
    const FGameplayTagContainer& GetTeamTags() const { return TeamTags; }

protected:
    virtual void BeginPlay() override;

    void SetTeamTagFromNumber(int32 TeamNumber);

private:
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
