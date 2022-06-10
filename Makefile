SRC_DIR := ./src
BUILD_DIR := ./build
TARGET_DIR := $(SRC_DIR)/target
LIB_DIR := $(SRC_DIR)/lib

TARGET_FILE := $(shell find $(TARGET_DIR) -name "*.cpp")
LIB_FILE := $(shell find $(LIB_DIR) -name "*.cpp")
LIB_HEADER := $(shell find $(LIB_DIR) -name "*.h")


LIB_OBJS := $(LIB_FILE:$(LIB_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET_OBJS := $(TARGET_FILE:$(TARGET_DIR)/%.cpp=$(BUILD_DIR)/%.o) 


TARGETS := $(TARGET_FILE:$(TARGET_DIR)/%.cpp=$(BUILD_DIR)/%)

all: $(TARGETS)

$(TARGETS): %: %.o $(LIB_OBJS)
	g++ -o $@ -O3 -fopenmp -Wall -Wextra -Wshadow $^

$(LIB_OBJS): $(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp $(LIB_HEADER) Makefile | $(BUILD_DIR)
	g++ -c -o $@ -O3 -I . -fopenmp -Wall -Wextra -Wshadow $<
	
$(TARGET_OBJS): $(BUILD_DIR)/%.o: $(TARGET_DIR)/%.cpp $(LIB_HEADER) Makefile | $(BUILD_DIR)
	g++ -c -o $@ -O3 -I . -fopenmp -Wall -Wextra -Wshadow $<

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

EXEC_PATH := ./benchmark
RESULT_FILE := ./result.txt

RUNS := $(TARGETS:$(BUILD_DIR)/%=run_%)

run: $(BUILD_DIR)/maze_routing_mt
	$< $(EXEC_PATH) ./result.txt
	@ echo "" 
	@ echo "Finished! Result is in "./result.txt
	@ echo "" 
	
run_all: $(RUNS)
	
$(RUNS): run_%: $(BUILD_DIR)/%
	$< $(EXEC_PATH) ./result_$@.txt
	@ echo "" 
	@ echo "Finished! Result is in "./result_$@.txt
	@ echo "" 

debug:
	gdb $(TARGET)
	
clean: 
	rm -r build
