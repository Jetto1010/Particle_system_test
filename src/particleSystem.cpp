#include <stdio.h>

#include "particleSystem.hpp"

ParticleSystem::ParticleSystem(unsigned int nrParticles) : nrParticles(nrParticles){
    for (unsigned int i = 0; i < nrParticles; i++) {
        this->particles.emplace_back();
    }
};

void ParticleSystem::spawnParticle(Particle &particle, glm::vec3 position, glm::vec3 offset) {
    float random = ((rand() % 100) - 50) / 10.0f;
    // float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.position = position + random + offset;
    particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    particle.lifeTime = 1.0f;
    particle.velocity = glm::vec3(-50, 50, 50);
}

void ParticleSystem::update(float deltaTime, glm::vec3 position, glm::vec3 offset) {
    // update all particles
    for (unsigned int i = 0; i < nrParticles; ++i) {
        Particle &p = particles[i];
        p.lifeTime -= deltaTime; // reduce life
        if (p.lifeTime > 0.0f) {	// particle is alive, thus update
            p.position += p.velocity * deltaTime;
            // p.color.a -= deltaTime * 2.5f;
        } else {
            spawnParticle(particles[i], position, offset);
        }
    }
}


