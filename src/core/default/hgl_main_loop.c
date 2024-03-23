
#include "../hgl.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

void setMainLoopCallback(MainLoopCallback *mainLoop) {
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(mainLoop, 60, true);
    #else
	while(1) {
		mainLoop();
	}
    #endif
}