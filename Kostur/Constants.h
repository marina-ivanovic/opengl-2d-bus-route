#pragma once
#include <cmath>

// --- CONSTANTS ---
const int NUM_STATIONS = 10;
const float PI = 3.14159265359f;
const double targetFPS = 75.0;
const double targetFrameTime = 1.0 / targetFPS;

// DECLARATIONS
extern float stationPos[10][2];
extern float pathControlPoints[10][2];