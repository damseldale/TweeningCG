#pragma once

namespace NexusTween {
namespace Mathematics {

// Struktur data vektor ultra-ringan. 
// Tanpa virtual function, tanpa constructor berat. Plain Old Data (POD).
struct Vec2 {
    float x;
    float y;
};

struct Vec3 {
    float x;
    float y;
    float z;
};

class SplineCore {
public:
    // Mencegah instansiasi. Kelas ini murni kumpulan komputasi aljabar.
    SplineCore() = delete;

    // [REVOLUSI 1]: Interpolasi Linear (Lerp) tanpa percabangan.
    // Memaksa compiler menjadikannya inline untuk eksekusi 1-cycle CPU.
    static inline float Lerp(float start, float end, float t) {
        // (end - start) dihitung agar lebih presisi pada floating point
        // dibandingkan (start * (1-t) + end * t).
        return start + t * (end - start);
    }

    // [REVOLUSI 2]: Evaluasi Bezier Kubik 1D (Untuk kurva Easing seperti EaseInOut).
    // Menggunakan kurva Bezier standar web/animasi dengan 2 titik kontrol (P1 dan P2).
    // P0 selalu (0,0) dan P3 selalu (1,1).
    static float EvaluateCubicBezierEasing(float t, float p1y, float p2y);

    // [REVOLUSI 3]: Interpolasi posisi spasial (3D) menggunakan Spline Hermite.
    // Jauh lebih mulus untuk pergerakan kamera/karakter dibanding Bezier biasa,
    // dikomputasi menggunakan matriks basis yang di-unroll.
    static Vec3 HermiteInterpolation(const Vec3& p0, const Vec3& p1, const Vec3& m0, const Vec3& m1, float t);
};

} // namespace Mathematics
} // namespace NexusTween
