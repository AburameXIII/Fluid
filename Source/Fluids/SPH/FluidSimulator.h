// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particle.h"
#include "../../../Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../Engine/Source/Runtime/Engine/Classes/VectorField/VectorField.h"
#include "../../../Engine/Source/Runtime/Engine/Classes/VectorField/VectorFieldVolume.h"
#include "../../../Engine/Source/Runtime/Engine/Classes/VectorField/VectorFieldAnimated.h"
#include "Billboard.h"
#include "FluidSimulator.generated.h"

UCLASS()
class FLUIDSIMULATION_API AFluidSimulator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFluidSimulator();

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* NiagaraPS;
	

	UPROPERTY(EditAnywhere)
		UVectorFieldAnimated* VFA;

	FVectorFieldInstance* FVAI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float SpawnCooldown;

};
