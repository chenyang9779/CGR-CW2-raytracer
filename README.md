# Raytracer

A simple raytracer program built in C++ that supports configurable options for BVH usage, tone mapping, and anti-aliasing.

---

## Features

- Supports JSON configuration files for scene setup.
- Implements BVH (Bounding Volume Hierarchies) for accelerated ray tracing (optional).
- Supports tone mapping for realistic image rendering (optional).
- Anti-aliasing for improved image quality (optional).

---

## How to Build

### Prerequisites

- **CMake** (version 3.10 or higher).
- A **C++ Compiler** that supports at least C++11 (e.g., GCC, Clang, MSVC).

### Steps to Build

1. **cd into the project's directory**:

   ```bash
   cd s2693586/Code
   ```

2. **Create a Build Directory**:

   ```bash
   mkdir build
   cd build
   ```
3. **Configure the Project with CMake**:
   
   ```bash
   cmake ..
   ```
4. **Build the Project**:
   ```bash
   make
   ```
### Usage

#### Command

To run the raytracer after building, use the following command:
```bash
./Raytracer <path_to_json_file> <output_file> <use_bvh (0 or 1)> <apply_tone_map (0 or 1)> <antialiasing (0 or 1)>
```

#### Arguments
1. **`path_to_json_file`**: Path to the input JSON configuration file containing scene details.
2. **`path_to_output_file`**: Path to the output file (e.g., `output.png`).
3. **`use_bvh`**: Set to `1` to enable BVH acceleration, or `0` to disable it.
4. **`apply_tone_map`**: Set to `1` to apply tone mapping, or `0` to disable it.
5. **`antialiasing`**: Set to `1` to enable anti-aliasing, or `0` to disable it.
