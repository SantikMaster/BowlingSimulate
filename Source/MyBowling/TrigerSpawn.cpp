// Fill out your copyright notice in the Description page of Project Settings.


#include "TrigerSpawn.h"

// Sets default values
ATrigerSpawn::ATrigerSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetWorldScale3D(FVector(1.f, 1.0f, 1.0f));
    CollisionBox->SetBoxExtent(FVector(60.f, 10.f, 5.f));
    
    CollisionBox->SetCollisionProfileName("Trigger");
    RootComponent = CollisionBox;

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATrigerSpawn::OnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ATrigerSpawn::OnOverlapEnd);

}

// Called when the game starts or when spawned
void ATrigerSpawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrigerSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ATrigerSpawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    //other actor is the actor that triggered the event. Check that is not ourself.
 //   if (OtherActor->IsA(AMyBowlingCharacter::StaticClass()))
    {
    //    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "enter the spawn area");

    }
}
void ATrigerSpawn::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    //other actor is the actor that triggered the event. Check that is not ourself.


    UE_LOG(LogTemp, Warning, TEXT("Left Trigger "));

    FVector Location(-325.f, -2790.f, 345.f);

    GetWorld()->SpawnActor<AActor>(OtherActor->GetClass(), Location, FRotator(0.f));

  //  UClass** MappedClass = OldToNewClassMap.Find(OldActor->GetClass());
   // UClass* SpawnClass = MappedClass ? *MappedClass : NewClass;
   //   FActorSpawnParameters SpawnInfo;
  /*   SpawnInfo.OverrideLevel = ActorLevel;
    SpawnInfo.Template = NewArchetype;
    SpawnInfo.bNoCollisionFail = true;
    SpawnInfo.bDeferConstruction = true;

    // Temporarily remove the deprecated flag so we can respawn the Blueprint in the level
    const bool bIsClassDeprecated = SpawnClass->HasAnyClassFlags(CLASS_Deprecated);
    SpawnClass->ClassFlags &= ~CLASS_Deprecated;

    AActor* NewActor = World->SpawnActor(SpawnClass, &Location, &Rotation, SpawnInfo);*/


   //   SpawnInfo.Template = OtherActor->IsTemplate;
   // UClass* SpawnClass = OtherActor->GetClass();

 //   AActor* NewActor = GetWorld()->SpawnActor<AActor>(SpawnClass, Location, FRotator(0.f));
 //   GetWorld()->SpawnActor<AActor>(SphereClass, Location, FRotator(0.f));

  /*  if (OtherActor->IsA(AMyProject3Character::StaticClass()))
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "You left the spawn area");

    }
    else if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        DestroyedObjects++;  // destroyed odjects counter
        GEngine->AddOnScreenDebugMessage(-1, 3.f,
            FColor::Red, "Destroyed: " + OtherActor->GetName() + ", Total = " + FString::FromInt(DestroyedObjects));    //  "Destroyed" 

    }

    if (DestroyedObjects == HadToBeDestr) // the level ends when player destroys certain amount of spheres
    {
        // setting new spawn location and new level conditions

        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "NEW WAVE");
        DestroyedObjects = 0;
        SpawnLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
        SetActorLocation(SpawnLocation);


        Radius *= 1.05; //  conditions for a new level
        SpawnNumber *= 1.1;
        SpawnItem(SphereClass);


    }
*/
}
// new
