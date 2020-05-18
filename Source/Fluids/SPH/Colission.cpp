// Fill out your copyright notice in the Description page of Project Settings.
#include "Colission.h"






// (Formula 4.36)
static FVector calculateContactPoint(FVector particlePosition, FVector differenceParticleNeighbour)
{
    double particleDistance = differenceParticleNeighbour.Size();
    /*GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Yellow, "Initial Position: " + particlePosition.ToString()
        + "Diference Vector: " + differenceParticleNeighbour.ToString()
        + "Difference Vector Normalized: " + (differenceParticleNeighbour / particleDistance).ToString()
        + " Particle Distance: " + FString::SanitizeFloat(particleDistance)
        + " factor: " + FString::SanitizeFloat(particleDistance - ParticleRadius)
        + " End Vector: " + ((differenceParticleNeighbour / particleDistance) * (ParticleRadius * 2 - particleDistance)).ToString()
        + "End Position: " + (particlePosition + (differenceParticleNeighbour / particleDistance) * (ParticleRadius * 2 - particleDistance)).ToString());  */                   
    return particlePosition + (differenceParticleNeighbour / particleDistance) * (2*ParticleRadius - particleDistance);
}

// (Formula 4.38)
static FVector calculateSurfaceNormal(FVector differenceParticleNeighbour)
{
    double particleDistance = differenceParticleNeighbour.Size();
    return -differenceParticleNeighbour / particleDistance;
}

// (Formula 4.56)
static FVector calculateVelocity(FVector particleVelocity, FVector differenceParticleNeighbour)
{
    double scalarProduct = particleVelocity.X * differenceParticleNeighbour.X +
        particleVelocity.Y * differenceParticleNeighbour.Y +
        particleVelocity.Z * differenceParticleNeighbour.Z;

    return differenceParticleNeighbour * scalarProduct;
}

void Colission::detectCollisions(UBillboardParticles& particles, const float Size) {
    for (auto i = 0; i < particles.particles.Num(); i++) {
        for (auto j = 0; j < particles.particles[i].neighbours.Num(); j++) {
            int neighbourposition = particles.particles[i].neighbours[j];
            FVector differenceParticleNeighbour = particles.particles[i].pos - particles.particles[neighbourposition].pos;
            if (differenceParticleNeighbour == FVector::ZeroVector) continue;
            if (differenceParticleNeighbour.Size() < 2 * ParticleRadius) {
                //GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, "Particle " + FString::FromInt(i) + " at position " + particles.particles[i].pos.ToString() + " is colliding with particle " + FString::FromInt(j) + " at position " + particles.particles[j].pos.ToString() + " resulting in dPN " + differenceParticleNeighbour.ToString());
                const FVector surfaceNormal = calculateSurfaceNormal(differenceParticleNeighbour);

                particles.particles[i].pos = calculateContactPoint(particles.particles[i].pos, differenceParticleNeighbour);

                
                
                particles.particles[i].vel += calculateVelocity(particles.particles[neighbourposition].vel - particles.particles[i].vel, surfaceNormal);
               //particles.particles[i].vel *= 0.5;
                particles.particles[neighbourposition].vel += calculateVelocity(particles.particles[i].vel - particles.particles[neighbourposition].vel, surfaceNormal);
                //particles.particles[i].vel *= 0.5;

                

                

                //particles.particles[j].vel -= calculateVelocity(particles.particles[i].vel - particles.particles[j].vel, surfaceNormal) * (-CollisionVelocityMultiplier);
            }
        }

        //Collision with bounds
        //GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, "YOSTART");
        if (particles.particles[i].pos.X > Size - ParticleRadius) {
            particles.particles[i].pos.X = Size - ParticleRadius;
            particles.particles[i].vel.X *= CollisionVelocityMultiplier;
        }

        if (particles.particles[i].pos.X < ParticleRadius) {
            particles.particles[i].pos.X = ParticleRadius;
            particles.particles[i].vel.X *= CollisionVelocityMultiplier;
        }

        if (particles.particles[i].pos.Y > Size - ParticleRadius) {
            particles.particles[i].pos.Y = Size - ParticleRadius;
            particles.particles[i].vel.Y *= CollisionVelocityMultiplier;
        }

        if (particles.particles[i].pos.Y < ParticleRadius) {
            particles.particles[i].pos.Y = ParticleRadius;
            particles.particles[i].vel.Y *= CollisionVelocityMultiplier;
        }

        if (particles.particles[i].pos.Z > Size - ParticleRadius) {
            particles.particles[i].pos.Z = Size - ParticleRadius;
            particles.particles[i].vel.Z *= CollisionVelocityMultiplier;
        }

        if (particles.particles[i].pos.Z < ParticleRadius) {
            particles.particles[i].pos.Z = ParticleRadius;
            particles.particles[i].vel.Z *= CollisionVelocityMultiplier;
        }
    }
}
