#include <stdio.h>
#include <algorithm>

#include "particleSystem.hpp"

ParticleSystem::ParticleSystem(unsigned int nrParticles, unsigned int nrNewParticles) : nrParticles(nrParticles), nrNewParticles(nrNewParticles){
    releaseInterval = 1 / 60;
    for (unsigned int i = 0; i < nrParticles; ++i) {
        this->particles.emplace_back();
    }
}

bool sortDistanceToCamera(Particle p1, Particle p2){
    return p1.distanceToCamera > p2.distanceToCamera;
}

float randomNumber(int min, int max) {
    return ((float)rand() / (float)RAND_MAX) + (float)(min + rand() % (max - min));
    // return (rand() % 2000 - 1000) / 100.0f;
}

unsigned int lastUsedParticle = 0;
unsigned int ParticleSystem::firstUnusedParticle(){
    for (unsigned int i = lastUsedParticle; i < nrParticles; ++i) {
        if (particles[i].lifeTime <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (particles[i].lifeTime <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    lastUsedParticle = 0;
    return 0;
}
/*
float spread = 3.5;
void ParticleSystem::spawnParticle(Particle &particle, glm::vec3 position) {
    particle.lifeTime = 1.0f;
    glm::vec3 randomVec(randomNumber(-50, 50), randomNumber(-50, 50), randomNumber(-50, 50));
    particle.position = glm::vec3(position.x + (randomVec.x * spread), position.y, position.z + (randomVec.z * spread) - 10.0f);
    float velY = 12 - (abs(randomVec.x) + abs(randomVec.z))  * 9;

    particle.velocity = glm::vec3(0, velY, 0);
    particle.colour = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);
}*/


void ParticleSystem::spawnParticle(Particle &particle, glm::vec3 position) {
    particle.lifeTime = 1.0f;
    particle.position = position;
    particle.velocity = glm::vec3(randomNumber(-30.0f, 30.0f), randomNumber(-30.0f, 30.0f), randomNumber(-30.0f, 30.0f));
    particle.acceleration = glm::vec3(0.0f, 0.3f, 0.0f);
    particle.colour = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);
}

void ParticleSystem::update(float deltaTime, glm::vec3 position, glm::vec3 cameraPosition) {
    elapsedTime += deltaTime;
    if (elapsedTime > releaseInterval) {
        elapsedTime = 0;

        for (unsigned int i = 0; i < nrNewParticles; ++i) {
            unsigned int unusedParticle = firstUnusedParticle();
            spawnParticle(particles[unusedParticle], position);
        }
    }

    for (Particle &p : particles) {
        p.lifeTime -= deltaTime;
        if (p.lifeTime > 0.0f) {
            p.position += p.velocity * deltaTime;
            p.velocity += p.acceleration * deltaTime;
            p.distanceToCamera = glm::length(p.position - cameraPosition);
            float alpha = p.lifeTime / 2.0f;
            p.colour = glm::vec4(p.colour.r, p.colour.g * alpha, p.colour.b * alpha, alpha);
        }
    }

    // Sorts particles so that those furthest away are rendered first
    std::sort(&particles[0], &particles[nrParticles], sortDistanceToCamera);
}


