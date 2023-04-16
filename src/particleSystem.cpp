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

void ParticleSystem::spawnParticle(Particle &particle, glm::vec3 position) {
    particle.lifeTime = 1.0f;
    particle.colour = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);

    // Particles are distributed in a circle
    glm::vec3 randomPos(randomNumber(-5, 5), 0, randomNumber(-5, 5));
    while (length(randomPos) > 5) {
        randomPos = glm::vec3(randomNumber(-5, 5), 0, randomNumber(-5, 5));
    }
    particle.position = position + randomPos;

    // Make particles drift towards the center
    glm::vec2 xRange(0,0);
    glm::vec2 zRange(0,0);
    if (randomPos.x < 0) {
        xRange = glm::vec2(0,5);
    } else {
        xRange = glm::vec2(-5,0);
    }
    if (randomPos.z < 0) {
        zRange = glm::vec2(0,5);
    } else {
        zRange = glm::vec2(-5,0);
    }
    glm::vec3 randomVel(randomNumber(xRange.x, xRange.y), 0, randomNumber(zRange.x, zRange.y));
    while (length(randomVel) > 5) {
        randomVel = glm::vec3(randomNumber(xRange.x, xRange.y), 0, randomNumber(zRange.x, zRange.y));
    }
    // The closer to the center the higher
    float yVel = randomNumber(5, 15) / (1 + length(randomPos));
    particle.velocity = glm::vec3(randomVel.x, yVel, randomVel.z);

    particle.acceleration = glm::vec3(-randomVel.x, 0,-randomVel.z);
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
            float alpha = p.lifeTime / 2;
            p.colour = glm::vec4(0.75,1-(p.position.y - 12)/3,0,0.8);
            if (p.position.y > 17) {
                p.colour = glm::vec4(0.5, 0.5, 0.5, 0.8);
            }


        }
    }

    // Sorts particles so that those furthest away are rendered first
    std::sort(particles.begin(), particles.end(), sortDistanceToCamera);
}


