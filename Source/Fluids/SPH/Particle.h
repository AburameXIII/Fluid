// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particle.generated.h"

UCLASS()
class FLUIDSIMULATION_API AParticle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticle();

	//Mesh of the particle
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ParticleMesh;

	UPROPERTY(VisibleAnywhere)
	FVector Position;
	UPROPERTY(VisibleAnywhere)
	FVector Velocity;
	UPROPERTY(VisibleAnywhere)
	FVector Forces;
	float density;
	float densityN;
	float pressure;
	float pressureN;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class UMaterial* ParticleMaterial;
};
