#include "SplineCore.h"

namespace NexusTween {
namespace Mathematics {

// Mengevaluasi nilai Y dari kurva Bezier kubik berdasarkan parameter t (0.0 hingga 1.0)
// Rumus dasar: Y(t) = 3(1-t)^2 t P1 + 3(1-t) t^2 P2 + t^3
// Kita memfaktorkannya (Horner's form) untuk meminimalkan instruksi perkalian.
float SplineCore::EvaluateCubicBezierEasing(float t, float p1y, float p2y) {
    // Pertahanan: Pastikan t dijepit (clamped) pada 0.0 - 1.0 murni menggunakan matematika,
    // BUKAN menggunakan if(t > 1.0) yang memicu branch prediction penalty.
    t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);

    // Kalkulasi basis polinomial yang diekspansi secara manual (Unrolled).
    // Compiler modern akan mengubah operasi ini menjadi satu instruksi FMA (Fused Multiply-Add).
    float t2 = t * t;
    float t3 = t2 * t;

    // Koefisien diekstraksi dari matriks basis Bezier:
    // C = 3 * p1y
    // B = 3 * (p2y - p1y) - C
    // A = 1 - C - B
    float c = 3.0f * p1y;
    float b = 3.0f * (p2y - p1y) - c;
    float a = 1.0f - c - b;

    // Evaluasi polinomial (At * t^3 + Bt * t^2 + Ct)
    return (a * t3) + (b * t2) + (c * t);
}

// Interpolasi Hermite Kubik untuk lintasan posisi (X, Y, Z).
// p0 = Titik awal, p1 = Titik akhir.
// m0 = Tangen/Kecepatan awal, m1 = Tangen/Kecepatan akhir.
Vec3 SplineCore::HermiteInterpolation(const Vec3& p0, const Vec3& p1, const Vec3& m0, const Vec3& m1, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    // Fungsi basis polinomial Hermite (Basis functions)
    // Sekali lagi, dihitung murni tanpa fungsi pustaka.
    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 = t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    // Komputasi akhir digabungkan. Jika di-compile di arsitektur ARM NEON atau x86 AVX,
    // kalkulasi x, y, dan z akan dieksekusi secara paralel pada hardware.
    return {
        h00 * p0.x + h10 * m0.x + h01 * p1.x + h11 * m1.x,
        h00 * p0.y + h10 * m0.y + h01 * p1.y + h11 * m1.y,
        h00 * p0.z + h10 * m0.z + h01 * p1.z + h11 * m1.z
    };
}

} // namespace Mathematics
} // namespace NexusTween
