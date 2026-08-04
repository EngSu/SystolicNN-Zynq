###############################################################################
# SystolicNN-Zynq
# Hardware Behavioral Simulator
#
# Build and verification system for the modular C reference simulator.
###############################################################################

CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude -Iconfig

LDFLAGS = -lm


###############################################################################
# Directories
###############################################################################

SRC_DIR   = src
TEST_DIR  = tests
BUILD_DIR = build
BIN_DIR   = bin


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
	$(SRC_DIR)/hidden_layer.c \
	$(SRC_DIR)/sigmoid.c \
	$(SRC_DIR)/output_layer.c \
	$(SRC_DIR)/softmax.c \
	$(SRC_DIR)/predictor.c \
	$(SRC_DIR)/nn_accelerator.c \
	$(SRC_DIR)/systolic_reference.c


OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)


###############################################################################
# Default Build
###############################################################################

.PHONY: all

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

.PHONY: \
	test_clock \
	test_input_buffer \
	test_weight_memory \
	test_activation_buffer \
	test_pe \
	test_systolic_array \
	test_hidden_layer \
	test_output_layer \
	test_sigmoid \
	test_softmax \
	test_predictor \
	test_data_loader \
	test_full_inference \
	test_golden_model


#----------------------------------------------------------------------
# Clock
#----------------------------------------------------------------------

test_clock:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_clock.c \
	$(SRC_DIR)/clock.c \
	-o $(BIN_DIR)/test_clock $(LDFLAGS)


#----------------------------------------------------------------------
# Input Buffer
#----------------------------------------------------------------------

test_input_buffer:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_input_buffer.c \
	$(SRC_DIR)/input_buffer.c \
	-o $(BIN_DIR)/test_input_buffer $(LDFLAGS)


#----------------------------------------------------------------------
# Weight Memory
#----------------------------------------------------------------------

test_weight_memory:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_weight_memory.c \
	$(SRC_DIR)/weight_memory.c \
	-o $(BIN_DIR)/test_weight_memory $(LDFLAGS)


#----------------------------------------------------------------------
# Activation Buffer
#----------------------------------------------------------------------

test_activation_buffer:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_activation_buffer.c \
	$(SRC_DIR)/activation_buffer.c \
	-o $(BIN_DIR)/test_activation_buffer $(LDFLAGS)


#----------------------------------------------------------------------
# Processing Element
#----------------------------------------------------------------------

test_pe:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_pe.c \
	$(SRC_DIR)/pe.c \
	-o $(BIN_DIR)/test_pe $(LDFLAGS)


#----------------------------------------------------------------------
# Systolic Array
#----------------------------------------------------------------------

test_systolic_array:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_systolic_array.c \
	$(SRC_DIR)/systolic_array.c \
	$(SRC_DIR)/pe.c \
	-o $(BIN_DIR)/test_systolic_array $(LDFLAGS)


#----------------------------------------------------------------------
# Hidden Layer
#----------------------------------------------------------------------

test_hidden_layer:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_hidden_layer_real.c \
	$(SRC_DIR)/hidden_layer.c \
	$(SRC_DIR)/systolic_array.c \
	$(SRC_DIR)/pe.c \
	$(SRC_DIR)/data_loader.c \
	-o $(BIN_DIR)/test_hidden_layer $(LDFLAGS)


#----------------------------------------------------------------------
# Sigmoid
#----------------------------------------------------------------------

test_sigmoid:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_sigmoid.c \
	$(SRC_DIR)/sigmoid.c \
	-o $(BIN_DIR)/test_sigmoid $(LDFLAGS)


#----------------------------------------------------------------------
# Output Layer
#----------------------------------------------------------------------

test_output_layer:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_output_layer.c \
	$(SRC_DIR)/output_layer.c \
	$(SRC_DIR)/systolic_array.c \
	$(SRC_DIR)/pe.c \
	-o $(BIN_DIR)/test_output_layer $(LDFLAGS)


#----------------------------------------------------------------------
# Softmax
#----------------------------------------------------------------------

test_softmax:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_softmax.c \
	$(SRC_DIR)/softmax.c \
	-o $(BIN_DIR)/test_softmax $(LDFLAGS)


#----------------------------------------------------------------------
# Predictor
#----------------------------------------------------------------------

test_predictor:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_predictor.c \
	$(SRC_DIR)/predictor.c \
	-o $(BIN_DIR)/test_predictor $(LDFLAGS)


#----------------------------------------------------------------------
# Data Loader
#----------------------------------------------------------------------

test_data_loader:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_data_loader.c \
	$(SRC_DIR)/data_loader.c \
	-o $(BIN_DIR)/test_data_loader $(LDFLAGS)


#----------------------------------------------------------------------
# Full Inference
#----------------------------------------------------------------------

test_full_inference:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_full_inference.c \
	$(SRC_DIR)/nn_accelerator.c \
	$(SRC_DIR)/hidden_layer.c \
	$(SRC_DIR)/output_layer.c \
	$(SRC_DIR)/sigmoid.c \
	$(SRC_DIR)/softmax.c \
	$(SRC_DIR)/predictor.c \
	$(SRC_DIR)/data_loader.c \
	$(SRC_DIR)/systolic_array.c \
	$(SRC_DIR)/pe.c \
	-o $(BIN_DIR)/test_full_inference $(LDFLAGS)


#----------------------------------------------------------------------
# Golden Model Verification
#
# Compares the modular simulator against the original golden reference
# values.
#----------------------------------------------------------------------

test_golden_inference:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_golden_inference.c \
	$(SRC_DIR)/nn_accelerator.c \
	$(SRC_DIR)/hidden_layer.c \
	$(SRC_DIR)/output_layer.c \
	$(SRC_DIR)/sigmoid.c \
	$(SRC_DIR)/softmax.c \
	$(SRC_DIR)/predictor.c \
	$(SRC_DIR)/data_loader.c \
	$(SRC_DIR)/systolic_array.c \
	$(SRC_DIR)/pe.c \
	-o $(BIN_DIR)/test_golden_inference $(LDFLAGS)


test_golden_model:

	mkdir -p $(BIN_DIR)

	$(CC) $(CFLAGS) \
	$(TEST_DIR)/test_golden_model.c \
	$(SRC_DIR)/nn_accelerator.c \
	$(SRC_DIR)/hidden_layer.c \
	$(SRC_DIR)/output_layer.c \
	$(SRC_DIR)/sigmoid.c \
	$(SRC_DIR)/softmax.c \
	$(SRC_DIR)/predictor.c \
	$(SRC_DIR)/data_loader.c \
	$(SRC_DIR)/systolic_array.c \
	$(SRC_DIR)/pe.c \
	-o $(BIN_DIR)/test_golden_model $(LDFLAGS)


###############################################################################
# Run Individual Tests
###############################################################################

.PHONY: \
	run_clock \
	run_input_buffer \
	run_weight_memory \
	run_activation_buffer \
	run_pe \
	run_systolic_array \
	run_hidden_layer \
	run_sigmoid \
	run_output_layer \
	run_softmax \
	run_predictor \
	run_data_loader \
	run_full_inference \
	run_golden_model


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


run_hidden_layer: test_hidden_layer

	./$(BIN_DIR)/test_hidden_layer


run_sigmoid: test_sigmoid

	./$(BIN_DIR)/test_sigmoid


run_output_layer: test_output_layer

	./$(BIN_DIR)/test_output_layer


run_softmax: test_softmax

	./$(BIN_DIR)/test_softmax


run_predictor: test_predictor

	./$(BIN_DIR)/test_predictor


run_data_loader: test_data_loader

	./$(BIN_DIR)/test_data_loader


run_full_inference: test_full_inference

	./$(BIN_DIR)/test_full_inference


run_golden_model: test_golden_model

	./$(BIN_DIR)/test_golden_model

run_golden_inference: test_golden_inference

	./$(BIN_DIR)/test_golden_inference
###############################################################################
# Full Regression Test
###############################################################################

.PHONY: run_tests

run_tests: \
	test_clock \
	test_input_buffer \
	test_weight_memory \
	test_activation_buffer \
	test_pe \
	test_systolic_array \
	test_hidden_layer \
	test_sigmoid \
	test_output_layer \
	test_softmax \
	test_predictor \
	test_data_loader \
	test_full_inference \
	test_golden_model

	@echo ""
	@echo "=============================================="
	@echo " SystolicNN-Zynq Regression Test"
	@echo "=============================================="
	@echo ""

	./$(BIN_DIR)/test_clock

	./$(BIN_DIR)/test_input_buffer

	./$(BIN_DIR)/test_weight_memory

	./$(BIN_DIR)/test_activation_buffer

	./$(BIN_DIR)/test_pe

	./$(BIN_DIR)/test_systolic_array

	./$(BIN_DIR)/test_hidden_layer

	./$(BIN_DIR)/test_sigmoid

	./$(BIN_DIR)/test_output_layer

	./$(BIN_DIR)/test_softmax

	./$(BIN_DIR)/test_predictor

	./$(BIN_DIR)/test_data_loader

	./$(BIN_DIR)/test_full_inference

	./$(BIN_DIR)/test_golden_model

	@echo ""
	@echo "=============================================="
	@echo " ALL TESTS PASSED"
	@echo "=============================================="


###############################################################################
# Clean
###############################################################################

.PHONY: clean

clean:

	rm -rf $(BUILD_DIR)

	rm -rf $(BIN_DIR)

	rm -f $(TARGET)


###############################################################################
# Help
###############################################################################

.PHONY: help

help:

	@echo ""
	@echo "=============================================="
	@echo " SystolicNN-Zynq Simulator"
	@echo "=============================================="
	@echo ""
	@echo "Build:"
	@echo "  make                    Build simulator"
	@echo "  make clean              Remove build files"
	@echo ""
	@echo "Run simulator:"
	@echo "  ./systolic_reference"
	@echo ""
	@echo "Individual tests:"
	@echo "  make run_clock"
	@echo "  make run_input_buffer"
	@echo "  make run_weight_memory"
	@echo "  make run_activation_buffer"
	@echo "  make run_pe"
	@echo "  make run_systolic_array"
	@echo "  make run_hidden_layer"
	@echo "  make run_sigmoid"
	@echo "  make run_output_layer"
	@echo "  make run_softmax"
	@echo "  make run_predictor"
	@echo "  make run_data_loader"
	@echo "  make run_full_inference"
	@echo "  make run_golden_model"
	@echo ""
	@echo "Regression:"
	@echo "  make run_tests"
	@echo ""