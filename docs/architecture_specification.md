with this:

# SystolicNN-Zynq Accelerator Architecture Specification

# 1. Project Overview

This project implements a complete FPGA-accelerated Feed Forward Neural Network (FFNN) inference system using a systolic array architecture on a Xilinx Zynq SoC.

Target hardware:

- FPGA: Xilinx Zynq-7020
- Board: ZedBoard
- Device: XC7Z020-CLG484-1
- Development Tool: Vivado 2025

The accelerator performs binary classification on the MNIST dataset.

Classes:

- Digit 0
- Digit 1

The neural network training is performed offline.

The FPGA accelerator is responsible for inference only using pre-trained weights.

The system demonstrates:

- Neural network inference
- Systolic array architecture
- Hardware/software co-design
- AXI communication
- Embedded Linux control
- FPGA acceleration
- Fixed-point optimization

---

# 2. Neural Network Topology

The accelerator implements a two-layer Feed Forward Neural Network.

The input layer is not a computational layer. It provides the input features to the first computational layer.

The network contains:

1. Hidden Layer

2. Output Layer

## Layer 1: Hidden Layer

Input:

784 pixel values

Input format:

Normalized value:
0.0 - 1.0

Weight matrix:

784 x 2

Computation:

Z1 = X × W1

The hidden layer contains:

Neuron 0
Neuron 1

Activation:

Sigmoid

Output:

hidden[0]

hidden[1]

These two activated values become the input to the output layer.

---

# Layer 2: Output Layer

Input:

hidden[0]

hidden[1]

Weight matrix:

2 x 2

Computation:

Z2 = Hidden × W2

Output:

class_score[0]

class_score[1]

Activation:

Softmax

Final output:

Probability(class 0)

Probability(class 1)

Prediction:

prediction = argmax(probability)

---

# 3. Top Level Accelerator Architecture

              ARM Cortex-A9
                   |
                   |
                AXI-Lite
                   |
                   v

         Accelerator Controller


                   |
    +--------------+--------------+
    |                             |
    v                             v

Input Buffer Weight Memory

    |                             |
    |                             |
    |              +--------------+
    |              |
    |              |
    v              v

+-----------------------------------------+
| Hidden Layer Accelerator |
| |
| 2 Processing Elements |
| |
| PE0 --> Hidden Neuron 0 |
| PE1 --> Hidden Neuron 1 |
| |
+-----------------------------------------+

                   |
                   |
                   v


          Sigmoid Activation LUT


                   |
                   |
                   v

+-----------------------------------------+
| Output Layer Accelerator |
| |
| 2 Processing Elements |
| |
| PE0 --> Class 0 Score |
| PE1 --> Class 1 Score |
| |
+-----------------------------------------+

                   |
                   |
                   v


          Softmax / Argmax


                   |
                   |
                   v


             Prediction

---

# 4. Processing Element Architecture

The Processing Element (PE) is the fundamental computation unit.

Each PE performs a multiply-accumulate operation:

partial_sum =
partial_sum + input × weight

Internal registers:

| Register    | Purpose                      |
| ----------- | ---------------------------- |
| input_reg   | Stores input value           |
| weight_reg  | Stores weight value          |
| product_reg | Stores multiplication result |
| accumulator | Stores partial sum           |

The same PE architecture is reused for:

Hidden Layer:

784 MAC operations

Output Layer:

2 MAC operations

---

# 5. Hidden Layer Accelerator

The hidden layer uses two parallel PEs.

         Input Stream

              |

    +---------+---------+

    |                   |

    v                   v

PE0 PE1

    |                   |

    v                   v

Hidden[0] Hidden[1]

Each PE calculates one neuron.

Operations:

Hidden neuron =
sum(input[i] × weight[i])

Latency:

784 clock cycles

---

# 6. Output Layer Accelerator

The output layer reuses the PE architecture.

Input:

hidden[0]

hidden[1]

Two parallel PEs calculate:

PE0 --> Class 0 score

PE1 --> Class 1 score

Latency:

2 clock cycles

---

# 7. Design Synchronization Rule

The following components must remain synchronized:

Architecture Specification

      |

      v

C/C++ Behavioral Simulator

      |

      v

Verilog RTL Implementation

      |

      v

FPGA Hardware Implementation

The C++ simulator represents the golden hardware behavior and is used as the reference model for RTL verification.
