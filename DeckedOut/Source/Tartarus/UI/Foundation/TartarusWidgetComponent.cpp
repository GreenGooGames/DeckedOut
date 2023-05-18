// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/TartarusWidgetComponent.h"

UTartarusWidgetComponent::UTartarusWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetDrawAtDesiredSize(true);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWidgetSpace(EWidgetSpace::Screen);
}

#pragma region UWidgetComponent
void UTartarusWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OrientToUp();
}

void UTartarusWidgetComponent::InitWidget()
{
	Super::InitWidget();

	if (IsValid(GetWidget()))
	{
		OnWidgetCreatedEvent().Broadcast(this);
	}
}
#pragma endregion

#pragma region Orientation
void UTartarusWidgetComponent::OrientToUp()
{
	FVector ActorCenter = FVector::ZeroVector;
	FVector BoundExtents = FVector::ZeroVector;
	GetOwner()->GetActorBounds(false, ActorCenter, BoundExtents, false);

	SetWorldLocationAndRotationNoPhysics(ActorCenter + Offset, FVector::UpVector.ToOrientationRotator());
}
#pragma endregion
