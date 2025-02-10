// Fill out your copyright notice in the Description page of Project Settings.
#include "MouseHandler.h"
#include <Windows.h>


// Sets default values for this component's properties
UMouseHandler::UMouseHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMouseHandler::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMouseHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UMouseHandler::SimulateMouseClick(FVector2D position)
{
    // Configura o evento de clique do mouse
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN; // Pressiona o botão esquerdo
    SendInput(1, &input, sizeof(INPUT));
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP; // Solta o botão esquerdo
    SendInput(1, &input, sizeof(INPUT));
}

/*
void UMouseHandler::SimulateMouseClick(FVector2D position)
{
    // Posiciona o cursor do mouse
    SetCursorPos(position.X, position.Y);

    // Configura os eventos de clique do mouse
    INPUT input[2] = { 0 };

    // Pressiona o botão esquerdo
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    // Solta o botão esquerdo
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    // Envia os eventos
    SendInput(2, input, sizeof(INPUT));
}*/


void UMouseHandler::SetMousePosition(FVector2D position)
{
	// Posiciona o mouse em uma posição específica na tela
	SetCursorPos(position.X, position.Y); // Altere as coordenadas conforme necessário
}

FVector2D UMouseHandler::GetWindowPosition()
{
	// Obtém a posição da janela no Windows
	RECT windowRect;
	if (GetWindowRect(GetForegroundWindow(), &windowRect))
	{
		int windowX = windowRect.left;
		int windowY = windowRect.top;
		return FVector2D(windowX, windowY);
	}
	return FVector2D::ZeroVector;
}