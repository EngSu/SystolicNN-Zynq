SystolicNN-Zynq Timing Analysis

# 1. Clock Assumption

Target FPGA clock:

100 MHz

Clock period:

10 ns

---

# 2. Hidden Layer Timing

The hidden layer computes:

784 inputs

x

784 x 2 weight matrix

The architecture uses two parallel Processing Elements:

PE0 --> Hidden neuron 0

PE1 --> Hidden neuron 1

Each PE performs:

784 MAC operations

Because one input-weight pair is processed every clock:

Latency:

784 cycles

Time:

784 x 10 ns

= 7.84 us

---

# 3. Sigmoid LUT Timing

The hidden layer produces:

2 values

The sigmoid activation uses a lookup table.

Latency:

2 cycles

Time:

20 ns

---

# 4. Output Layer Timing

Input:

hidden_activation[2]

Weights:

2 x 2

Two PEs operate in parallel:

PE0 --> class 0

PE1 --> class 1

Each PE performs:

2 MAC operations

Latency:

2 cycles

Time:

20 ns

---

# 5. Softmax Timing

Softmax implementation depends on hardware approach.

Initial version:

Software calculation:

ARM processor

Future hardware version:

Softmax accelerator

For first RTL version:

Softmax is considered outside the accelerator timing.

---

# 6. Prediction Timing

Prediction operation:

argmax(probability)

Latency:

1 cycle

---

# 7. Total Accelerator Latency

Current hardware model:

| Operation     | Cycles |
| ------------- | ------ |
| Input loading | TBD    |
| Hidden MAC    | 784    |
| Sigmoid LUT   | 2      |
| Output MAC    | 2      |
| Prediction    | 1      |

Total:

≈789 cycles

At 100 MHz:

789 x 10 ns

= 7.89 us

---

# 8. Performance Metrics

The final implementation will measure:

## Latency

cycles per inference

## Throughput

images per second

## Resource Utilization

Measured from Vivado:

- LUTs
- Flip-Flops
- DSP blocks
- BRAM

## Accuracy

Compare:

Software floating-point model

    |

Fixed-point model

    |

RTL simulation

    |

FPGA hardware

The outputs should match within fixed-point tolerance.
After these two files are done, your documentation set is almost complete:
docs/

├── architecture_specification.md ✅
├── controller_fsm.md ✅
├── dataflow.md ✅
├── memory_map.md ✅
└── timing_analysis.md ✅
