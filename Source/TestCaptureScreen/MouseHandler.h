// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MouseHandler.generated.h"

UCLASS(ClassGroup= (Custom), meta=(BlueprintSpawnableComponent))
class TESTCAPTURESCREEN_API UMouseHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UMouseHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "Mouse")
	void SimulateMouseClick(FVector2D position);

	UFUNCTION(BlueprintCallable, Category = "Mouse")
	void SetMousePosition(FVector2D position);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mouse")
	FVector2D GetWindowPosition();
};
