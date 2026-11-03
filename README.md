# SystolicNN-Zynq

## FPGA-Based Neural Network Accelerator for Xilinx Zynq SoC

SystolicNN-Zynq is an FPGA hardware acceleration project focused on designing, implementing, and verifying a fixed-point feed-forward neural network for handwritten-digit classification on the Xilinx Zynq-7000 SoC.

The project follows a hardware/software co-design approach, beginning with a validated C reference model and progressing toward a configurable RTL accelerator, FPGA implementation, AXI-based communication, and embedded software integration.

The long-term architecture aims to explore systolic-array-based neural-network acceleration, including processing-element organization, dataflow, resource utilization, latency, and throughput.

**Current implementation:** A verified two-processing-element output-stationary datapath with synchronous ROM-based input and weight access.

## Target Hardware

| Component          | Specification                |
| ------------------ | ---------------------------- |
| Development board  | ZedBoard                     |
| FPGA/SoC           | Xilinx Zynq-7000             |
| Device             | XC7Z020-CLG484-1             |
| Processing system  | Dual-core ARM Cortex-A9      |
| Programmable logic | FPGA fabric                  |
| RTL language       | Verilog/SystemVerilog        |
| Software reference | C                            |
| FPGA development   | AMD Vivado (target workflow) |
| RTL simulation     | Icarus Verilog               |

## Neural Network Architecture

The initial accelerator targets a small feed-forward neural network trained for a two-class MNIST classification task.

**Network topology:** 784 → 2 → 2

- Input layer: 784 normalized MNIST pixels (28 × 28)
- Hidden layer: 2 neurons with sigmoid activation
- Output layer: 2 neurons
- Classification: Argmax of the two output scores

The hardware inference datapath does not require floating-point softmax to determine the predicted class.

### Fixed-Point Representation

| Quantity                | Representation                    |
| ----------------------- | --------------------------------- |
| Input activations       | Signed Q1.15, 16-bit              |
| Neural-network weights  | Signed Q1.15, 16-bit              |
| Multiplication products | Signed 32-bit, 30 fractional bits |
| MAC accumulators        | Signed 64-bit, 30 fractional bits |
| Hidden pre-activations  | Signed Q4.12, 16-bit              |
| Sigmoid outputs         | Signed Q1.15, 16-bit              |
| Sigmoid approximation   | 32-entry LUT                      |

The numerical specification includes defined rounding, saturation, LUT indexing, and signed arithmetic behavior.

## Development and Verification Flow

```text
Floating-Point C Reference
            |
            v
Fixed-Point C Model
            |
            v
Integer-Only Inference Verification
            |
            v
RTL Reference Vector Generation
            |
            v
Verilog Multiplier and MAC
            |
            v
Two-PE Output-Stationary Datapath
            |
            v
Synchronous ROM + Hidden-Layer Controller
            |
            v
Fixed-Point Activation + Output Layer
            |
            v
Complete RTL Neural-Network Inference
            |
            v
Vivado Synthesis and FPGA Implementation
            |
            v
AXI and Zynq Processing-System Integration
            |
            v
Embedded Software and Linux Drivers
            |
            v
Performance and Resource Evaluation
```

The flow distinguishes completed implementation and verification milestones from planned development.

## Verification Results

### Software Reference

The floating-point and fixed-point C models have been evaluated on 2,115 MNIST input images.

- 2,115 images evaluated
- Zero prediction mismatches between the integer C model and floating-point reference
- Zero prediction mismatches against the existing C LUT inference path
- Exhaustive comparison of all 65,536 signed Q4.12 input encodings for sigmoid LUT equivalence
- Zero LUT-equivalence mismatches

Prediction agreement is distinct from classification accuracy against MNIST labels.

### RTL Verification

The following modules have been implemented and tested:

| Component                      | Verification                                                   |
| ------------------------------ | -------------------------------------------------------------- |
| Signed Q1.15 multiplier        | Directed arithmetic and boundary tests                         |
| 64-bit MAC                     | Cycle-by-cycle accumulation checks                             |
| Two-PE output-stationary array | Validated against integer C reference                          |
| Synchronous Q1.15 ROM          | Integrated and simulated                                       |
| Hidden-layer controller        | 784 memory reads and MAC operations per image                  |
| ROM-backed hidden layer        | Exact hidden-accumulator agreement with C for two MNIST images |

The ROM-backed hidden layer has passed simulation for two reference images, including cases where hidden-layer accumulations exceed the signed 32-bit range.

These results establish functional RTL simulation correctness for the tested cases, not FPGA timing closure or physical board validation.

## Repository Organization

```text
SystolicNN-Zynq/
|
├── rtl/
│   ├── mac/
│   ├── pe/
│   ├── memory/
│   ├── control/
│   └── hidden/
|
├── tb/
│   ├── mac/
│   ├── pe/
│   └── hidden/
|
├── simulator/
│   ├── reference_c/
│   └── fixed_point/
|
├── data/
│   └── test_vectors/
│       └── q15_baseline/
|
└── docs/
    └── verification/
```

The project separates synthesizable RTL, self-checking testbenches, software reference models, verification data, and documentation.

## Current Project Status

**Completed**

- [x] Project architecture and numerical baseline
- [x] Floating-point C reference verification
- [x] Fixed-point C inference model
- [x] 32-entry sigmoid LUT verification
- [x] Integer-only inference across 2,115 MNIST images
- [x] RTL reference-vector generation
- [x] Signed Q1.15 multiplier
- [x] Signed 64-bit MAC
- [x] Two-PE output-stationary datapath
- [x] Synchronous ROM-based hidden-layer controller
- [x] Hidden-layer C-to-RTL accumulator verification

**Upcoming**

- [ ] RTL Q30-to-Q4.12 conversion
- [ ] RTL sigmoid LUT
- [ ] Output-layer computation and classification
- [ ] Complete 784–2–2 neural-network RTL integration
- [ ] Wider RTL regression testing
- [ ] Systolic dataflow architecture exploration
- [ ] Vivado synthesis, resource analysis, and timing closure
- [ ] ZedBoard FPGA deployment
- [ ] AXI interface and processing-system integration
- [ ] Embedded software and Linux driver development
- [ ] Performance, resource, and power benchmarking

## Engineering Objectives

This project is intended to develop and evaluate reusable hardware acceleration techniques, including:

- Fixed-point quantization and numerical accuracy
- RTL architecture and processing-element design
- Multiply-accumulate optimization
- Output-stationary and systolic dataflow exploration
- Synchronous FPGA memory integration
- Controller and datapath verification
- Hardware/software communication on Zynq SoC
- Latency, throughput, resource, and power trade-offs

## Current Development Focus

The next milestone is to implement the fixed-point activation stage, converting the verified 64-bit hidden-layer accumulators into Q4.12 pre-activations and Q1.15 sigmoid outputs.

The implementation will be checked against the existing C-generated reference vectors before output-layer integration.

## Project Status

**Active development — Integer C reference verified; ROM-backed hidden-layer RTL verified in simulation.**

Target platform: Xilinx ZedBoard (XC7Z020).

FPGA synthesis, hardware deployment, and complete neural-network RTL inference remain in progress.
