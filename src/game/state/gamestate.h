#ifndef ___GAME_STATE_
#define ___GAME_STATE_

#include <stdint.h>

enum MissionState { hidden, active, completed };

typedef struct CourseMissionState {
    enum MissionState missionState[NUM_MISSIONS_PER_COURSE];
}CourseMissionState;

typedef struct GameState {
    int8_t lastCourse;
    int8_t lastMission;
    CourseMissionState courseMissionState[NUM_COURSES];
}GameState;

#endif
