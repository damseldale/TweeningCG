#pragma once
#include "../Memory/ArenaAllocator.h"
#include "../Data_Structure/TransformSoA.h"
#include "../Systems/TweenMutator.h"
#include <cstdint>

namespace NexusTween {
namespace API {

// Handle generik yang sangat ringan (4 byte).
// Pengguna hanya akan memegang struct ini, BUKAN pointer ke data.
struct TweenHandle {
    uint32_t id;
    // Di masa depan, kita bisa menambahkan bit generasi (generation bit) 
    // di sini untuk mencegah bug penggunaan ulang ID.
};

// [REVOLUSI ANTARMUKA]: NexusEngine menyembunyikan brutalitas Data-Oriented Design 
// di balik fasad yang elegan, deterministik, dan bebas alokasi dinamis.
class NexusEngine {
private:
    Memory::ArenaAllocator arena;
    DataStructure::TransformSoA transforms;
    bool isBooted;

public:
    NexusEngine() : isBooted(false) {}

    // 1. IGNITION (Menyalakan Mesin)
    // Pengguna harus secara eksplisit mendeklarasikan batas maksimal (Max Capacity).
    // Ini memaksa disiplin memori dan mencegah kebocoran secara arsitektural.
    void Boot(size_t maxEntities) {
        if (isBooted) return;

        // Kalkulasi kasar memori yang dibutuhkan:
        // 10 Array (X,Y,Z untuk Current, Start, Target + 1 Array Waktu)
        // Setiap array berukuran = maxEntities * 4 byte (ukuran float)
        size_t bytesNeeded = 10 * maxEntities * sizeof(float);
        
        // Tambahkan padding ekstra untuk keamanan alignment 32-byte
        bytesNeeded += 1024; 

        arena.Initialize(bytesNeeded);
        transforms.Initialize(arena, maxEntities);
        isBooted = true;
    }

    // 2. INJECTION (Menambahkan Animasi)
    // Antarmuka yang mulus. Mengembalikan 'Handle', bukan referensi objek.
    TweenHandle Animate(float startX, float startY, float startZ, 
                        float targetX, float targetY, float targetZ) {
        
        if (!isBooted) return { static_cast<uint32_t>(-1) };

        size_t internalIndex = transforms.AddTween(startX, startY, startZ, 
                                                   targetX, targetY, targetZ);
        
        return { static_cast<uint32_t>(internalIndex) };
    }

    // 3. EXECUTION (Siklus Detak Jam / Frame Loop)
    // Dipanggil setiap frame (misal di dalam Unity, Unreal, atau custom game loop).
    // Meneruskan waktu linear ke jantung mutator.
    void Tick(float deltaTime) {
        if (!isBooted) return;
        Systems::TweenMutator::Mutate(transforms, deltaTime);
    }

    // 4. DATA RETRIEVAL (Membaca Hasil)
    // Pengguna menggunakan Handle untuk menarik data dari SoA tanpa menyentuh strukturnya langsung.
    // Eksekusi O(1) mutlak melalui pembacaan array.
    void GetCurrentPosition(TweenHandle handle, float& outX, float& outY, float& outZ) const {
        if (handle.id >= transforms.activeCount) {
            // Handle kedaluwarsa atau salah, berikan posisi 0 sebagai pertahanan (fail-safe)
            outX = outY = outZ = 0.0f; 
            return;
        }

        outX = transforms.currentX[handle.id];
        outY = transforms.currentY[handle.id];
        outZ = transforms.currentZ[handle.id];
    }

    // 5. TERMINATION (Mematikan Mesin)
    void Shutdown() {
        if (!isBooted) return;
        arena.Shutdown();
        isBooted = false;
    }
};

} // namespace API
} // namespace NexusTween
