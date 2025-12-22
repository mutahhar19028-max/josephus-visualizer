# Josephus Problem Visualizer

A hardware-accelerated, real-time visualization of the Josephus permutation problem using C++ and Raylib. This project contrasts the algorithmic simulation ( O(N  . K) ) against the mathematical closed-form solution ( O(N) ) to determine the survivor in a circular elimination game.

## ⚡ Technical Overview

### Core Architecture
* **Simulation Engine:** Raylib (Immediate Mode GUI).
* **Data Structure:** Custom `CircularQueue<T>` implementation using raw pointers for manual memory control. This strictly enforces the circular topology required by the problem domain, avoiding the overhead of index modulation found in `std::vector` approaches.
* **Algorithm:**
    1.  **Visual Simulation:** Physically rotates the head of the queue K times and performs a deletion ( O(1) ) per elimination step.
    2.  **Mathematical Validation:** Pre-calculates the survivor using the iterative recursive formula  J(n, k) = (J(n-1, k) + k) \pmod n  to validate the simulation result.

### Complexity Analysis
| Method | Time Complexity | Space Complexity | Notes |
| :--- | :--- | :--- | :--- |
| **Simulation** |  O(N  . K)  |  O(N)  | Accurate state visualization. High CPU usage for large  N, K . |
| **Formula** |  O(N)  |  O(1)  | Used for instant prediction and result validation. |

---

## 🛠️ Build & Dependencies

### Requirements
* **C++ Compiler:** C++17 or higher (GCC/Clang/MSVC).
* **Library:** [Raylib 4.5+](https://www.raylib.com/).

### Compilation
Link against `raylib`, `opengl32`, `gdi32`, and `winmm` (Windows) or `GL`, `m`, `pthread`, `dl`, `rt`, `X11` (Linux).

**MinGW / G++:**
```bash
g++ main.cpp -o josephus -O3 -lraylib -lopengl32 -lgdi32 -lwinmm
```

## 📂 Asset Structure
The application strictly requires the following directory hierarchy relative to the executable. Missing assets will cause runtime assertions.
```
├── main.cpp
├── josephus.exe
└── assets/
    ├── font/
    │   └── vister-regular.otf
    ├── visuals/
    │   ├── background.gif
    │   ├── sprite.png
    │   └── deathImage.png
    └── audio/
        ├── deathSound.wav
        ├── bgm.mp3
  ```

## 🎮 Controls
The simulation runs a Finite State Machine (FSM) handling manual stepping, autoplay, and pause states.

| Key | Function | Description |
| :--- | :--- | :--- |
| **SPACE** | Toggle Autoplay | Switches between manual stepping and time-interval based automatic elimination. |
| **ENTER** | Manual Step | Executes exactly one elimination round (removes  K -th node). |
| **ESC** | Pause / Resume | Halts the render loop updates and audio stream. |
| **R** | Soft Reset | Re-initializes the queue and visual state without restarting the application. |

## 🧩 Implementation Details

Circular Queue (CircularQueue<T>)
A bespoke container tailored for the Josephus problem.
Enqueue:  O(1)  via tail pointer.Rotation: next() operation moves head/tail pointers in  O(1)  without data copying.
Memory: RAII-compliant destructor ensures clean teardown of the circular linked list.

Visual Rendering
Interpolation: Survivor animations use sine-wave scaling and alpha blending for "death" states.
State Decoupling: The visual representation (std::vector<Vector2>) is decoupled from the logic structure (CircularQueue), 
allowing the visual engine to render "dead" entities (ghosts) even after they are logically removed from memory.

