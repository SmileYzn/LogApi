# Makefile - LogApi (Linux 32-bit Build)
MAKEFLAGS += --warn-undefined-variables --silent

BUILD_DIR=Release
BUILD_NAME=logapi_mm
BUILD_SOURCES=$(wildcard *.cpp)
BUILD_OBJECTS=$(BUILD_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
BUILD_COMPILER=g++

# ✅ FLAGS DE COMPILAÇÃO (inclui -m32)
BUILD_CFLAGS=-std=c++11 -Wall -fcommon -m32 -O3 -fPIC -msse4.1 -march=i686 \
             -D_GLIBCXX_USE_CXX11_ABI=0 -fno-stack-protector -fno-strict-aliasing \
             -DCURL_STATICLIB -DHTTP_ONLY -DNDEBUG -Dlinux

# ✅ FLAGS DE LINK (inclui -m32 e bibliotecas)
BUILD_LDFLAGS=-m32 -shared -static-libstdc++ -static-libgcc
BUILD_LIBS=-L/usr/local/curl32/lib -lcurl -lz -lpthread -lrt -ldl -lm

# ✅ INCLUDES
INCLUDE=-I. -Iinclude/metamod -Iinclude/cssdk/common -Iinclude/cssdk/dlls \
        -Iinclude/cssdk/engine -Iinclude/cssdk/game_shared -Iinclude/cssdk/pm_shared \
        -Iinclude/cssdk/public -I/usr/local/curl32/include

default: prebuild linux32

prebuild:
	mkdir -p $(BUILD_DIR)

# ✅ REGRA DE COMPILAÇÃO (usa BUILD_CFLAGS corretamente)
$(BUILD_DIR)/%.o: %.cpp
	$(BUILD_COMPILER) $(INCLUDE) $(BUILD_CFLAGS) -o $@ -c $<

# ✅ REGRA DE LINK (usa BUILD_LDFLAGS e BUILD_LIBS)
linux32: $(BUILD_OBJECTS)
	$(BUILD_COMPILER) $(BUILD_LDFLAGS) -o $(BUILD_DIR)/$(BUILD_NAME).so $(BUILD_OBJECTS) $(BUILD_LIBS)

clean:
	rm -rf $(BUILD_DIR)
