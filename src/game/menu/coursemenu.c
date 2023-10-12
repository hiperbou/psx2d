#include "coursemenu.h"

void initCourseMenu(CourseMenu *courseMenu, const Course* course, const GameState* gameState) {
    //const CourseMissionState *currentCourseMissionState = &gameState->courseMissionState[gameState->lastCourse];
    //const enum MissionState *missionState = currentCourseMissionState->missionState;
    const enum MissionState *missionState = gameState->courseMissionState[gameState->lastCourse].missionState;

    int8_t numStars = 1;
    for (int i = NUM_MISSIONS_PER_COURSE - 1; i >= 0; i--) {
        if (missionState[i] != hidden) { numStars = i + 1; break; }
    }

    int8_t nextMission = CourseMissionState_getNextMission(&gameState->courseMissionState[gameState->lastCourse]);
    int8_t selectedItem = nextMission >= 0 ? nextMission : 0;

    *courseMenu = (CourseMenu) {
            .numStars = numStars,
            .selectedItem = selectedItem,
            .missionState = missionState,
            .courseTitle = course->courseTitle,
            .missionDescription = {
                    course->missionDescription[0],
                    course->missionDescription[1],
                    course->missionDescription[2],
                    course->missionDescription[3],
                    course->missionDescription[4],
                    course->missionDescription[5]
            }
    };
}