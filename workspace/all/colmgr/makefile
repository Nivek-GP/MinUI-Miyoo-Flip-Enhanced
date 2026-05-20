###########################################################

ifeq (,$(PLATFORM))
PLATFORM=$(UNION_PLATFORM)
endif

ifeq (,$(PLATFORM))
$(error please specify PLATFORM)
endif

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

###########################################################

include ../../$(PLATFORM)/platform/makefile.env

DC_SRC = ../../$(PLATFORM)/other/DinguxCommander-sdl2/src
DPLATFORM = PLATFORM_$(shell echo $(PLATFORM) | tr 'a-z' 'A-Z')
RESDIR = res

TARGET  = colmgr
PRODUCT = build/$(PLATFORM)/$(TARGET).elf

SRCS = colmgr.cpp resourceManager.cpp window.cpp \
       $(DC_SRC)/sdlutils.cpp \
       $(DC_SRC)/keyboard.cpp

OBJS = $(patsubst %.cpp,build/$(PLATFORM)/%.o,$(notdir $(SRCS)))

CC      = $(CROSS_COMPILE)g++
CFLAGS  = $(ARCH) -fomit-frame-pointer -Ofast
CFLAGS += -I. -I$(DC_SRC) -I$(PREFIX)/include/SDL2
CFLAGS += -D$(DPLATFORM) -DRESDIR="\"$(RESDIR)\""
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

VPATH = $(DC_SRC)

all:
	mkdir -p build/$(PLATFORM)
	$(CC) $(SRCS) -o $(PRODUCT) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf build/$(PLATFORM)
