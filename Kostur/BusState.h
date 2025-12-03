#pragma once

enum BusMode {
    MOVING,
    WAITING
};

struct State {
    BusMode mode;
    int currentStation;
    int nextStation;
    float t;
    float waitTimer;

    int passengers;
    int fines;
    bool hasControl;
    bool doorsOpen;
};