#include "ArenaAllocator.h"
#include <cstdlib> // Hanya diizinkan untuk std::malloc / std::free awal
#include <cassert> // Untuk pertahanan sistem (system defense)

namespace NexusTween {
namespace Memory {

ArenaAllocator::ArenaAllocator() 
    : memoryBlock(nullptr), totalSize(0), currentOffset(0) {}

ArenaAllocator::~ArenaAllocator() {
    Shutdown();
}

void ArenaAllocator::Initialize(size_t sizeInBytes) {
    // Kita melakukan 'dosa' memanggil heap-allocation HANYA SEKALI di sini.
    // Setelah mesin NexusTween berjalan, kita menolak intervensi Sistem Operasi.
    memoryBlock = static_cast<uint8_t*>(std::malloc(sizeInBytes));
    assert(memoryBlock != nullptr && "FATAL: Gagal merampas memori dari OS!");
    
    totalSize = sizeInBytes;
    currentOffset = 0;
}

void* ArenaAllocator::Allocate(size_t size, size_t alignment) {
    // Memastikan alignment selalu berupa pangkat dari 2 (2, 4, 8, 16, 32...)
    // Ini krusial untuk komputasi SIMD (Vectorization) nantinya.
    assert((alignment & (alignment - 1)) == 0 && "Alignment harus kelipatan pangkat 2!");

    // 1. Dapatkan representasi numerik dari alamat memori mentah saat ini.
    uintptr_t currentPtr = reinterpret_cast<uintptr_t>(memoryBlock + currentOffset);
    
    // 2. Kalkulasi Padding (Ruang kosong agar data sejajar di memori).
    // Rumus Bitwise: (alignment - (currentPtr & (alignment - 1))) & (alignment - 1)
    // Pendekatan ini tidak menggunakan percabangan (if/else) atau operasi modulo, 
    // sehingga CPU pipeline tidak akan pernah tersendat.
    size_t padding = (alignment - (currentPtr & (alignment - 1))) & (alignment - 1);
    
    // 3. Proyeksi offset memori berikutnya.
    size_t nextOffset = currentOffset + padding + size;
    
    // 4. Pertahanan mutlak: Pastikan kita tidak melampaui kapasitas arena.
    assert(nextOffset <= totalSize && "Arena Allocator: Memori Overload! Kapasitas terlampaui.");
    
    // 5. Tandai titik awal alokasi data.
    size_t alignedOffset = currentOffset + padding;
    
    // 6. Bump (geser) pointer ke depan. (O(1) operation, ~3 CPU cycles).
    currentOffset = nextOffset;
    
    // 7. Kembalikan penunjuk memori yang siap digunakan tanpa inisialisasi ulang yang lambat.
    return memoryBlock + alignedOffset;
}

void ArenaAllocator::Reset() {
    // De-alokasi konvensional akan menghancurkan (destruct) objek satu per satu.
    // Kita menolak metode itu. Kita membuang ribuan data animasi dengan
    // sekadar menggeser kembali penanda ke angka NOL.
    // Data lama akan tertimpa secara otomatis. Sangat radikal, sangat cepat.
    currentOffset = 0;
}

void ArenaAllocator::Shutdown() {
    if (memoryBlock != nullptr) {
        std::free(memoryBlock);
        memoryBlock = nullptr;
    }
    totalSize = 0;
    currentOffset = 0;
}

} // namespace Memory
} // namespace NexusTween
