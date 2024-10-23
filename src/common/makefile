DEST?=/usr/local/lib/chuck

CK_SRC_PATH?=../../chuck/include/

CC_OBJECTS=$(addsuffix .o,$(basename $(CC_MODULES)))

CXX_OBJECTS=$(addsuffix .o,$(basename $(CXX_MODULES)))

CHUG=$(addsuffix .chug,$(CHUGIN_NAME))

LDFLAGS+=-Wl,-undefined,dynamic_lookup -shared 

FLAGS+=-I$(CK_SRC_PATH) -DTEST -Wno-error=narrowing

ifneq ($(CHUCK_DEBUG),)
FLAGS+= -g
else
FLAGS+= -O3
endif
ifneq ($(CHUCK_STRICT),)
FLAGS+= -Werror
endif

ifeq ($(filter mac osx,$(MAKECMDGOALS)),$(MAKECMDGOALS))
FLAGS+=-mmacosx-version-min=10.9 -arch $(shell uname -m)
LDFLAGS+=-mmacosx-version-min=10.9 -arch $(shell uname -m) -lc++ 
CXX=clang++
endif

ifeq ($(filter linux,$(MAKECMDGOALS)),$(MAKECMDGOALS))
FLAGS+=-D__LINUX_ALSA__ -D__PLATFORM_LINUX__ -fPIC
LDFLAGS+=-lstdc++ -fPIC
CXX=g++
endif

LD=$(CXX)
CC=gcc

current:
	@echo "[chugins build]: please use one of the following configurations:"
	@echo "   make linux, make mac, make osx, make clean, or make install"

mac osx linux: $(CHUG)
$(CHUG): $(C_OBJECTS) $(CXX_OBJECTS) $(CC_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(C_OBJECTS): %.o: %.c
	$(GCC) $(FLAGS) -c -o $@ $<

$(CXX_OBJECTS): %.o: %.cpp $(CK_SRC_PATH)/chugin.h
	$(CXX) $(FLAGS) -c -o $@ $<

%.o: %.cc $(CK_SRC_PATH)/chugin.h
	$(CXX) $(FLAGS) -c -o $@ $<

install: $(CHUG)
	mkdir -p $(DEST)
	cp $^ $(DEST)
	chmod 755 $(DEST)/$(CHUG)

clean:
	rm -rf $(C_OBJECTS) $(CC_OBJECTS) $(CXX_OBJECTS) $(CHUG) Release Debug
