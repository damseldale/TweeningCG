#pragma once
#include "../Memory/ArenaAllocator.h"
#include "../Data_Structure/TransformSoA.h"
#include "../Data_Structure/EntityMasks.h"
#include "../Systems/TweenMutator.h"
#include <cstdint>

namespace NexusTween {
namespace API {

struct TweenHandle {
    uint32_t id;
};

// [REVOLUSI ANTARMUKA]: Pintu gerbang utama. Pengguna tidak pernah melihat 
// pointer memori, hanya memegang 'Handle' (ID) ringan.
class NexusEngine {
private:
    Memory::ArenaAllocator arena;
    DataStructure::TransformSoA transforms;
    DataStructure::EntityMasks masks; // Injeksi Bit-Matrix kita
    bool isBooted;

public:
    NexusEngine() : isBooted(false) {}

    // IGNITION
    void Boot(size_t maxEntities) {
        if (isBooted) return;

        // Kalkulasi kasar memori: 10 Array SoA + Array Bit-Masks
        size_t bytesNeeded = (10 * maxEntities * sizeof(float)) + 
                             ((maxEntities + 31) / 32 * sizeof(uint32_t)) + 1024; 

        arena.Initialize(bytesNeeded);
        transforms.Initialize(arena, maxEntities);
        masks.Initialize(arena, maxEntities); // Inisialisasi Masks
        
        isBooted = true;
    }

    // INJECTION
    TweenHandle Animate(float startX, float startY, float startZ, 
                        float targetX, float targetY, float targetZ) {
        
        if (!isBooted) return { static_cast<uint32_t>(-1) };

        size_t internalIndex = transforms.AddTween(startX, startY, startZ, 
                                                   targetX, targetY, targetZ);
        
        if (internalIndex != static_cast<size_t>(-1)) {
            masks.Play(internalIndex); // Nyalakan bit agar animasi berjalan
        }
        
        return { static_cast<uint32_t>(internalIndex) };
    }

    // [KENDALI INSTAN O(1)]: Mengubah bit tunggal tanpa percabangan
    void Pause(TweenHandle handle) {
        if (!isBooted || handle.id >= transforms.activeCount) return;
        masks.Pause(handle.id);
    }

    void Resume(TweenHandle handle) {
        if (!isBooted || handle.id >= transforms.activeCount) return;
        masks.Play(handle.id);
    }

    // EXECUTION
    void Tick(float deltaTime) {
        if (!isBooted) return;
        // Mutator kini menelan matriks data dan matriks bit secara bersamaan
        Systems::TweenMutator::Mutate(transforms, masks, deltaTime);
    }

    // DATA RETRIEVAL
    void GetCurrentPosition(TweenHandle handle, float& outX, float& outY, float& outZ) const {
        if (handle.id >= transforms.activeCount) {
            outX = outY = outZ = 0.0f; 
            return;
        }

        outX = transforms.currentX[handle.id];
        outY = transforms.currentY[handle.id];
        outZ = transforms.currentZ[handle.id];
    }

    // TERMINATION
    void Shutdown() {
        if (!isBooted) return;
        arena.Shutdown();
        isBooted = false;
    }
};

} // namespace API
} // namespace NexusTween
