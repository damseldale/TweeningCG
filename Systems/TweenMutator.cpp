
#include "TweenMutator.h"
#include "../Mathematics/SplineCore.h"

namespace NexusTween {
namespace Systems {

void TweenMutator::Mutate(DataStructure::TransformSoA& soa, float deltaTime) {
    if (soa.activeCount == 0) return; // Zero-cost idle jika tidak ada yang perlu dianimasikan.

    // =====================================================================
    // PASS 1: MATEMATIKA VEKTOR (SIMD-FRIENDLY PIPELINE)
    // =====================================================================
    // Perulangan ini 100% deterministik dan bebas percabangan (branchless).
    // Compiler akan membongkar perulangan ini (loop unrolling) dan 
    // memproses X, Y, Z secara simultan.
    for (size_t i = 0; i < soa.activeCount; ++i) {
        // 1. Majukan Waktu Linear
        // (Asumsi durasi universal 1.0 demi kesederhanaan. Dalam praktiknya, 
        // kita bisa menambahkan array soa.durationT[i]).
        float rawT = soa.timeT[i] + deltaTime;
        soa.timeT[i] = rawT;

        // 2. Klem Nilai Waktu (Clamp) tanpa instruksi If
        // Matematika murni untuk membatasi t pada maksimum 1.0f.
        float clampedT = rawT > 1.0f ? 1.0f : rawT;

        // 3. Modulasi Kurva (Easing)
        // Transformasi waktu linear menjadi kurva transisi mulus menggunakan 
        // Evaluasi Bezier Kubik yang kita rancang sebelumnya.
        float easedT = Mathematics::SplineCore::EvaluateCubicBezierEasing(clampedT, 0.0f, 1.0f);

        // 4. Interpolasi Posisi (Lerp)
        // Menarik data dari Array Start, memproyeksikannya ke Array Target.
        // Karena data sejajar di RAM, CPU prefetcher akan menarik blok memori berikutnya 
        // bahkan sebelum perhitungan ini selesai.
        soa.currentX[i] = Mathematics::SplineCore::Lerp(soa.startX[i], soa.targetX[i], easedT);
        soa.currentY[i] = Mathematics::SplineCore::Lerp(soa.startY[i], soa.targetY[i], easedT);
        soa.currentZ[i] = Mathematics::SplineCore::Lerp(soa.startZ[i], soa.targetZ[i], easedT);
    }

    // =====================================================================
    // PASS 2: PEMUSNAHAN ENTITAS (THE REAPER)
    // =====================================================================
    // Memindai memori untuk animasi yang telah selesai (t >= 1.0).
    // Eksekusi ini terpisah agar tidak mengganggu kecepatan kalkulasi matematika di Pass 1.
    size_t currentIndex = 0;
    while (currentIndex < soa.activeCount) {
        if (soa.timeT[currentIndex] >= 1.0f) {
            // Mekanisme Swap-and-Pop: O(1) Instan.
            soa.RemoveTween(currentIndex);
            
            // PENTING: Kita tidak menambah (increment) currentIndex di sini.
            // Mengapa? Karena entitas dari akhir array baru saja ditukar (swap) 
            // ke posisi currentIndex. Kita harus memeriksa entitas baru ini 
            // di putaran iterasi berikutnya.
        } else {
            // Jika belum selesai, lanjut ke entitas berikutnya.
            currentIndex++;
        }
    }
}

} // namespace Systems
} // namespace NexusTween
