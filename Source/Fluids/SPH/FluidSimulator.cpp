// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidSimulator.h"

// Sets default values
AFluidSimulator::AFluidSimulator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NiagaraPS = CreateDefaultSubobject<UNiagaraComponent>("NiagaraPS");
	RootComponent = NiagaraPS;

	VFA = CreateDefaultSubobject<UVectorFieldAnimated>("VFA");
}

// Called when the game starts or when spawned
void AFluidSimulator::BeginPlay()
{
	Super::BeginPlay();
	
	// Generate a texture to use as billboard sprite 128 original size
	Texture2D tex(16, 16);
	for (unsigned j = 0; j < tex.height(); ++j) {
		for (unsigned i = 0; i < tex.width(); ++i) {

			float u = float(i) / tex.width();
			float v = float(j) / tex.height();
			float x = 2. * u - 1.;
			float y = 2. * v - 1.;

			// Radius squared from center of sprite
			float rsqr = x * x + y * y;

			// Set pixel color and alpha
			auto* pixel = tex.pixel(i, j);
			pixel[0] = 255. * u;
			pixel[1] = 0 * v;
			pixel[2] = 0;
			pixel[3] = rsqr < 1. ? 255 : 0; // cut-out a circle
		}
	}

	// Returns an Unreal UTexture2D
	auto* utex = tex.newUTexture2D();

	VFA->Texture = utex;

}

// Called every frame
void AFluidSimulator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	/*
	if (SpawnCooldown <= 0 && Fluid.Num() < ParticleNumber) {
		FVector Location = FVector(FMath::RandRange(0, 100), FMath::RandRange(0, 100), 1000.f);
		AParticle* NewParticle = GetWorld()->SpawnActor<AParticle>(AParticle::StaticClass(), Location, FRotator::ZeroRotator);
		Fluid.Add(NewParticle);

		SpawnCooldown = 0.1f;
	}
	else {
		SpawnCooldown -= DeltaTime;
	}

	
	for (auto P : Fluid) {
		P->SetActorLocation(P->GetActorLocation() + P->Velocity * DeltaTime, true);

	}
	

	for (auto P : Fluid) {
		auto NearbyParticles = Fluid.FilterByPredicate([&](const AParticle* otherP){
			return (P->GetActorLocation() - otherP->GetActorLocation()).Size() < 3000.f;
			});

		for (auto P2 : NearbyParticles) {
			auto Q = 1 - (P->GetActorLocation() - P2->GetActorLocation()).Size() / 3000.f;
			auto Q2 = pow(Q, 2);
			auto Q3 = pow(Q, 3);
			P->density += Q2;
			P->densityN += Q3;
			P2->density += Q2;
			P->densityN += Q3;
		}
	}

	for (auto P : Fluid) {
		TArray<AParticle*> NearbyParticles = Fluid.FilterByPredicate([&](const AParticle* otherP) {
			return (P->GetActorLocation() - otherP->GetActorLocation()).Size() < 3000.f;
			});

		P->pressure = 0.004f * (P->density);
		P->pressureN = 0.01f * (P->densityN);

		for (auto P2 : NearbyParticles) {
			auto Q = 1 - (P->GetActorLocation() - P2->GetActorLocation()).Size() / 3000.f;
			auto Q2 = pow(Q, 2);
			auto Q3 = pow(Q, 3);

			auto press = P->pressure + P2->pressure;
			auto displace = (press * Q2 + press * Q3) * DeltaTime * DeltaTime;
			auto a2bN = (P->GetActorLocation() - P2->GetActorLocation()).Normalize();
			P->SetActorLocation(P->GetActorLocation() + displace * a2bN, true);
			P2->SetActorLocation(P2->GetActorLocation() - displace * a2bN, true);
		}
	}
	*/
}

