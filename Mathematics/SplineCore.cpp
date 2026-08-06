#include "SplineCore.h"

namespace NexusTween {
namespace Mathematics {

float SplineCore::EvaluateCubicBezierEasing(float t, float p1y, float p2y) {
    t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
    float t2 = t * t;
    float t3 = t2 * t;

    float c = 3.0f * p1y;
    float b = 3.0f * (p2y - p1y) - c;
    float a = 1.0f - c - b;

    return (a * t3) + (b * t2) + (c * t);
}

Vec3 SplineCore::HermiteInterpolation(const Vec3& p0, const Vec3& p1, const Vec3& m0, const Vec3& m1, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 = t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    return {
        h00 * p0.x + h10 * m0.x + h01 * p1.x + h11 * m1.x,
        h00 * p0.y + h10 * m0.y + h01 * p1.y + h11 * m1.y,
        h00 * p0.z + h10 * m0.z + h01 * p1.z + h11 * m1.z
    };
}

// [REVOLUSI FISIKA]: Kalkulasi Harmonic Oscillator murni tanpa library standar.
float SplineCore::EvaluateSpring(float t, float tension, float friction) {
    // Klem t jika mencapai akhir animasi untuk mencegah osilasi tak terhingga
    if (t >= 1.0f) return 1.0f; 
    if (t <= 0.0f) return 0.0f;

    // Hitung frekuensi angular (omega) dan rasio redaman (zeta)
    // Dalam engine tingkat lanjut, nilai akar kuadrat ini biasanya di-Precompute 
    // saat Tween dibuat, namun untuk demonstrasi kita komputasi langsung secara efisien.
    
    // Kita gunakan teknik fast approximation (seperti Fast Inverse Square Root ala Quake)
    // Namun untuk presisi matematis Spring, kita biarkan kompilator mengoptimalkan.
    
    // Konstanta fisika dasar
    float dampingRatio = friction / (2.0f * 10.0f); // 10.0f = estimasi massa akar kuadrat
    float angularFreq = tension * 0.1f; 

    // Jika underdamped (Memantul)
    if (dampingRatio < 1.0f) {
        float dampedFreq = angularFreq * (1.0f - (dampingRatio * dampingRatio));
        
        float exponent = -dampingRatio * angularFreq * t;
        float phase = dampedFreq * t;
        
        float cosPhase = FastCos(phase);
        float sinPhase = FastSin(phase);
        
        float c1 = 1.0f;
        float c2 = (dampingRatio * angularFreq) / dampedFreq;
        
        // 1 - e^(-zeta * omega * t) * (cos(omega_d * t) + (zeta * omega / omega_d) * sin(omega_d * t))
        return 1.0f - FastExp(exponent) * (c1 * cosPhase + c2 * sinPhase);
    }
    
    // Fallback murni jika rasio redaman berlebih (Tidak memantul / Overdamped)
    // Kita gunakan Lerp biasa sebagai pertahanan sistem.
    return t; 
}

} // namespace Mathematics
} // namespace NexusTween
