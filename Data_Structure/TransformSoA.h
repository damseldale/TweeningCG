#pragma once
#include "../Memory/ArenaAllocator.h"
#include <cstdint>

namespace NexusTween {
namespace DataStructure {

// [REVOLUSI KELAS]: TransformSoA bukan menyimpan "objek", melainkan "kolom data".
// Ini adalah implementasi murni Data-Oriented Design (DOD).
class TransformSoA {
public:
    // Pointers ke blok memori yang dialokasikan oleh Arena.
    // Kita memisahkan X, Y, Z untuk Current, Start, dan Target.
    // Membaca X tidak akan mencemari cache dengan Y atau Z.
    
    // Status Saat Ini (Current State)
    float* currentX;
    float* currentY;
    float* currentZ;

    // Status Awal (Start State)
    float* startX;
    float* startY;
    float* startZ;

    // Status Target Animasi (Target State)
    float* targetX;
    float* targetY;
    float* targetZ;

    // Nilai interpolasi per entitas (0.0f hingga 1.0f)
    float* timeT;

    size_t capacity;     // Kapasitas maksimum
    size_t activeCount;  // Jumlah entitas yang sedang dianimasikan (aktif) saat ini

    TransformSoA() : capacity(0), activeCount(0) {
        // Pointer disetel null secara default
        currentX = currentY = currentZ = nullptr;
        startX = startY = startZ = nullptr;
        targetX = targetY = targetZ = nullptr;
        timeT = nullptr;
    }

    // [INISIALISASI REVOLUSIONER]
    // Merampas blok memori dari Arena secara berurutan dan disejajarkan ke 32-byte 
    // agar bersahabat dengan instruksi CPU AVX/SIMD tingkat lanjut.
    void Initialize(Memory::ArenaAllocator& arena, size_t maxEntities) {
        capacity = maxEntities;
        activeCount = 0;

        size_t bytesPerArray = capacity * sizeof(float);
        size_t alignment = 32; // Penyejajaran memori ekstrem untuk Vectorization

        // Alokasi memori beruntun, nol fragmentasi.
        currentX = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));
        currentY = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));
        currentZ = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));

        startX = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));
        startY = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));
        startZ = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));

        targetX = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));
        targetY = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));
        targetZ = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));

        timeT = static_cast<float*>(arena.Allocate(bytesPerArray, alignment));
    }

    // Mendaftarkan entitas baru untuk dianimasikan (O(1) execution time)
    // Mengembalikan ID (Index) dari entitas tersebut.
    size_t AddTween(float sX, float sY, float sZ, float tX, float tY, float tZ) {
        if (activeCount >= capacity) {
            // Dalam sistem tingkat elit, engine tidak pernah crash. 
            // Kita bisa memberikan fallback atau mengabaikannya.
            return static_cast<size_t>(-1); 
        }

        size_t index = activeCount;
        
        currentX[index] = startX[index] = sX;
        currentY[index] = startY[index] = sY;
        currentZ[index] = startZ[index] = sZ;

        targetX[index] = tX;
        targetY[index] = tY;
        targetZ[index] = tZ;

        timeT[index] = 0.0f; // Waktu selalu mulai dari 0.0

        activeCount++;
        return index;
    }

    // [MEKANISME PENGHANCURAN REVOLUSIONER]: Swap-And-Pop
    // Saat animasi selesai, kita TIDAK mengosongkan dan meninggalkan "lubang" memori 
    // yang akan menghancurkan prediksi hardware (cache miss).
    // Kita menukar data entitas terakhir yang aktif ke posisi yang dihapus, 
    // lalu mengurangi activeCount. 
    // Eksekusi murni O(1) tanpa memicu Garbage Collector.
    void RemoveTween(size_t indexToRemove) {
        if (indexToRemove >= activeCount) return;

        size_t lastIndex = activeCount - 1;

        // Pindahkan data dari entitas terakhir ke indeks yang akan dihapus
        currentX[indexToRemove] = currentX[lastIndex];
        currentY[indexToRemove] = currentY[lastIndex];
        currentZ[indexToRemove] = currentZ[lastIndex];

        startX[indexToRemove] = startX[lastIndex];
        startY[indexToRemove] = startY[lastIndex];
        startZ[indexToRemove] = startZ[lastIndex];

        targetX[indexToRemove] = targetX[lastIndex];
        targetY[indexToRemove] = targetY[lastIndex];
        targetZ[indexToRemove] = targetZ[lastIndex];

        timeT[indexToRemove] = timeT[lastIndex];

        // Kurangi jumlah aktif
        activeCount--;
    }
};

} // namespace DataStructure
} // namespace NexusTween
