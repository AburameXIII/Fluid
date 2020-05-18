// Fill out your copyright notice in the Description page of Project Settings.


#include "Billboard.h"

// Sets default values
ABillboard::ABillboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Billboard = CreateDefaultSubobject<std::decay<decltype(*Billboard)>::type>("Billboard");

	// Must set this so billboard shows up in game!!!
	Billboard->bHiddenInGame = false;

	/* Load a texture asset and assign to billboard sprite
	{
		// This is static so it only gets created once over the program lifetime
		static ConstructorHelpers::FObjectFinder<UTexture2D> asset(TEXT("Texture'/Engine/EngineDebugMaterials/TargetIcon.TargetIcon'"));
		if(asset.Succeeded()){
			Billboard->Sprite = asset.Object;
		}
	}
	//*/

	//* Generate texture procedurally and assign to billboard sprite
	Texture2D tex(128, 128);
	for (unsigned j = 0; j < tex.height(); ++j) {
		for (unsigned i = 0; i < tex.width(); ++i) {

			float u = float(i) / tex.width();
			float v = float(j) / tex.height();
			float x = 2. * u - 1.;
			float y = 2. * v - 1.;

			float rsqr = x * x + y * y;

			auto* pixel = tex.pixel(i, j);
			pixel[0] = 255. * u;
			pixel[1] = 255. * v;
			pixel[2] = 0;
			pixel[3] = rsqr > 1. ? 0 : 255;
		}
	}
	Billboard->Sprite = tex.newUTexture2D();
	//*/

	// Make billboard this Actor's scene graph root
	RootComponent = Billboard;

}

// Called when the game starts or when spawned
void ABillboard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABillboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


