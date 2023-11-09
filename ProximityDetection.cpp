#include "ProximityDetectionComponent.h"
#include "GameplayTagsManager.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UProximityDetectionComponent::UProximityDetectionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Create the detection sphere component
    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UProximityDetectionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Ensure the owner is valid
    if (GetOwner())
    {
        // Attach the sphere to the root component of the owner
        DetectionSphere->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

        // Set the sphere radius
        DetectionSphere->SetSphereRadius(DetectionRange);

        // Register the overlap events
        DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &UProximityDetectionComponent::OnOverlapBegin);
        DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &UProximityDetectionComponent::OnOverlapEnd);
    }
}

void UProximityDetectionComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Early exit if overlap is with self or other actor is not valid
    if (!OtherActor || OtherActor == GetOwner() || !OtherComp) return;

    // Try to get the ProximityDetectionComponent from the other actor
    UProximityDetectionComponent* OtherProximityComponent = OtherActor->FindComponentByClass<UProximityDetectionComponent>();
    if (OtherProximityComponent)
    {
        // Now we can access the TeamTags of the other component directly
        const FGameplayTagContainer& OtherActorTags = OtherProximityComponent->GetTeamTags();

        // Check if the other actor's tags do not contain any of the same tags as this actor's tags
        bool IsEnemy = !TeamTags.HasAny(OtherActorTags);

        if (IsEnemy)
        {
            // Broadcast the proximity event
            OnProximityDetected.Broadcast(OtherActor, IsEnemy);

            // Debug messages for development builds only
#if !UE_BUILD_SHIPPING
            if (GEngine)
            {
                FString Message = FString::Printf(TEXT("%s detected %s. Enemy: Yes"),
                    *GetOwner()->GetName(),
                    *OtherActor->GetName());
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
            }
#endif
        }
    }
}

void UProximityDetectionComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd called for actor: %s"), *GetOwner()->GetName());
#endif

    if (!OtherActor || OtherActor == GetOwner() || !OtherComp) return;

    // Broadcasting that an actor is no longer overlapping
    OnProximityEnded.Broadcast(OtherActor);

    // Debug messages for development builds only
#if !UE_BUILD_SHIPPING
    FString Message = FString::Printf(TEXT("%s no longer detected %s."),
        *GetOwner()->GetName(),
        *OtherActor->GetName());
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
#endif
}


void UProximityDetectionComponent::SetTeamTagFromNumber(int32 InTeamNumber)
{
    // Clear previous tags related to teams
    TeamTags.Reset();

    // Create a tag for the new team number
    FString TagName = FString::Printf(TEXT("Team.Team%d"), InTeamNumber);
    FGameplayTag NewTeamTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*TagName), true);

    // Add the new team tag to the container
    TeamTags.AddTag(NewTeamTag);
}
