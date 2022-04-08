// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "TrigerSpawn.generated.h"

UCLASS()
class MYBOWLING_API ATrigerSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrigerSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> SphereClass;
	// we will spawn Blueprint objects that we choose in UE editor

	void SpawnItem(UClass* ItemToSpawn);// this procedure spawn all the spheres for the level
	FVector SpawnLocation;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = Location)
		FVector NewLocation;


	// events on the actors enter or leavig the trigger

	UFUNCTION()	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



};
