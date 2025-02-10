// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureScreen.generated.h"

// Enum para os modos de captura
UENUM(BlueprintType)
enum class ECaptureMode : uint8
{
	Window UMETA(DisplayName = "Window"),
	FullScreen UMETA(DisplayName = "Full Screen"),
	Monitor UMETA(DisplayName = "Monitor")
};

UCLASS()
class TESTCAPTURESCREEN_API ACaptureScreen : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACaptureScreen();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Render Target para onde a textura será desenhada
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	UTextureRenderTarget2D* RenderTarget;

	// Título da janela a ser capturada
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	FString WindowTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "Capture")
	ECaptureMode CaptureMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "Capture")
	int monitorIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	bool CaptureOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	bool AutoSetRenderTargetSize;

	// Função para atualizar o Render Target com a textura capturada
	UFUNCTION(BlueprintCallable, Category = "Capture")
	void UpdateRenderTarget();

	// Função para simular um clique do mouse
	UFUNCTION(BlueprintCallable, Category = "Mouse")
	void SimulateMouseClick(FVector2D position);

	UFUNCTION(BlueprintCallable, Category = "Windows")
	TArray<FString> ListOpenWindowTitles();

private:

	float sRGBToLinear(float value);

};


