//
// Created by Joerund on 03.04.2023.
//

#include "particleSystem.hpp"

ParticleSystem::ParticleSystem(unsigned int nrParticles) : nrParticles(nrParticles){
    for (unsigned int i = 0; i < nrParticles; i++) {
        this->particles.emplace_back();
    }
};

unsigned int lastUsedParticle = 0;
unsigned int ParticleSystem::findUnusedParticle() {
    // search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->nrParticles; ++i) {
        if (particles[i].lifeTime <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (particles[i].lifeTime <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // override first particle if all others are alive
    lastUsedParticle = 0;
    return 0;
}

void ParticleSystem::respawnParticle(Particle &particle, glm::vec3 position, glm::vec3 offset) {
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.position = position + random + offset;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.lifeTime = 1.0f;
    // particle.velocity = object.Velocity * 0.1f;
}

void ParticleSystem::update(float deltaTime, glm::vec3 position, unsigned int newParticles, glm::vec3 offset) {
    // add new particles
    for (unsigned int i = 0; i < newParticles; ++i) {
        unsigned int unusedParticle = findUnusedParticle();
        respawnParticle(particles[unusedParticle], position, offset);
    }
    // update all particles
    for (unsigned int i = 0; i < nrParticles; ++i) {
        Particle &p = particles[i];
        p.lifeTime -= deltaTime; // reduce life
        if (p.lifeTime > 0.0f)
        {	// particle is alive, thus update
            p.position -= p.velocity * deltaTime;
            p.color.a -= deltaTime * 2.5f;
        }
    }
}


