// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Fluid.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLUIDS_API UFluid : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFluid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int N;
	int size;
	int iter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float diffusion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float viscosity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialDiffusion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialViscosity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitalFade;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> density;
	TArray<float> s;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Vx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Vy;

	TArray<float> V0x;
	TArray<float> V0y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* RT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D position;
	FVector2D prevPosition;
	FVector2D prevVelocity;


	UFUNCTION(BlueprintCallable, Category = "Utils")
	void UpdatePosition(FVector2D newposition);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	int IX(int x, int y);
	
	void AddDensity(int x, int y, float amount);
	void AddVelocity(int x, int y, float amountx, float amounty);
	


	void Diffuse(int b, TArray<float>& x, TArray<float>& x0, float diff, float dt);
	void LinSolve(int b, TArray<float>& x, TArray<float>& x0, float a, float c);
	void Project(TArray<float>& velocX, TArray<float>& velocY, TArray<float>& p, TArray<float>& div);
	void Advect(int b, TArray<float>& d, TArray<float>& d0, TArray<float>& velocX, TArray<float>& velocY, float dt);
	void SetBnd(int b, TArray<float>& x);

};


