#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Headphone.generated.h"

class AVRCharacterParent; // Forward declaration

UCLASS()
class TESTCAPTURESCREEN_API AHeadphone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHeadphone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	// Root Scene Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* RootScene;

	// Box Collision Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComponent;

	// Function to handle overlap
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// Blueprint implementable event
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void AttachToCharacterHead(AVRCharacterParent* Character);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void StartMusic();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void StopMusic();

};
