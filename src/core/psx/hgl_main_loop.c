#include "../hgl.h"

void setMainLoopCallback(MainLoopCallback *mainLoop) {
	while(1) {
		mainLoop();
	}
}
