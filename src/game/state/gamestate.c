#include "gamestate.h"

int8_t CourseMissionState_completeMission(CourseMissionState* courseMissionState, int8_t mission) {
    if (mission<0 || mission>=NUM_MISSIONS_PER_COURSE) return -1;
    courseMissionState->missionState[mission] = completed;
    return mission;
}

int8_t CourseMissionState_getNextMission(const CourseMissionState* courseMissionState) {
    for (int8_t i=0; i < NUM_MISSIONS_PER_COURSE; i++) {
        enum MissionState missionState = courseMissionState->missionState[i];
        if (missionState == active || missionState == hidden) {
            return i;
        }
    }
    return -1;
}

int8_t CourseMissionState_activateMission(CourseMissionState* courseMissionState, int8_t mission) {
    if (mission<0 || mission>=NUM_MISSIONS_PER_COURSE) return -1;
    courseMissionState->missionState[mission] = active;
    return mission;
}
