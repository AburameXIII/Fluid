// Fill out your copyright notice in the Description page of Project Settings.


#include "Particle.h"

// Sets default values
AParticle::AParticle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//Initializes the mesh
	ParticleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParticleMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (PAsset.Succeeded()) {
		ParticleMesh->SetStaticMesh(PAsset.Object);
		ParticleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		ParticleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		//ParticleMesh->SetSimulatePhysics(true);
		
	}

	//Initializes the materials
	//static ConstructorHelpers::FObjectFinder<UMaterial> RoomWallMaterialR(TEXT("Material'/Game/RoomWallMaterial.RoomWallMaterial'"));
	//RoomWallMaterial = RoomWallMaterialR.Object;
}

// Called when the game starts or when spawned
void AParticle::BeginPlay()
{
	Super::BeginPlay();
	//SetActorScale3D(FVector(0.2f, 0.2f, 0.2f));
	Forces = FVector(0, 0, -9.8) * 10;
	Position = GetActorLocation();
}

// Called every frame
void AParticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Velocity += Forces * DeltaTime;
	Position += Velocity * DeltaTime;
	//SetActorLocation(GetActorLocation() + Velocity * DeltaTime, true,nullptr,ETeleportType::ResetPhysics);
}

