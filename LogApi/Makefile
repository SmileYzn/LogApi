# Makefile - LogApi (Linux Build - Estático)
MAKEFLAGS += --warn-undefined-variables --silent

BUILD_DIR=Release
BUILD_NAME=logapi_mm
BUILD_SOURCES=$(wildcard *.cpp)
BUILD_OBJECTS=$(BUILD_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
BUILD_COMPILER=g++

BUILD_CFLAGS=-std=c++11 -Wall -fcommon -m32 -O3 -fPIC -msse4.1 -march=i686 \
             -D_GLIBCXX_USE_CXX11_ABI=0 -fno-stack-protector -fno-strict-aliasing \
             -DCURL_STATICLIB -DHTTP_ONLY

# Linkagem ESTÁTICA do libcurl
LDFLAGS=-m32 -shared -static-libstdc++ -static-libgcc
LIBS=-L/usr/local/curl32/lib -lcurl -lz -lpthread -lrt -ldl -lm

INCLUDE=-I. -Iinclude/metamod -Iinclude/cssdk/common -Iinclude/cssdk/dlls \
        -Iinclude/cssdk/engine -Iinclude/cssdk/game_shared -Iinclude/cssdk/pm_shared \
        -Iinclude/cssdk/public -I/usr/local/curl32/include

default: prebuild linux32

prebuild:
	mkdir -p $(BUILD_DIR)

linux32: $(BUILD_OBJECTS)
	$(BUILD_COMPILER) $(LDFLAGS) -o $(BUILD_DIR)/$(BUILD_NAME).so $(BUILD_OBJECTS) $(LIBS)

$(BUILD_DIR)/%.o: %.cpp
	$(BUILD_COMPILER) $(INCLUDE) $(BUILD_CCFLAGS) -o $@ -c $<

clean:
	rm -rf $(BUILD_DIR)
