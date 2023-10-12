#ifndef ___GAME_STATE_
#define ___GAME_STATE_

#include <stdint.h>
#include "../data/gamedata.h"

enum MissionState { hidden, active, completed };

typedef struct CourseMissionState {
    enum MissionState missionState[NUM_MISSIONS_PER_COURSE];
}CourseMissionState;

typedef struct GameState {
    uint8_t lastCourse;
    CourseMissionState courseMissionState[NUM_COURSES];
}GameState;

int8_t CourseMissionState_completeMission(CourseMissionState* courseMissionState, int8_t mission);
int8_t CourseMissionState_getNextMission(const CourseMissionState* courseMissionState);
int8_t CourseMissionState_activateMission(CourseMissionState* courseMissionState, int8_t mission);

#endif
