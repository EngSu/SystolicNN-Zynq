# SystolicNN-Zynq Memory Architecture

# 1. Overview

The accelerator contains several memory regions used during neural network inference.

The main memories are:

1. Input Memory

2. Weight Memory

3. Activation Memory

4. Output Memory

The memory organization is designed to support the following neural network:
784 input features

    |

    v

Hidden Layer

784 x 2 weights

    |

    v

Sigmoid

    |

    v

Output Layer

2 x 2 weights

    |

    v

Prediction

---

# 2. Input Memory

## Purpose

Stores the MNIST image input vector.

Size:

784 values

Data format:

Initial:

Floating point

Future:

Fixed point Q-format

Organization:

| Address | Data       |
| ------- | ---------- |
| 0       | pixel[0]   |
| 1       | pixel[1]   |
| ...     | ...        |
| 783     | pixel[783] |

Example:

Input_Buffer[0:783]

---

# 3. Hidden Layer Weight Memory

## Purpose

Stores weights for the first computational layer.

Matrix:

W1 = 784 x 2

Meaning:

784 inputs

2 hidden neurons

Organization:

| Address | Hidden Neuron 0 | Hidden Neuron 1 |
| ------- | --------------- | --------------- |
| 0       | W1[0][0]        | W1[0][1]        |
| 1       | W1[1][0]        | W1[1][1]        |
| ...     | ...             | ...             |
| 783     | W1[783][0]      | W1[783][1]      |

Access pattern:

During hidden computation:

input[i]

-

weight[i][neuron]

|

v

PE accumulator

---

# 4. Output Layer Weight Memory

## Purpose

Stores weights for the output layer.

Matrix:

W2 = 2 x 2

Organization:

| Address | Class 0  | Class 1  |
| ------- | -------- | -------- |
| 0       | W2[0][0] | W2[0][1] |
| 1       | W2[1][0] | W2[1][1] |

Input:

hidden_activation[2]

Output:

class_score[2]

---

# 5. Activation Memory

Stores intermediate neural network values.

## Hidden Pre-Activation

Before sigmoid:

hidden_pre_activation[2]

Example:

Z1[0]

Z1[1]

---

## Hidden Activation

After sigmoid:

hidden_activation[2]

These values become the input to the output layer.

---

# 6. Output Memory

Stores final layer results.

Before softmax:

output_score[2]

After softmax:

probability[2]

Final:

prediction

---

# 7. AXI-Lite Register Map

The ARM processor controls the accelerator through AXI-Lite registers.

| Address | Register    | Description           |
| ------- | ----------- | --------------------- |
| 0x00    | CONTROL     | Start accelerator     |
| 0x04    | STATUS      | Busy/done status      |
| 0x08    | INPUT_ADDR  | Input buffer address  |
| 0x0C    | WEIGHT_ADDR | Weight memory address |
| 0x10    | RESULT      | Prediction result     |
| 0x14    | CYCLE_COUNT | Execution cycles      |

---

# 8. Simulator Mapping

The C++ simulator will model these memories as:

InputBuffer

WeightMemory

ActivationBuffer

OutputBuffer

The simulator memory behavior must match the RTL memory architecture.
