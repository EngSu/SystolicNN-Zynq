###############################################################################
# SystolicNN-Zynq
# Hardware Behavioral Simulator
###############################################################################

CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude

LDFLAGS = -lm


###############################################################################
# Directories
###############################################################################

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build


###############################################################################
# Main Simulator
###############################################################################

TARGET = systolic_reference


SRC = \
	$(SRC_DIR)/clock.c \
	$(SRC_DIR)/input_buffer.c \
	$(SRC_DIR)/weight_memory.c \
	$(SRC_DIR)/activation_buffer.c \
	$(SRC_DIR)/data_loader.c \
	$(SRC_DIR)/pe.c \
	$(SRC_DIR)/systolic_array.c \
	$(SRC_DIR)/systolic_reference.c


OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)


###############################################################################
# Default Build
###############################################################################

all: $(TARGET)



$(TARGET): $(OBJ)

	$(CC) $(OBJ) -o $@ $(LDFLAGS)



###############################################################################
# Compile Object Files
###############################################################################

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c

	mkdir -p $(BUILD_DIR)

	$(CC) $(CFLAGS) -c $< -o $@



###############################################################################
# Unit Tests
###############################################################################

###############################################################################
# Unit Tests
###############################################################################

TEST_DIR = tests
BIN_DIR = bin


test_clock:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_clock.c \
	src/clock.c \
	-o $(BIN_DIR)/test_clock $(LDFLAGS)



test_input_buffer:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_input_buffer.c \
	src/input_buffer.c \
	-o $(BIN_DIR)/test_input_buffer $(LDFLAGS)



test_weight_memory:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_weight_memory.c \
	src/weight_memory.c \
	-o $(BIN_DIR)/test_weight_memory $(LDFLAGS)



test_activation_buffer:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_activation_buffer.c \
	src/activation_buffer.c \
	-o $(BIN_DIR)/test_activation_buffer $(LDFLAGS)



test_pe:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_pe.c \
	src/pe.c \
	-o $(BIN_DIR)/test_pe $(LDFLAGS)

###############################################################################
# Run Tests
###############################################################################

run_tests: test_clock \
           test_input_buffer \
           test_weight_memory \
           test_activation_buffer \
           test_pe

	./test_clock

	./test_input_buffer

	./test_weight_memory

	./test_activation_buffer

	./test_pe


run_clock: test_clock
	./test_clock


run_input_buffer: test_input_buffer
	./test_input_buffer


run_weight_memory: test_weight_memory
	./test_weight_memory


run_activation_buffer: test_activation_buffer
	./test_activation_buffer

run_pe: test_pe

	./test_pe    

###############################################################################
# Clean
###############################################################################

clean:

	rm -rf $(BUILD_DIR)

	rm -f $(TARGET)

	rm -f test_clock

	rm -f test_input_buffer

	rm -f test_weight_memory

	rm -f test_activation_buffer

	rm -f test_pe


###############################################################################
# Help
###############################################################################

help:

	@echo ""
	@echo "Build:"
	@echo " make                 Build simulator"
	@echo " make clean           Clean project"
	@echo ""
	@echo "Tests:"
	@echo " make test_clock"
	@echo " make test_input_buffer"
	@echo " make test_weight_memory"
	@echo " make test_activation_buffer"
	@echo " make run_tests"
	@echo ""