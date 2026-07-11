---

# File: docs/design_decisions.md

Add:

```markdown
# Design Decisions

## Why FPGA?

FPGA provides:

- Parallel computation
- Custom datapath design
- Hardware acceleration
- Energy efficiency

## Why Systolic Array?

Matrix multiplication dominates neural network inference.

A systolic array provides:

- Data reuse
- Regular architecture
- High throughput
- Pipeline execution

## Why Fixed Point?

Floating point arithmetic requires significant FPGA resources.

Fixed point provides:

- Lower resource utilization
- Faster computation
- Efficient DSP usage

## Why Sigmoid LUT?

The exponential operation is expensive.

A lookup table provides:

- Constant latency
- Reduced hardware complexity
- Predictable implementation

## Why Zynq?

Zynq combines:

- ARM processor
- FPGA fabric

allowing hardware/software co-design.
```
