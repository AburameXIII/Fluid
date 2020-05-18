// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <cstdint>
#include <cmath>
#include <ctime> // time
#include "GameFramework/Actor.h"
#include "ParticlesSystem.generated.h"

static const int ParticleNumber = 100;
static const double ParticleRadius = 0.16;
static const double CollisionVelocityMultiplier = -0.5f;
static const double WaterSupportRadius = 1.0; //0.1
static const double WaterParticleMass = 20.0; //0.02
static const double WaterDensity = 998.29; //998.29
static const double WaterStiffness = 3.0; //3.0
static const double WaterViscosity = 3.5; //3.5
static const double WaterSurfaceTension = 0.0728; //0.0728
static const double PiPowH9 = 3.14159 * pow(WaterSupportRadius, 9);
static const double PiPowH6 = 3.14159 * pow(WaterSupportRadius, 6);
static const double SupportRadiusSqr = WaterSupportRadius * WaterSupportRadius;
static const double KernelDefaultMultiplier = 315.0 / (64.0 * PiPowH9);
static const double KernelDefaultGradientMultiplier = -945.0 / (32.0 * PiPowH9);
static const double KernelPressureGradientMultiplier = -45.0 / PiPowH6;
static const double KernelViscosityLaplacianMultiplier = 45.0 / PiPowH6;
static const double OwnDensity = 315.0 / (64.0 * 3.14159 * pow(WaterSupportRadius, 3));
static const FVector GravitationalAcceleration = FVector(0.0, 0.0, -9.82f);

// Particle that holds Unreal billboard
USTRUCT(BlueprintType)
struct FBillboardParticle {
	GENERATED_BODY()
		UPROPERTY(EditAnywhere)
		FVector pos;
	UPROPERTY(EditAnywhere)
		FVector vel;
	UPROPERTY(EditAnywhere)
		FVector acc;
	float age = 0.;
	UPROPERTY(EditAnywhere)
		float density;
	UPROPERTY(EditAnywhere)
	float pressure;
	UPROPERTY(EditAnywhere)
	FVector fPressure;
	UPROPERTY(EditAnywhere)
	FVector fViscosity;
	FVector fInternal;
	UPROPERTY(EditAnywhere)
	FVector fGravity;
	UPROPERTY(EditAnywhere)
	FVector fSurfaceTension;

	// Update position of particle
	void update(float dt);
	void Draw();

	FBillboardParticle() {
		pos = FVector(FMath::FRandRange(1.f,5.f), FMath::FRandRange(1.f, 5.f),3.f);
		vel = FVector(0,0,0);
		acc = FVector(0,0,0);
		density = 0;
		pressure = 0;
		fViscosity, fGravity, fSurfaceTension, fPressure = FVector::ZeroVector;
	}

	UPROPERTY()
	class UBillboardComponent* billboard;

	TArray<int> neighbours;
	
};


UCLASS(BlueprintType)
class UBillboardParticles : public USceneComponent {
public:
	GENERATED_BODY()

		// Array of all the particles
		UPROPERTY(EditAnywhere)
		TArray<FBillboardParticle> particles;

	

	// Return oldest particle from set
	FBillboardParticle& nextParticle();

	// Update all particles
	void update(float dt);
	void Draw();
	

private:
	int currIndex = -1; // read/write tap into array
};




UCLASS()
class FLUIDSIMULATION_API AParticlesSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticlesSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The particle system
	UPROPERTY(EditAnywhere)
		UBillboardParticles* particleSystem;

	void CalculateNeighbours();
	void ComputeDensity();
	void ComputeInternalForces();
	void ComputeGravityForce();
	void ComputeSurfaceTension();
};


