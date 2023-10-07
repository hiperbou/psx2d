#ifndef ___GAME_MENU_COURSE_MENU_
#define ___GAME_MENU_COURSE_MENU_

#include <stdint.h>
#include "../data/gamedata.h"
#include "../state/gamestate.h"
#include "../../core/hgl_actor.h"

typedef struct CourseMenu {
    int8_t numStars;
    int8_t selectedItem;
    Actor *stars[NUM_MISSIONS_PER_COURSE];
    const enum MissionState *missionState;
    const char* missionDescription[NUM_MISSIONS_PER_COURSE];
    const char* courseTitle;
}CourseMenu;

void initCourseMenu(CourseMenu *courseMenu, const Course* course, const GameState* gameState);

#endif