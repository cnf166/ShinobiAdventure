#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "CommonFunc.h"
#include <vector>
#include <cstdlib>

class Particle {
public:
    Particle(int x, int y, SDL_Renderer* renderer);
    ~Particle();
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    bool IsDead() const { return lifetime <= 0; }

private:
    float x, y;          // Vị trí hạt
    float vx, vy;        // Vận tốc (ngang và dọc)
    float lifetime;      // Thời gian sống (giây)
    float maxLifetime;   // Thời gian sống tối đa
    SDL_Texture* texture; // Hình ảnh bông tuyết
    SDL_Rect rect;       // Hình chữ nhật để render
};

class ParticleEngine {
public:
    ParticleEngine(SDL_Renderer* renderer, int maxParticles);
    ~ParticleEngine();
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    void SpawnParticle();

private:
    SDL_Renderer* renderer;
    std::vector<Particle*> particles;
    int maxParticles;
    SDL_Texture* snowflakeTexture; // Texture chung cho bông tuyết
    Uint32 lastSpawnTime;          // Thời gian spawn hạt cuối cùng
    const Uint32 spawnInterval = 50; // Khoảng thời gian giữa các lần spawn (ms)
};

#endif // PARTICLE_H_
