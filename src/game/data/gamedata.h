#ifndef ___GAME_DATA_
#define ___GAME_DATA_

#define NUM_COURSES 10
#define NUM_MISSIONS_PER_COURSE 6

typedef struct Course {
    const char* missionDescription[NUM_MISSIONS_PER_COURSE];
    const char* courseTitle;
}Course;

typedef struct GameData {
    const Course course[NUM_COURSES];
}GameData;

extern GameData gameData;

#endif