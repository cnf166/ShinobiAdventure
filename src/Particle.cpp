#include "Particle.h"

Particle::Particle(int x, int y, SDL_Renderer* renderer) {
    this->x = static_cast<float>(x);
    this->y = static_cast<float>(y);
    this->vx = (rand() % 200 - 100) / 100.0f; // Vận tốc ngang ngẫu nhiên (-1.0 đến 1.0)
    this->vy = (rand() % 100 + 50) / 100.0f;  // Vận tốc dọc (0.5 đến 1.5)
    this->maxLifetime = 5.0f;                  // Sống 5 giây
    this->lifetime = maxLifetime;

    // Tải hình ảnh bông tuyết (hoặc vẽ chấm trắng nếu không có hình)
    SDL_Surface* surface = IMG_Load("img//snowflake.png");
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        rect.w = 16; // Kích thước bông tuyết
        rect.h = 16;
        SDL_FreeSurface(surface);
    } else {
        texture = nullptr; // Nếu không tải được hình, sẽ vẽ chấm trắng
        rect.w = 4;
        rect.h = 4;
    }
}

Particle::~Particle() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Particle::Update(float deltaTime) {
    x += vx * deltaTime; // Cập nhật vị trí theo vận tốc
    y += vy * deltaTime;
    lifetime -= deltaTime; // Giảm thời gian sống

    // Cập nhật vị trí render
    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);

    // Nếu hạt ra ngoài màn hình hoặc hết thời gian sống, đánh dấu là chết
    if (y > SCREEN_HEIGHT || lifetime <= 0) {
        lifetime = 0;
    }
}

void Particle::Render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    } else {
        // Nếu không có hình ảnh, vẽ chấm trắng
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

ParticleEngine::ParticleEngine(SDL_Renderer* renderer, int maxParticles) {
    this->renderer = renderer;
    this->maxParticles = maxParticles;
    this->lastSpawnTime = 0;

    // Tải texture bông tuyết chung (nếu có)
    SDL_Surface* surface = IMG_Load("img//snowflake.png");
    if (surface) {
        snowflakeTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        snowflakeTexture = nullptr;
    }
}

ParticleEngine::~ParticleEngine() {
    for (Particle* particle : particles) {
        delete particle;
    }
    particles.clear();
    if (snowflakeTexture) {
        SDL_DestroyTexture(snowflakeTexture);
    }
}

void ParticleEngine::Update(float deltaTime) {
    // Xóa các hạt đã chết
    for (size_t i = 0; i < particles.size(); ) {
        particles[i]->Update(deltaTime);
        if (particles[i]->IsDead()) {
            delete particles[i];
            particles.erase(particles.begin() + i);
        } else {
            ++i;
        }
    }

    // Sinh hạt mới nếu chưa đạt tối đa
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastSpawnTime >= spawnInterval && particles.size() < static_cast<size_t>(maxParticles)) {
        SpawnParticle();
        lastSpawnTime = currentTime;
    }
}

void ParticleEngine::Render(SDL_Renderer* renderer) {
    for (Particle* particle : particles) {
        particle->Render(renderer);
    }
}

void ParticleEngine::SpawnParticle() {
    int x = rand() % SCREEN_WIDTH; // Vị trí x ngẫu nhiên trên màn hình
    int y = -16;                   // Bắt đầu từ trên màn hình
    Particle* particle = new Particle(x, y, renderer);
    particles.push_back(particle);
}
