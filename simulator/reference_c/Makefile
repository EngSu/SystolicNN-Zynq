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
TEST_DIR = tests

BUILD_DIR = build
BIN_DIR = bin


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



test_systolic_array:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_systolic_array.c \
	src/systolic_array.c \
	src/pe.c \
	-o $(BIN_DIR)/test_systolic_array $(LDFLAGS)



###############################################################################
# Run Individual Tests
###############################################################################


run_clock: test_clock

	./$(BIN_DIR)/test_clock



run_input_buffer: test_input_buffer

	./$(BIN_DIR)/test_input_buffer



run_weight_memory: test_weight_memory

	./$(BIN_DIR)/test_weight_memory



run_activation_buffer: test_activation_buffer

	./$(BIN_DIR)/test_activation_buffer



run_pe: test_pe

	./$(BIN_DIR)/test_pe



run_systolic_array: test_systolic_array

	./$(BIN_DIR)/test_systolic_array



###############################################################################
# Full Regression Test
###############################################################################

run_tests: test_clock \
           test_input_buffer \
           test_weight_memory \
           test_activation_buffer \
           test_pe \
           test_systolic_array

	./$(BIN_DIR)/test_clock

	./$(BIN_DIR)/test_input_buffer

	./$(BIN_DIR)/test_weight_memory

	./$(BIN_DIR)/test_activation_buffer

	./$(BIN_DIR)/test_pe

	./$(BIN_DIR)/test_systolic_array



###############################################################################
# Clean
###############################################################################

clean:

	rm -rf $(BUILD_DIR)

	rm -rf $(BIN_DIR)

	rm -f $(TARGET)



###############################################################################
# Help
###############################################################################

help:

	@echo ""
	@echo "=============================="
	@echo " SystolicNN-Zynq Simulator"
	@echo "=============================="
	@echo ""
	@echo "Build:"
	@echo " make                    Build simulator"
	@echo " make clean              Remove build files"
	@echo ""
	@echo "Run simulator:"
	@echo " ./systolic_reference"
	@echo ""
	@echo "Tests:"
	@echo " make test_clock"
	@echo " make test_input_buffer"
	@echo " make test_weight_memory"
	@echo " make test_activation_buffer"
	@echo " make test_pe"
	@echo " make test_systolic_array"
	@echo ""
	@echo "Regression:"
	@echo " make run_tests"
	@echo ""