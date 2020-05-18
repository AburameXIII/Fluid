// Fill out your copyright notice in the Description page of Project Settings.


#include "Fluid.h"

// Sets default values for this component's properties
UFluid::UFluid()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFluid::BeginPlay()
{
	Super::BeginPlay();

    N =  64;
    size = 256;
    //diffusion = 0.1f;

    diffusion = 0.001f;
    viscosity = 0.0001f;
    iter = 4;

    s.SetNum(N * N);
    density.SetNum(N * N);
    Vx.SetNum(N * N);
    Vy.SetNum(N * N);
    V0x.SetNum(N * N);
    V0y.SetNum(N * N);
}


// Called every frame
void UFluid::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AddDensity(position.X, position.Y, 5);

    FVector2D velocity = position - prevPosition;
    velocity *= 10;
    //if (velocity != FVector2D::ZeroVector) prevVelocity = velocity * 100;
    //GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, "Velocity: " + velocity.ToString() + " Position: " + position.ToString() + " PrevPosition: " + prevPosition.ToString());
    AddVelocity(position.X, position.Y, velocity.X, velocity.Y);
    //AddVelocity(position.X, position.Y, -100, -100);

    Diffuse(1, V0x, Vx, viscosity, DeltaTime);
    Diffuse(2, V0y, Vy, viscosity, DeltaTime);

    Project(V0x, V0y, Vx, Vy);

    Advect(1, Vx, V0x, V0x, V0y, DeltaTime);
    Advect(2, Vy, V0y, V0x, V0y, DeltaTime);

    Project(Vx, Vy, V0x, V0y);
    Diffuse(0, s, density, diffusion, DeltaTime);
    Advect(0, density, s, Vx, Vy, DeltaTime);

    for (auto& d : density) {
        d = FMath::Clamp(d - 0.03f * DeltaTime, 0.f, 3.f);
    }
}

void UFluid::UpdatePosition(FVector2D newposition) {
    prevPosition = position;
    position = newposition * (N-1);
    position.X = FMath::Clamp(position.X, 0.f, (float) N - 1);
    position.Y = FMath::Clamp(position.Y, 0.f, (float) N - 1);
}




int UFluid::IX(int x, int y) {
    int CX = FMath::Clamp(x, 0, N - 1);
    int CY = FMath::Clamp(y, 0, N - 1);
    return CX + CY * N;
}


void UFluid::AddDensity(int x, int y, float amount) {
    auto index = IX(x, y);
    density[index] += amount;
}

void UFluid::AddVelocity(int x, int y, float amountX, float amountY) {
    auto index = IX(x, y);
    Vx[index] += amountX;
    Vy[index] += amountY;
}



void UFluid::Diffuse(int b, TArray<float>& x, TArray<float>& x0, float diff, float dt) {
    auto a = dt * diff * (N - 2) * (N - 2);
    
    LinSolve(b, x, x0, a, 1 + 4 * a);
}


/*
    Function of solving linear differential equation
*/
void UFluid::LinSolve(int b, TArray<float>& x, TArray<float>& x0, float a, float c) {
    auto cRecip = 1.0 / c;
    for (auto t = 0; t < iter; t++) {
        for (auto j = 1; j < N - 1; j++) {
            for (auto i = 1; i < N - 1; i++) {
                x[IX(i, j)] =
                    (x0[IX(i, j)] +
                        a *
                        (x[IX(i + 1, j)] +
                            x[IX(i - 1, j)] +
                            x[IX(i, j + 1)] +
                            x[IX(i, j - 1)])) *
                    cRecip;
            }
        }
        SetBnd(b, x);
    }
}



/*
    Function of project : This operation runs through all the cells and fixes them up so everything is in equilibrium.
    - velocX : float[]
    - velocY : float[]
    = p : float[]
    - div : float[]
*/
void UFluid::Project(TArray<float>& velocX, TArray<float>& velocY, TArray<float>& p, TArray<float>& div) {
    for (auto j = 1; j < N - 1; j++) {
        for (auto i = 1; i < N - 1; i++) {
            div[IX(i, j)] =
                (-0.5 *
                    (velocX[IX(i + 1, j)] -
                        velocX[IX(i - 1, j)] +
                        velocY[IX(i, j + 1)] -
                        velocY[IX(i, j - 1)])) /
                N;
            p[IX(i, j)] = 0;
        }
    }

    SetBnd(0, div);
    SetBnd(0, p);
    LinSolve(0, p, div, 1, 4);

    for (auto j = 1; j < N - 1; j++) {
        for (auto i = 1; i < N - 1; i++) {
            velocX[IX(i, j)] -= 0.5 * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * N;
            velocY[IX(i, j)] -= 0.5 * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * N;
        }
    }

    SetBnd(1, velocX);
    SetBnd(2, velocY);
}



/*
    Function of advect: responsible for actually moving things around
    - b : int
    - d : float[]
    - d0 : float[]
    - velocX : float[]
    - velocY : float[]
    - velocZ : float[]
    - dt : float[]
*/
void UFluid::Advect(int b, TArray<float>& d, TArray<float>& d0, TArray<float>& velocX, TArray<float>& velocY, float dt) {
    int i0, i1, j0, j1;

    auto dtx = dt * (N - 2);
    auto dty = dt * (N - 2);

    int s0, s1, t0, t1;
    float tmp1, tmp2, x, y;

    float Nfloat = N;
    float ifloat, jfloat;
    int i, j;

    for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
        for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
            tmp1 = dtx * velocX[IX(i, j)];
            tmp2 = dty * velocY[IX(i, j)];
            x = ifloat - tmp1;
            y = jfloat - tmp2;

            if (x < 0.5) x = 0.5;
            if (x > Nfloat + 0.5) x = Nfloat + 0.5;
            i0 = x;
            i1 = i0 + 1.0;
            if (y < 0.5) y = 0.5;
            if (y > Nfloat + 0.5) y = Nfloat + 0.5;
            j0 = y;
            j1 = j0 + 1.0;

            s1 = x - i0;
            s0 = 1.0 - s1;
            t1 = y - j0;
            t0 = 1.0 - t1;

            d[IX(i, j)] =
                s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
        }
    }

    SetBnd(b, d);
}



/*
    Function of dealing with situation with boundary cells.
    - b : int
    - x : float[]
*/
void UFluid::SetBnd(int b, TArray<float>& x) {
    for (auto i = 1; i < N - 1; i++) {
        x[IX(i, 0)] = ((b == 2) ? -x[IX(i, 1)] : x[IX(i, 1)]);
        x[IX(i, N - 1)] = ((b == 2) ? -x[IX(i, N - 2)] : x[IX(i, N - 2)]);
    }
    for (auto j = 1; j < N - 1; j++) {
        x[IX(0, j)] = ((b == 1) ? -x[IX(1, j)] : x[IX(1, j)]);
        x[IX(N - 1, j)] = ((b == 1) ? -x[IX(N - 2, j)] : x[IX(N - 2, j)]);
    }

    x[IX(0, 0)] = 0.5 * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, N - 1)] = 0.5 * (x[IX(1, N - 1)] + x[IX(0, N - 2)]);
    x[IX(N - 1, 0)] = 0.5 * (x[IX(N - 2, 0)] + x[IX(N - 1, 1)]);
    x[IX(N - 1, N - 1)] = 0.5 * (x[IX(N - 2, N - 1)] + x[IX(N - 1, N - 2)]);
}


