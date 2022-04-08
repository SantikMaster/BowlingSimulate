// Fill out your copyright notice in the Description page of Project Settings.


#include "Gabber.h"
#include "DrawDebugHelpers.h"
// Sets default values for this component's properties
UGabber::UGabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGabber::BeginPlay()
{
	Super::BeginPlay();
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No Handle "));
	}

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No Input "));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Input "));
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGabber::Release);

	}

	// ...
	
}
void UGabber::Grab()
{

	UE_LOG(LogTemp, Warning, TEXT("Grab"));

	auto HitResult = GetFirstPhysicsBody();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();
	if (!ComponentToGrab)
	{
		UE_LOG(LogTemp, Error, TEXT("No Hit Result "));
	}

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!ActorHit)
	{
		UE_LOG(LogTemp, Error, TEXT("No Actor Hit "));
	}




	if (ActorHit)
	{
		PhysicsHandle->GrabComponent(
			ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(),
			true); // allow rotation
	}

	FVector PlayerViewPointL;
	FRotator PlayerViewPointR;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointL, PlayerViewPointR);


	FVector LineTraceEnd = PlayerViewPointL + PlayerViewPointR.Vector() * Reach;

	/*DrawDebugLine(
		GetWorld(),
		PlayerViewPointL, LineTraceEnd,
		FColor(255, 0, 0),
		false, 0.f, 0.f,
		10.f
	);*/
}
void UGabber::Release()
{
	PhysicsHandle->ReleaseComponent();
	UE_LOG(LogTemp, Warning, TEXT("Release"));
}

const FHitResult UGabber::GetFirstPhysicsObjectFromTrace()
{
	FHitResult HitResult;

	FVector PlayerViewPointL;
	FRotator PlayerViewPointR;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointL, PlayerViewPointR);

	FVector LineTraceEnd = PlayerViewPointL + PlayerViewPointR.Vector() * Reach;



	FCollisionQueryParams queryParams(FName(""), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult, PlayerViewPointL, LineTraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), queryParams
	);

	return HitResult;
}

FHitResult UGabber::GetFirstPhysicsBody() const
{
	FVector PlayerViewPointL;
	FRotator PlayerViewPointR;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointL, PlayerViewPointR);


	FVector LineTraceEnd = PlayerViewPointL + PlayerViewPointR.Vector() * Reach;

	/*DrawDebugLine(
		GetWorld(),
		PlayerViewPointL, LineTraceEnd,
		FColor(255, 0, 0),
		false, 0.f, 0.f,
		10.f
	);*/
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No Handle "));
	}
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No Input "));
	}

	/*FCollisionQueryParams TraceParameters(FName(TEXT("")), true, GetOwner());
	FHitResult Hit;
	bool IsHit = GetWorld()->LineTraceSingleByObjectType(
		Hit, PlayerViewPointL, LineTraceEnd,
		FCollisionObjectQueryParams{ ECollisionChannel::ECC_PhysicsBody },//:ECC_WorldStatic
		TraceParameters

	);
	if (IsHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit!! is "));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("nO Hit!!  is"));
	}*/

	FHitResult HitResult;

	FCollisionQueryParams queryParams(FName(""), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult, PlayerViewPointL, LineTraceEnd, FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), queryParams
	);
	FHitResult Hit;
	Hit = HitResult;

	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit!! "));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("nO Hit!! "));
	}
	FHitResult();
	return Hit;
}



// Called every frame
void UGabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector PlayerViewPointL;
	FRotator PlayerViewPointR;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointL, PlayerViewPointR);


	FVector LineTraceEnd = PlayerViewPointL + PlayerViewPointR.Vector() * Reach;


	/*	DrawDebugLine(
			GetWorld(),
			PlayerViewPointL, LineTraceEnd,
			FColor(255, 0, 0),
			false, 0.f, 0.f,
			10.f
		);*/

	if (PhysicsHandle->GrabbedComponent)
	{

		UE_LOG(LogTemp, Warning, TEXT("gRABBED "));
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
	// ...
}

