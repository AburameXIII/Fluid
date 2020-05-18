// Fill out your copyright notice in the Description page of Project Settings.


#include "ParticlesSystem.h"

#include "Components/BillboardComponent.h"
#include <string>
#include <algorithm> // max
#include "Colission.h"
#include "Billboard.h"


void FBillboardParticle::update(float dt) {

	/*
	vel += acc * dt;
	pos += vel * dt;*/
	age += dt;
	
	
	FVector fTotal = fGravity + fViscosity +fPressure;// +fPressure + fViscosity + fSurfaceTension;
	//FVector fTotal = fGravity;
	//GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, "Force: " + fGravity.ToString() + ", density: " + FString::SanitizeFloat(density) + ", pressure: " + FString::SanitizeFloat(pressure));

	if (FMath::Abs(density) > 0) {
		acc = fTotal / density;
	}

	vel += acc * dt;
	if (vel.Size() > 300.f) vel = vel/vel.Size() * 300.f;

	pos += vel * dt;// +acc / 2 * dt * dt;
	
}

void FBillboardParticle::Draw() {
	if (billboard) billboard->SetRelativeLocation(pos * 100);
}


FBillboardParticle& UBillboardParticles::nextParticle() {
	++currIndex %= particles.Num();
	auto& p = particles[currIndex];
	p.age = 0.;
	return p;
}

void UBillboardParticles::update(float dt) {
	for (auto& p : particles) p.update(dt);

}

void UBillboardParticles::Draw() {
	for (auto& p : particles) p.Draw();
}

// Sets default values
AParticlesSystem::AParticlesSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	

	// Generate a texture to use as billboard sprite 128 original size
	Texture2D tex(8, 8);
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
			pixel[0] = 255. *u;
			pixel[1] = 0 * v;
			pixel[2] = 0;
			pixel[3] = rsqr < 1. ? 255 : 0; // cut-out a circle
		}
	}

	// Returns an Unreal UTexture2D
	auto* utex = tex.newUTexture2D();

	particleSystem = CreateDefaultSubobject<UBillboardParticles>("particleSystem");
	RootComponent = particleSystem;

	auto& particles = particleSystem->particles;

	// Set the number of particles in our system
	particles.SetNum(50);

	// Initialize all the particles
	for (int i = 0; i < particles.Num(); ++i) {
		auto& p = particles[i];

		//EZ_PRINTF("particle %p\n", &p);

		// Generate a name for the particle (req'd by CreateDefaultSubobject)
		auto name = FName(("p" + std::to_string(i)).c_str());

		// Create the billboard
		p.billboard = CreateDefaultSubobject<std::decay<decltype(*p.billboard)>::type>(name);

		// Set billboard sprite to our texture generated above
		p.billboard->Sprite = utex;

		// Yes, you really need to do this...
		p.billboard->bHiddenInGame = false;

		// Attach particle to particle system component
		p.billboard->SetupAttachment(particleSystem);

		
	}

}

// Called when the game starts or when spawned
void AParticlesSystem::BeginPlay()
{
	Super::BeginPlay();

	


	
}

// Called every frame
void AParticlesSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 10% of the time, emit a new particle
	//if (FMath::RandRange(0,100) < 10) {
	//	auto& p = particleSystem->nextParticle();
	//	p.pos = FVector(0.2f, 0.2f, 0.2f);	// random position within a sphere
	//	p.acc = FVector(0, 0, -9.8f); // tiny bit of gravity
	//}

	// Add some jitter to each particle (based on a random walk)
	//for (auto& p : particleSystem->particles) {
	//	p.vel += rng.ball<FVector>() * 10.;
	//}
	particleSystem->update(DeltaTime);
	CalculateNeighbours();
	ComputeDensity();

	ComputeInternalForces();
	ComputeGravityForce();
	ComputeSurfaceTension();

	

	// Update particle positions
	
	Colission::detectCollisions((*particleSystem), 1.f);
	particleSystem->Draw();
}


static double defaultKernelLaplacian(FVector differenceParticleNeighbour) {
	// (Formula 4.5)
	const double particleDistanceSqr = differenceParticleNeighbour.SizeSquared();
	return KernelDefaultGradientMultiplier * (SupportRadiusSqr - particleDistanceSqr)
		* (3.0 * SupportRadiusSqr - 7.0 * particleDistanceSqr);
}


static FVector defaultKernelGradient(FVector differenceParticleNeighbour) {
	// (Formula 4.4)
	const double particleDistanceSqr = differenceParticleNeighbour.SizeSquared();
	return differenceParticleNeighbour * KernelDefaultGradientMultiplier
		* (SupportRadiusSqr - particleDistanceSqr)
		* (SupportRadiusSqr - particleDistanceSqr);
}

static double defaultKernel(FVector differenceParticleNeighbour) {
	// (Formula 4.3)
	const double particleDistanceSqr = differenceParticleNeighbour.SizeSquared();
	return KernelDefaultMultiplier * pow(SupportRadiusSqr - particleDistanceSqr, 3);
}

static FVector pressureKernelGradient(FVector differenceParticleNeighbour) {
	// (Formula 4.14)
	const double particleDistance = differenceParticleNeighbour.Size();
	return differenceParticleNeighbour * KernelPressureGradientMultiplier / particleDistance
		* (WaterSupportRadius - particleDistance)
		* (WaterSupportRadius - particleDistance);
}

static double viscosityKernelLaplacian(FVector differenceParticleNeighbour) {
	// (Formula 4.22)
	const double particleDistance = differenceParticleNeighbour.Size();
	return KernelViscosityLaplacianMultiplier * (WaterSupportRadius - particleDistance);
}

void AParticlesSystem::ComputeDensity()
{
	// (Formula 4.6)
	for (size_t i = 0; i < particleSystem->particles.Num(); i++)
	{
		particleSystem->particles[i].density = OwnDensity;

		for (size_t j = 0; j < particleSystem->particles[i].neighbours.Num(); j++)
		{
			int NeighbourPosition = particleSystem->particles[i].neighbours[j];
			const FVector differenceParticleNeighbour = particleSystem->particles[i].pos - particleSystem->particles[NeighbourPosition].pos;

			//IF not needed since its guaranteed neighbour
			if (WaterSupportRadius > differenceParticleNeighbour.Size())
				particleSystem->particles[i].density += WaterParticleMass * defaultKernel(differenceParticleNeighbour);
		}

		particleSystem->particles[i].pressure = WaterStiffness * (particleSystem->particles[i].density - WaterDensity);
	}
}



void AParticlesSystem::ComputeInternalForces()
{
	for (size_t i = 0; i < particleSystem->particles.Num(); i++)
	{
		particleSystem->particles[i].fPressure = FVector::ZeroVector;
		particleSystem->particles[i].fViscosity = FVector::ZeroVector;

		//Neighbours
		for (size_t j = 0; j < particleSystem->particles[i].neighbours.Num(); j++)
		{
			int NeighbourPosition = particleSystem->particles[i].neighbours[j];
			if ((particleSystem->particles[i].density) <= 0.) continue;
			if ((particleSystem->particles[NeighbourPosition].density) <= 0.) continue;



			const FVector differenceParticleNeighbour =
				particleSystem->particles[i].pos - particleSystem->particles[NeighbourPosition].pos;

			const double particleDistance = differenceParticleNeighbour.Size();

			if (particleDistance > 0.)
			{
				const double dividedMassDensity =
					WaterParticleMass / particleSystem->particles[NeighbourPosition].density;

				// (Formulae 4.11 & 4.14)
				particleSystem->particles[i].fPressure +=
					pressureKernelGradient(differenceParticleNeighbour) *
					(particleSystem->particles[i].pressure + particleSystem->particles[NeighbourPosition].pressure) *
					dividedMassDensity;
				
				
				// (Formulae 4.17 & 4.22)
				particleSystem->particles[i].fViscosity += 
					(particleSystem->particles[NeighbourPosition].vel - particleSystem->particles[i].vel) *
					viscosityKernelLaplacian(differenceParticleNeighbour) * dividedMassDensity;
			}
		}

		particleSystem->particles[i].fPressure *= -0.5;
		particleSystem->particles[i].fViscosity *= WaterViscosity;

		//particleSystem->particles[i].fInternal = particleSystem->particles[i].fPressure + particleSystem->particles[i].fViscosity;
	}
}



void AParticlesSystem::ComputeGravityForce()
{
	for (int i = 0; i < particleSystem->particles.Num(); i++) {
		particleSystem->particles[i].fGravity = GravitationalAcceleration * particleSystem->particles[i].density;
	}
		
}

void AParticlesSystem::ComputeSurfaceTension()
{
	for (size_t i = 0; i < particleSystem->particles.Num(); i++)
	{
		particleSystem->particles[i].fSurfaceTension = FVector::ZeroVector;

		FVector surfaceTensionGradient = FVector::ZeroVector;
		double surfaceTensionLaplacian = 0.0;

		//neighbours
		for (size_t j = 0; j < particleSystem->particles[i].neighbours.Num(); j++)
		{
			int NeighbourPosition = particleSystem->particles[i].neighbours[j];
			if ((particleSystem->particles[i].density) <= 0.)continue;
			if ((particleSystem->particles[NeighbourPosition].density) <= 0.)continue;

			const FVector differenceParticleNeighbour =
				particleSystem->particles[i].pos - particleSystem->particles[NeighbourPosition].pos;

			if (differenceParticleNeighbour.SizeSquared() <= SupportRadiusSqr)
			{
				const double dividedMassDensity =
					WaterParticleMass / particleSystem->particles[NeighbourPosition].density;

				// (Formulae 4.28 & 4.4)
				surfaceTensionGradient += defaultKernelGradient(differenceParticleNeighbour) * dividedMassDensity;

				// (Formulae 4.27 & 4.5)
				surfaceTensionLaplacian += defaultKernelLaplacian(differenceParticleNeighbour) * dividedMassDensity;
			}
		}

		// (Formulae 4.32 & 5.17)
		//neighbours size at the end TODO
		if (surfaceTensionGradient.Size() >= std::sqrt(WaterDensity / particleSystem->particles[i].neighbours.Num()))
			// (Formula 4.26 is presented by combination of 4.27 & 4.5 - laplacian - and 4.28 & 4.4 - gradient)
			particleSystem->particles[i].fSurfaceTension = -surfaceTensionGradient / surfaceTensionGradient.Size() *
			surfaceTensionLaplacian * WaterSurfaceTension;
	}
}


void AParticlesSystem::CalculateNeighbours() {

	for (auto p: particleSystem->particles)
	{
		p.neighbours.Empty();
	}

	for (int i = 0; i < particleSystem->particles.Num() - 1; i++)
	{
		for (int j = i + 1; j < particleSystem->particles.Num(); j++)
		{
			FVector differenceParticleNeighbour = particleSystem->particles[i].pos - particleSystem->particles[j].pos;
			if (differenceParticleNeighbour.Size() < WaterSupportRadius) {
				particleSystem->particles[i].neighbours.Add(j);
				particleSystem->particles[j].neighbours.Add(i);
			}
		}

	}
}