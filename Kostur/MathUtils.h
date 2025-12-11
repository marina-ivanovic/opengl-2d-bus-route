#pragma once
#include "Constants.h"

void initRandomPaths();

void getBezierPoint(float t, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float& outX, float& outY);