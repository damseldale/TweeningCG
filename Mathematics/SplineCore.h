#pragma once

namespace NexusTween {
namespace Mathematics {

struct Vec2 { float x; float y; };
struct Vec3 { float x; float y; float z; };

// [REVOLUSI MATEMATIKA]: Menolak <cmath>. Semua fungsi dieksekusi murni via FMA (Fused Multiply-Add).
class SplineCore {
public:
    static constexpr float PI = 3.14159265359f;
    static constexpr float TWO_PI = 6.28318530718f;

    SplineCore() = delete;

    static inline float Lerp(float start, float end, float t) {
        return start + t * (end - start);
    }

    static float EvaluateCubicBezierEasing(float t, float p1y, float p2y);

    static Vec3 HermiteInterpolation(const Vec3& p0, const Vec3& p1, const Vec3& m0, const Vec3& m1, float t);

    // ====================================================================
    // SIMD-FRIENDLY FAST MATH PURE ALGEBRA (No <cmath>)
    // ====================================================================
    
    // Eksponensial Cepat (e^x) menggunakan Maclaurin Series (Derajat 4)
    static inline float FastExp(float x) {
        // e^x = 1 + x + (x^2)/2 + (x^3)/6 + (x^4)/24
        float x2 = x * x;
        float x3 = x2 * x;
        float x4 = x2 * x2;
        return 1.0f + x + (x2 * 0.5f) + (x3 * 0.16666667f) + (x4 * 0.04166667f);
    }

    // Wrap nilai ke dalam batas -PI hingga PI tanpa instruksi branching
    static inline float WrapToPI(float x) {
        // Modulo cepat menggunakan konversi integer
        float quotient = static_cast<float>(static_cast<int>(x * (1.0f / TWO_PI)));
        float wrapped = x - (quotient * TWO_PI);
        // Offset shift tanpa if/else
        wrapped -= (wrapped > PI) ? TWO_PI : 0.0f;
        wrapped += (wrapped < -PI) ? TWO_PI : 0.0f;
        return wrapped;
    }

    // Sinus Cepat menggunakan Taylor Series (Derajat 5)
    static inline float FastSin(float x) {
        x = WrapToPI(x);
        float x2 = x * x;
        float x3 = x2 * x;
        float x5 = x3 * x2;
        // x - (x^3)/6 + (x^5)/120
        return x - (x3 * 0.16666667f) + (x5 * 0.00833333f);
    }

    // Cosinus Cepat murni dari turunan Sinus
    static inline float FastCos(float x) {
        return FastSin(x + (PI * 0.5f));
    }

    // ====================================================================
    // SPRING PHYSICS ENGINE
    // ====================================================================
    
    // Menghasilkan nilai interpolasi (0.0 ke 1.0) dengan efek pantulan organik (Underdamped).
    // tension: Tingkat kekakuan pegas (contoh: 100.0)
    // friction: Tingkat redaman gesekan (contoh: 10.0)
    static float EvaluateSpring(float t, float tension, float friction);
};

} // namespace Mathematics
} // namespace NexusTween
