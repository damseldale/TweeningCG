#pragma once
#include <cstdint>  // Untuk uint8_t, uintptr_t
#include <cstddef>  // Untuk size_t

namespace NexusTween {
namespace Memory {

// ArenaAllocator: Fondasi memori pra-bayar (Pre-allocated memory).
// Merupakan jantung dari sistem Data-Oriented kita.
class ArenaAllocator {
private:
    uint8_t* memoryBlock;     // Pointer mentah ke blok memori raksasa kita.
    size_t totalSize;         // Kapasitas maksimum arena.
    size_t currentOffset;     // Penanda (bookmark) lokasi memori kosong saat ini.

public:
    ArenaAllocator();
    ~ArenaAllocator();

    // Mencegah penyalinan (copying) yang tidak disengaja. Arena bersifat tunggal dan absolut.
    ArenaAllocator(const ArenaAllocator&) = delete;
    ArenaAllocator& operator=(const ArenaAllocator&) = delete;

    // Mengakuisisi blok memori dari OS HANYA SEKALI saat mesin menyala.
    void Initialize(size_t sizeInBytes);

    // [REVOLUSI]: Alokasi instan tanpa scanning.
    // Alignment penting agar data bersandar sejajar dengan jalur cache CPU (biasanya kelipatan 8 atau 16 byte).
    void* Allocate(size_t size, size_t alignment = 8);

    // [REVOLUSI]: Mengosongkan seluruh data puluhan ribu animasi secara instan (O(1)).
    void Reset();
    
    // Mengembalikan memori ke Sistem Operasi (hanya dipanggil saat aplikasi dimatikan).
    void Shutdown();
};

} // namespace Memory
} // namespace NexusTween
