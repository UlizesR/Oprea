CC := gcc
SRC_DIR := src
BUILD_DIR := bin
TEST_DIR := test
INCLUDE_DIR := include

# External library paths
CPLOTLIB_DIR := external/CPlotLib
CPLOTLIB_INCLUDE := $(CPLOTLIB_DIR)/include
CPLOTLIB_LIB := $(CPLOTLIB_DIR)/lib

# Get pkg-config flags for GLEW and GLFW3
PKG_LIBS := $(shell pkg-config --libs glew glfw3)
PKG_CFLAGS := $(shell pkg-config --cflags glew glfw3)

# Compiler flags
CFLAGS := -I$(INCLUDE_DIR) -I$(CPLOTLIB_INCLUDE) $(PKG_CFLAGS)

# Linker flags
LDFLAGS := -L$(CPLOTLIB_LIB) -lcpl -framework OpenGL $(PKG_LIBS)

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)

# Default target
all: $(BUILD_DIR)/test1

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build CPlotLib first
$(CPLOTLIB_LIB)/libcpl.a:
	$(MAKE) -C $(CPLOTLIB_DIR)

# Build the test (now depends on CPlotLib)
$(BUILD_DIR)/test1: $(CPLOTLIB_LIB)/libcpl.a $(TEST_DIR)/2D_Curves.c $(SRCS) | $(BUILD_DIR)
	$(CC) $(TEST_DIR)/2D_Curves.c $(SRCS) -o $@ $(CFLAGS) $(LDFLAGS)

# Clean build files 
clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C $(CPLOTLIB_DIR) clean

.PHONY: all clean