# SystolicNN-Zynq Design Decisions

# 1. Project Objective

The goal of this project is to design and implement a complete FPGA-accelerated neural network inference system.

The design demonstrates:

- Neural network implementation
- Hardware acceleration
- Systolic array architecture
- FPGA resource optimization
- Hardware/software co-design
- Embedded Linux control

The target platform is:

- Xilinx Zynq-7020
- ZedBoard
- XC7Z020-CLG484-1

---

# 2. Why FPGA?

FPGA was selected because neural network inference contains highly parallel mathematical operations, especially matrix multiplication.

Compared with software execution on a CPU, FPGA provides:

## Parallel Processing

Multiple multiply-accumulate operations can execute simultaneously.

## Custom Datapath Design

The hardware architecture can be optimized specifically for neural network computation.

## Hardware Pipeline

Data can flow continuously through processing elements, increasing throughput.

## Energy Efficiency

Dedicated hardware computation can achieve better performance per watt compared with general-purpose processors.

---

# 3. Why Systolic Array Architecture?

Matrix multiplication is the dominant operation in neural network inference.

The computation:

Output = Input × Weight Matrix

requires many repeated multiply-accumulate operations.

A systolic array was selected because it provides:

## Data Reuse

Weights and intermediate values can be reused between processing elements.

## Regular Hardware Structure

The repeated PE structure simplifies:

- RTL design
- Verification
- Timing closure

## Pipeline Execution

Each processing element performs one stage of computation while new data enters the array.

---

# 4. Why Two Processing Elements?

The hidden layer contains:

784 inputs

2 hidden neurons

A fully parallel implementation would require:

784 x 2 = 1568 multipliers

which consumes significant FPGA resources.

This design uses:

PE0 → Hidden neuron 0

PE1 → Hidden neuron 1

Advantages:

- Both hidden neurons compute simultaneously.
- Hardware resources remain reasonable.
- The architecture is scalable.

The same PE architecture is reused for the output layer.

Future versions can increase the number of PEs to improve throughput.

---

# 5. Why Weight-Stationary Dataflow?

The design uses a weight-stationary approach.

Weights remain inside the processing elements while inputs stream through the array.

Advantages:

- Reduced memory access
- Improved weight reuse
- Lower communication overhead
- Better energy efficiency

This matches the matrix multiplication pattern of neural networks.

---

# 6. Why Start With Floating Point?

The first implementation uses floating-point arithmetic.

Reason:

The floating-point model provides a golden reference for verification.

Development flow:

Floating-point C++ model

    |

    v

Fixed-point model

    |

    v

RTL implementation

    |

    v

FPGA hardware

This allows numerical accuracy comparison at every stage.

---

# 7. Why Fixed Point?

FPGA resources are limited compared with CPUs and GPUs.

Floating-point operations require:

- More logic resources
- Higher power consumption
- More complex hardware

Fixed-point arithmetic provides:

- Lower resource utilization
- Faster computation
- Efficient DSP usage
- Predictable hardware behavior

The selected fixed-point format will be determined after numerical analysis.

---

# 8. Why Sigmoid LUT?

The sigmoid activation function requires exponential computation:

sigmoid(x)=1/(1+e^-x)

Implementing exponential directly in hardware is expensive.

A lookup table provides:

- Constant execution latency
- Reduced hardware complexity
- Efficient FPGA implementation

The sigmoid LUT is placed between the hidden and output layers.

---

# 9. Why Keep Softmax Initially on ARM?

The first accelerator version focuses on:

- Matrix multiplication
- Systolic array design
- Hardware/software communication

Softmax requires exponential and division operations.

Instead of increasing FPGA complexity, the initial design performs:

FPGA:

Matrix multiplication

ARM:

Softmax and final classification

Future versions can implement a hardware softmax unit.

---

# 10. Why Zynq SoC?

Zynq combines:

ARM Cortex-A9 Processor

FPGA Programmable Logic

This enables:

## Hardware Acceleration

Neural network computation runs in FPGA fabric.

## Software Control

Linux running on ARM controls the accelerator.

## Hardware/Software Co-design

The system demonstrates communication between:

- Linux application
- Kernel driver
- AXI interface
- FPGA accelerator

---

# 11. Why AXI-Lite First?

The first hardware/software interface uses AXI-Lite.

Reason:

- Simple register-based communication
- Easy debugging
- Suitable for control signals

Example:

ARM

|

AXI-Lite

|

Accelerator Controller

DMA can be added later for high-bandwidth data transfer.

---

# 12. Why Build a C++ Hardware Simulator?

The simulator provides a bridge between algorithm and RTL.

Instead of:

Neural network software

    |

    v

Verilog

the development flow becomes:

Neural network model

    |

    v

Cycle-accurate C++ simulator

    |

    v

RTL implementation

The simulator models:

- Processing elements
- Clock cycles
- Data movement
- Controller states
- Pipeline behavior

This reduces RTL debugging time and provides a golden reference model.

---

# 13. Design Synchronization Rule

All implementations must follow the same architecture:

Design Documentation

    |

    v

C++ Hardware Simulator

    |

    v

Verilog RTL

    |

    v

FPGA Hardware

Any architectural change must first be reflected in the documentation.
