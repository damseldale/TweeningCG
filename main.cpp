#include <iostream>
#include <vector>
// (Sertakan semua file NexusTween yang telah kita buat)
#include "API/NexusTween.h"

int main() {
    std::cout << "[Nexus Engine] Menginisialisasi arsitektur DOD..." << std::endl;

    // 1. Instansiasi Engine Utama
    NexusTween::API::NexusEngine engine;

    // 2. Booting dengan kapasitas masif: 100.000 animasi sekaligus.
    // Memori dipesan (Pre-allocated) ke OS hanya dalam baris ini. Tidak ada malloc lagi setelahnya.
    engine.Boot(100000);

    // 3. Injeksi Data Masal
    std::cout << "[Nexus Engine] Menyiapkan 100.000 entitas..." << std::endl;
    std::vector<NexusTween::API::TweenHandle> handles;
    handles.reserve(100000);

    for (int i = 0; i < 100000; i++) {
        // Animasi bergerak dari X=0 ke X=1000
        handles.push_back(engine.Animate(0.0f, 0.0f, 0.0f, 1000.0f, 50.0f, 0.0f));
    }

    // Mendemonstrasikan keunggulan O(1) Bit-Masking
    // Kita jeda (Pause) 50.000 entitas ganjil secara instan
    for (int i = 1; i < 100000; i += 2) {
        engine.Pause(handles[i]);
    }

    std::cout << "[Nexus Engine] Memulai Game Loop (Simulasi 60 FPS)..." << std::endl;
    
    // 4. Game Loop / Execution Pipeline
    float deltaTime = 0.016f; // Simulasi ~60 FPS (16ms per frame)
    int framesToSimulate = 10; // Kita jalankan 10 putaran untuk demonstrasi

    for (int frame = 0; frame < framesToSimulate; frame++) {
        // Panggilan tunggal ini mengeksekusi Spring Physics pada 100.000 data 
        // menggunakan SIMD (Vectorization) di bawah kap.
        engine.Tick(deltaTime);

        // Menarik data satu entitas yang aktif (Genap)
        float x, y, z;
        engine.GetCurrentPosition(handles[0], x, y, z);
        
        // Menarik data satu entitas yang di-pause (Ganjil)
        float px, py, pz;
        engine.GetCurrentPosition(handles[1], px, py, pz);

        std::cout << "Frame " << frame << " | "
                  << "Entitas [0] (Aktif): X=" << x << " | "
                  << "Entitas [1] (Pause): X=" << px << std::endl;
    }

    // 5. Pemusnahan Instan
    engine.Shutdown();
    std::cout << "[Nexus Engine] Memori dikembalikan ke OS. Operasi selesai." << std::endl;

    return 0;
}
