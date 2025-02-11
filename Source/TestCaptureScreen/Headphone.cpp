#include "Headphone.h"
#include "Components/BoxComponent.h"
#include "VRCharacterParent.h" // Include the header for AVRCharacterParent

// Sets default values
AHeadphone::AHeadphone()
{
	// Create and set up Root Scene Component
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	// Create and set up Box Component
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootScene); // Attach BoxComponent to RootScene
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void AHeadphone::BeginPlay()
{
	Super::BeginPlay();
	// Bind the overlap event
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AHeadphone::OnBoxOverlap);
}

void AHeadphone::OnBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (AVRCharacterParent* Character = Cast<AVRCharacterParent>(OtherActor))
	{
		// Call the Blueprint implementable event
		AttachToCharacterHead(Character);
	}
}
