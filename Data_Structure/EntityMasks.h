#pragma once
#include "../Memory/ArenaAllocator.h"
#include <cstdint>
#include <cstddef>

namespace NexusTween {
namespace DataStructure {

// [REVOLUSI STATUS]: EntityMasks tidak menyimpan boolean.
// Menyimpan status puluhan ribu entitas dalam matriks bit (Bit-Matrix) yang sangat padat.
// 1 Blok uint32_t mengendalikan 32 entitas sekaligus.
class EntityMasks {
public:
    uint32_t* activeBits; 
    size_t capacity;

    EntityMasks() : activeBits(nullptr), capacity(0) {}

    // Inisialisasi O(1) dari ArenaAllocator.
    void Initialize(Memory::ArenaAllocator& arena, size_t maxEntities) {
        capacity = maxEntities;
        
        // Membagi jumlah entitas dengan 32 untuk mendapatkan jumlah blok uint32_t yang dibutuhkan.
        // Penambahan 31 menjamin pembulatan ke atas (Ceiling) murni tanpa float math.
        size_t blockCount = (maxEntities + 31) / 32;
        
        activeBits = static_cast<uint32_t*>(arena.Allocate(blockCount * sizeof(uint32_t), 32));
        
        // Secara default, biarkan memori kosong (Semua bit bernilai 0 / tidak aktif)
        // Saat Tween ditambahkan, kita akan menyalakan bit-nya.
    }

    // [OPERASI BITWISE INSTAN]: O(1), ~2 CPU Cycles
    void Play(size_t entityIndex) {
        // entityIndex >> 5 setara dengan entityIndex / 32 (Mencari blok)
        // entityIndex & 31 setara dengan entityIndex % 32 (Mencari posisi bit di dalam blok)
        activeBits[entityIndex >> 5] |= (1U << (entityIndex & 31));
    }

    void Pause(size_t entityIndex) {
        // Mematikan bit menggunakan operator bitwise AND dengan bit terbalik (NOT)
        activeBits[entityIndex >> 5] &= ~(1U << (entityIndex & 31));
    }

    void Toggle(size_t entityIndex) {
        // Membalikkan status menggunakan XOR. Jika 1 jadi 0, jika 0 jadi 1.
        activeBits[entityIndex >> 5] ^= (1U << (entityIndex & 31));
    }

    // Menarik status murni tanpa percabangan. Mengembalikan 0 atau 1.
    inline uint32_t GetState(size_t entityIndex) const {
        return (activeBits[entityIndex >> 5] >> (entityIndex & 31)) & 1U;
    }
};

} // namespace DataStructure
} // namespace NexusTween
