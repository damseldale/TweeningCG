# [REVOLUSI KOMPILASI]: Menolak pengaturan default. 
# Kita menetapkan standar arsitektur dan optimasi secara eksplisit.
cmake_minimum_required(VERSION 3.10)
project(NexusTweenEngine VERSION 1.0)

# Paksa penggunaan standar C++17 murni
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Kumpulkan semua sumber arsitektur kita
set(SOURCES
    main.cpp
    Memory/ArenaAllocator.cpp
    Mathematics/SplineCore.cpp
    Systems/TweenMutator.cpp
)

# Ciptakan executable
add_executable(NexusTween ${SOURCES})

# [INJEKSI OPTIMASI PERANTI KERAS]:
# Jika kita menggunakan GCC atau Clang, paksa optimasi O3 dan Vectorization.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # -O3: Agresif inlining dan loop unrolling.
    # -march=native: Izinkan compiler menggunakan instruksi AVX/SIMD dari CPU mesin ini.
    # -ffast-math: Abaikan aturan ketat IEEE 754 untuk floating point demi kecepatan ekstrem.
    target_compile_options(NexusTween PRIVATE -O3 -march=native -ffast-math -Wall -Wextra)
endif()

# Jika menggunakan MSVC (Windows)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # /O2: Maximize speed
    # /arch:AVX2: Enable Advanced Vector Extensions
    # /fp:fast: Fast floating point model
    target_compile_options(NexusTween PRIVATE /O2 /arch:AVX2 /fp:fast /W4)
endif()
