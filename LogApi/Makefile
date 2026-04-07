# Makefile settings
MAKEFLAGS += --warn-undefined-variables --silent
#
# Build directory
BUILD_DIR=Release
#
# System based commands
PRE_CMD=mkdir -p $(BUILD_DIR)
POS_CMD=sh PostBuild.sh
CLEAN_CMD=rm -rf $(BUILD_DIR)
#
# Common build definitions
BUILD_NAME=logapi_mm
BUILD_SOURCES=$(wildcard *.cpp)
BUILD_CFLAGS=-std=c++11 -Wall -Wno-unknown-pragmas -fcommon -m32 -mtune=generic -msse3 -msse4.1 -shared -s -pipe -g0 -O3 -fPIC -funroll-loops -fomit-frame-pointer -fno-stack-protector -fvisibility=hidden
BUILD_TARGET=linux32
BUILD_EXTENSION=so
BUILD_OBJECTS=$(BUILD_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
BUILD_COMPILER=c++
BUILD_LINKER=-lcurl -lssl -lcrypto -ldl -lm -lz
BUILD_CCFLAGS=$(BUILD_CFLAGS) -DNDEBUG -Dlinux -D__linux__ -D__BUILD__ -D_GLIBCXX_USE_CXX11_ABI=0 -DCURL_STATICLIB -DHTTP_ONLY
#
# Include directory
SRCDIR=.
METAMOD=include/metamod
SDK=include/cssdk
INCLUDE=-I$(SRCDIR) -I$(METAMOD) -I$(SDK)/common -I$(SDK)/dlls -I$(SDK)/engine -I$(SDK)/game_shared -I$(SDK)/pm_shared -I$(SDK)/public
#
# Defult: prebuild, linux32, posbuild
default: prebuild $(BUILD_TARGET) posbuild
#
# Linux: prebuild, linux32, posbuild
linux: $(BUILD_TARGET)
#
# Pre-build: Execute command
prebuild:
	$(PRE_CMD)
#
# Pos-build: Execute command
posbuild:
	$(POS_CMD)
#
# Linux target build
$(BUILD_TARGET): $(BUILD_OBJECTS)
	$(BUILD_COMPILER) $(INCLUDE) $(BUILD_CCFLAGS) $(BUILD_OBJECTS) $(BUILD_LINKER) -o$(BUILD_DIR)/$(BUILD_NAME).$(BUILD_EXTENSION)
$(BUILD_DIR)/%.o: %.cpp
	$(BUILD_COMPILER) $(INCLUDE) $(BUILD_CCFLAGS) -o $@ -c $<
#
# Clean
clean:
	$(CLEAN_CMD)
