TARGET = Demo
TYPE = ps-exe

MODULES = core engine game game/data game/menu game/state input media pool utils
MODULES_PSX = platform/psx core/psx engine/psx input/psx media/psx
SOURCES	= src $(addprefix src/,$(MODULES)) $(addprefix src/,$(MODULES_PSX))

CFILES		= $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CPPFILES	= $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))

OBJECTS		= $(CFILES:.c=.o)
CPPOBJECTS	= $(CPPFILES:.cpp=.o)

INCLUDES  = $(addprefix -I,$(SOURCES))

#OBJECTS:=$(patsubst src/%.c,build/%.o,$(wildcard src/*.c))

CLionSources:
	@echo $(CFILES)
	@echo $(CPPFILES)
	@echo $(OBJECTS)
	@echo $(INCLUDES)
	@echo CLION_CPPFLAGS := -Isrc -Isrc/core -Isrc/engine -Isrc/game -Isrc/media -Isrc/pool -Isrc/psx -Isrc/utils
	@echo CLION_SRCS := src/main.c \
    src/game.c \
    src/gamemain.c \
    src/gametitle.c \
    src/gameending.c \
    src/gameresources.c \
    src/core/hgl.c \
    src/core/hgl_anim.c \
    src/core/hgl_command.c \
    src/core/hgl_ent.c \
    src/core/hgl_pool.c \
    src/core/hgl_scroll.c \
    src/core/hgl_spr.c \
    src/core/hgl_text.c \
    src/core/psx/hgl.c \
    src/core/psx/hgl_file.c \
    src/core/psx/hgl_mem.c \
    src/engine/fader.c \
    src/engine/particle.c \
    src/engine/sprites.c \
    src/engine/tilemap.c \
    src/game/actors.c \
    src/game/camera.c \
    src/game/bee.c \
    src/game/block.c \
    src/game/enemyupdate.c \
    src/game/fallToBackgroundScript.c \
    src/game/goal.c \
    src/game/menustar.c \
    src/game/motobug.c \
    src/game/sonic.c \
    src/game/tileshader.c \
    src/game/triggerScript.c \
    src/game/data/gamedata.c \
    src/game/menu/coursemenu.c \
    src/game/state/gamestate.c \
    src/input/psx/input.c \
    src/media/fpg.c \
    src/pool/FixedPool.c \
    src/pool/ObjectPool.c \
    src/psx/system.c \
    src/psx/systemcd.c \
    src/psx/systemmem.c \
    src/utils/utils.c \
    src/utils/cppfunction.cpp

SRCS = ${OBJECTS} \
$(CPPOBJECTS) \
../third_party/common/syscalls/printf.s \
../third_party/common/crt0/crt0.s
CPPFLAGS += -DPSX
CPPFLAGS += -I../third_party/common
CPPFLAGS += -I../third_party/psyq/include
CPPFLAGS += $(INCLUDES)
LDFLAGS += -L../third_party/psyq/lib
LDFLAGS += -Wl,--start-group
LDFLAGS += -lapi
LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
LDFLAGS += -lgs
LDFLAGS += -lgte
LDFLAGS += -lgpu
LDFLAGS += -lgun
LDFLAGS += -lhmd
LDFLAGS += -lmath
LDFLAGS += -lmcrd
LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
LDFLAGS += -ltap
LDFLAGS += -lcd
LDFLAGS += -Wl,--end-group

include ../third_party/common.mk

define OBJCOPYME
$(PREFIX)-objcopy -I binary --set-section-alignment .data=4 --rename-section .data=.rodata,alloc,load,readonly,data,contents -O $(FORMAT) -B mips $< $@
endef

# maps WIP #
TMXS := $(wildcard references/intro/*.tmx)
MAPS := $(subst .tmx,.h,$(subst references/intro/,src/levels/,$(TMXS)))
src/levels/%.h: references/intro/%.tmx
	java -jar tools/tmx2h-1.0-standalone.jar $< > $@;
	touch src/gametitle.c

buildMaps: $(MAPS)
	@echo $(TMXS)
	@echo $(MAPS)

# convert TIM file to bin
%.o: %.tim
	$(call OBJCOPYME)

# convert VAG files to bin
%.o: %.vag
	$(call OBJCOPYME)

# convert HIT to bin
%.o: %.HIT
	$(call OBJCOPYME)

cleaniso:
	-rm -f hello_cd.cue hello_cd.bin hello_cd.VCD

iso: buildMaps all cleaniso
	mkpsxiso -y ./isoconfig.xml
	-CUE2POPS_2_3 hello_cd.cue

load: all
	./../../gdb-multiarch/bin/gdb.exe -q --command=gdb.txt --batch

run: all
	./../../gdb-multiarch/bin/gdb.exe -q --command=gdbrun.txt

emulator: iso
	./../../pcsx-redux/pcsx-redux.exe -run -iso hello_cd.cue

ctoy:
	pushd ./../../../../git/CToy-1.06 ; .\CToy.exe ; popd

cppcheck:
	cppcheck src --cppcheck-build-dir=.cppcheck -i ghz16.c