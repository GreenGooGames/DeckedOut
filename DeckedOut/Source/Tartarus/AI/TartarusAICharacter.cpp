// Fill out your copyright notice in the Description page of Project Settings.


#include "TartarusAICharacter.h"

#include "Components/StateTreeComponent.h"

ATartarusAICharacter::ATartarusAICharacter()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(FName("StateTreeComponent"));
}
