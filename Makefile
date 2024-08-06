CC=riscv64-unknown-linux-gnu-g++
CC_FLAGS=-std=c++17 -O3 
LD=ld
BUILD_DIR=./build/
TARGET_FILE=utilbru
SRC_DIR=./src/
SRCS=$(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJS=$(patsubst $(SRC_DIR)%.cpp, $(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(wildcard include/*.h)
INCLUDES=\
-I./include/ \


all: $(BUILD_DIR)/$(TARGET_FILE)
	chmod +x $^


# Rule to compile each source file into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) $(INCLUDES) -c $< -o $@


# Rule to link object files into executable
$(BUILD_DIR)/$(TARGET_FILE): $(OBJS)
	$(CC) $^ -o $@ $(CC_FLAGS)
	@chmod +x $@

clean: 
	rm ./build/*