#include "TweenMutator.h"
#include "../Mathematics/SplineCore.h"
#include "../Data_Structure/EntityMasks.h" // Modul bit-masking kita

namespace NexusTween {
namespace Systems {

// Mutator berevolusi menerima referensi matriks bit (Masks)
void TweenMutator::Mutate(DataStructure::TransformSoA& soa, DataStructure::EntityMasks& masks, float deltaTime) {
    if (soa.activeCount == 0) return;

    // =====================================================================
    // PASS 1: MATEMATIKA VEKTOR (SIMD-FRIENDLY & BRANCHLESS)
    // =====================================================================
    for (size_t i = 0; i < soa.activeCount; ++i) {
        
        // [BIT-MASKING INJEKSI]: Menarik status dari matriks bit yang dipadatkan (O(1))
        uint32_t stateBit = masks.GetState(i);
        
        // Konversi bit ke multiplier (1.0f untuk Play, 0.0f untuk Pause)
        float timeMultiplier = static_cast<float>(stateBit);

        // Majukan Waktu Linear secara determenistik.
        // Jika animasi dipause (multiplier 0.0), waktu terhenti tanpa memicu cache-miss if/else.
        float rawT = soa.timeT[i] + (deltaTime * timeMultiplier);
        soa.timeT[i] = rawT;

        // Matematika murni untuk klem nilai (0.0 ke 1.0)
        float clampedT = rawT > 1.0f ? 1.0f : rawT;

        // [REVOLUSI FISIKA]: 
        // Menggunakan evaluasi Spring murni tanpa pustaka Math.
        // Asumsi nilai tension = 120.0f, friction = 12.0f (bisa ditarik dari array SoA nantinya)
        float springT = Mathematics::SplineCore::EvaluateSpring(clampedT, 120.0f, 12.0f);

        // Interpolasi Posisi ditarik ke dalam cache L1 secara berurutan
        soa.currentX[i] = Mathematics::SplineCore::Lerp(soa.startX[i], soa.targetX[i], springT);
        soa.currentY[i] = Mathematics::SplineCore::Lerp(soa.startY[i], soa.targetY[i], springT);
        soa.currentZ[i] = Mathematics::SplineCore::Lerp(soa.startZ[i], soa.targetZ[i], springT);
    }

    // =====================================================================
    // PASS 2: THE REAPER (PEMUSNAHAN O(1) MUTLAK)
    // =====================================================================
    size_t currentIndex = 0;
    while (currentIndex < soa.activeCount) {
        if (soa.timeT[currentIndex] >= 1.0f) {
            
            // Hancurkan data animasi dari RAM
            soa.RemoveTween(currentIndex);
            
            // [SINKRONISASI BIT]: Karena entitas terakhir dipindahkan ke currentIndex 
            // (Swap and Pop), kita harus menyalin status bit-nya juga agar tidak terjadi korupsi status.
            size_t lastIndex = soa.activeCount; // Nilai sudah dikurangi oleh RemoveTween()
            
            if (masks.GetState(lastIndex) == 1) {
                masks.Play(currentIndex);
            } else {
                masks.Pause(currentIndex);
            }
            
        } else {
            currentIndex++;
        }
    }
}

} // namespace Systems
} // namespace NexusTween
