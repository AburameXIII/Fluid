// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <cstdint>
#include <vector>
#include "Engine/Texture2D.h"
#include "Components/BillboardComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Billboard.generated.h"

UCLASS()
class FLUIDSIMULATION_API ABillboard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABillboard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UBillboardComponent holds a texture that always faces the camera
	UPROPERTY(EditAnywhere)
	class UBillboardComponent* Billboard;
};

class Texture2D {
public:

	typedef uint8_t value_type;

	Texture2D(unsigned w, unsigned h) {
		resize(w, h);
	}

	Texture2D& resize(unsigned w, unsigned h) {
		mWidth = w;
		mHeight = h;
		mData.resize(w * h * numComps());
		return *this;
	}
	unsigned width() const { return mWidth; }
	unsigned height() const { return mHeight; }
	unsigned numComps() const { return 4; }

	std::vector<value_type>& data() { return mData; }
	const std::vector<value_type>& data() const { return mData; }

	value_type* pixel(unsigned x, unsigned y) {
		return &mData[(y * mWidth + x) * numComps()];
	}


	// Create a new UTexture2D object based on current texture state
	UTexture2D* newUTexture2D(FName name = TEXT("")) {

		//auto * newTex = NewObject<UTexture2D>(outer, name, RF_Public | RF_Standalone | RF_MarkAsRootSet);
		// Calls NewObject under the hood
		auto* newTex = UTexture2D::CreateTransient(mWidth, mHeight, EPixelFormat::PF_B8G8R8A8, name);
		//newTex->AddToRoot(); // prevent from being GC'ed

		if (newTex) {
			updateUTexture2D(newTex);

			/*
			//Initialize the platform data to store necessary information regarding our texture asset
			newTex->PlatformData = new FTexturePlatformData();
			newTex->PlatformData->SizeX = mWidth;
			newTex->PlatformData->SizeY = mHeight;
			//newTex->PlatformData->NumSlices = 1; // No longer defined in FTexturePlatformData
			newTex->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;

			//Allocate first mipmap.
			auto * mip = new FTexture2DMipMap();
			newTex->PlatformData->Mips.Add(mip);
			mip->SizeX = mWidth;
			mip->SizeY = mHeight;

			//Lock the mipmap data so it can be modified
			mip->BulkData.Lock(LOCK_READ_WRITE);
			auto * mipData = (value_type *)mip->BulkData.Realloc(numElems);
			//Copy the pixel data into the Texture data
			FMemory::Memcpy(mipData, &mData[0], sizeof(value_type) * numElems);
			mip->BulkData.Unlock();

			//Initialize the new texture
			newTex->Source.Init(mWidth, mHeight, 1, 1, ETextureSourceFormat::TSF_BGRA8, &pixels[0]);
			newTex->UpdateResource();
			//*/
		}

		return newTex;
	}

	// Copy current pixel data over to a UTexture2D.
	// The dimensions of this texture and the UTexture2D must match.
	// Returns whether the copy was successful.
	bool updateUTexture2D(UTexture2D* utex) {

		if (utex && utex->PlatformData->Mips.Num()) {
			// Get the first mipmap (an FTexture2DMipMap)
			auto& mip = utex->PlatformData->Mips[0];

			if (mip.SizeX == mWidth && mip.SizeY == mHeight) {
				// Lock the mipmap data so it can be modified
				auto* mipData = mip.BulkData.Lock(LOCK_READ_WRITE);
				// Copy the pixel data into the Texture data
				FMemory::Memcpy(mipData, &mData[0], sizeof(value_type) * mWidth * mHeight * numComps());
				mip.BulkData.Unlock();
				utex->UpdateResource();
				return true;
			}
		}
		return false;
	}


private:
	std::vector<value_type> mData;
	unsigned mWidth = 0, mHeight = 0;
};

