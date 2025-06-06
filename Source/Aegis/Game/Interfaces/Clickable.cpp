// Fill out your copyright notice in the Description page of Project Settings.


#include "Clickable.h"


// Add default functionality here for any IClickable functions that are not pure virtual.
void IClickable::Clicked_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("IClickable::Clicked_Implementation() - Clicked"));
}
