// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerEliminated.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATriggerEliminated::ATriggerEliminated()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetWorldScale3D(FVector(1.f, 1.0f, 1.0f));
    CollisionBox->SetBoxExtent(FVector(60.f, 10.f, 5.f));

    CollisionBox->SetCollisionProfileName("Trigger");
    RootComponent = CollisionBox;

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATriggerEliminated::OnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ATriggerEliminated::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ATriggerEliminated::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATriggerEliminated::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  /*  if (DestrActor)
    {
        if (GetWorld()->GetTimeSeconds()- BegDestrTime > 1.f
        )
        {

         DestrActor->Destroy();

        }
    }*/
}
    
void ATriggerEliminated::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    //other actor is the actor that triggered the event. Check that is not ourself.

    AMyBowlingCharacter* MyBowlingCharacter;
    MyBowlingCharacter = Cast< AMyBowlingCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (MyBowlingCharacter->ElimTriggerEnabled == true)
    {
 //       GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Blow!!!");
        
        Total = 0;
        if (Number == 1)
        {
            MyBowlingCharacter->Payer1_Score = Total;
         }
        if (Number == 2)
        {
             MyBowlingCharacter->Payer2_Score = Total;
        }
    }
    else
     {  
           
    }
}
void ATriggerEliminated::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AMyBowlingCharacter* MyBowlingCharacter;
    MyBowlingCharacter = Cast< AMyBowlingCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (MyBowlingCharacter->ElimTriggerEnabled == true)
    { 
    if(OtherActor->GetClass() == PinClass) {
        Total++;
        FString Count = FString::FromInt((Total));
        BegDestrTime = GetWorld()->GetTimeSeconds();
        DestrActor = OtherActor;
 //       GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Eliminated   "+ Count);
        


        if (Number == 1)
        {
            MyBowlingCharacter->Payer1_Score = Total;
        }
        if (Number == 2)
        {
            MyBowlingCharacter->Payer2_Score = Total;
        }

        OtherActor->Destroy();
        
    }
    }
    else
    {
        Total = 0;
        if (Number == 1)
        {
        MyBowlingCharacter->Payer1_Score = Total;
        }
        if (Number == 2)
        {
        MyBowlingCharacter->Payer2_Score = Total;
        }
    }
   

}
