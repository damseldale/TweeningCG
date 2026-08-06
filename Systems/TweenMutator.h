#pragma once
#include "../Data_Structure/TransformSoA.h"

namespace NexusTween {
namespace Systems {

// [REVOLUSI EKSEKUSI]: TweenMutator bukanlah sebuah objek. 
// Ini adalah pipa pemrosesan (pipeline) murni yang menelan memori SoA 
// dan memuntahkan koordinat baru.
class TweenMutator {
public:
    // Menolak instansiasi. Ini adalah modul fungsional statis murni.
    TweenMutator() = delete;

    // Memperbarui puluhan ribu animasi dalam satu siklus L1 Cache.
    // Menerima referensi ke TransformSoA dan delta waktu global.
    static void Mutate(DataStructure::TransformSoA& soa, float deltaTime);
};

} // namespace Systems
} // namespace NexusTween
