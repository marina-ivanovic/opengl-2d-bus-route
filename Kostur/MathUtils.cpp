#include "../Kostur/MathUtils.h"
#include <cmath>

extern float stationPos[10][2];
extern float pathControlPoints[10][2];

void initRandomPaths() {
    for (int i = 0; i < NUM_STATIONS; i++) {
        int next = (i + 1) % NUM_STATIONS;
        float p0x = stationPos[i][0]; float p0y = stationPos[i][1];
        float p2x = stationPos[next][0]; float p2y = stationPos[next][1];

        float midX = (p0x + p2x) / 2.0f;
        float midY = (p0y + p2y) / 2.0f;
        float dx = p2x - p0x;
        float dy = p2y - p0y;

        float wave = sin((float)i * 2.5f);
        float curvature = 0.35f * wave;

        pathControlPoints[i][0] = midX - dy * curvature;
        pathControlPoints[i][1] = midY + dx * curvature;
    }
}

void getBezierPoint(float t, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float& outX, float& outY) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    outX = uu * p0x + 2 * u * t * p1x + tt * p2x;
    outY = uu * p0y + 2 * u * t * p1y + tt * p2y;
}