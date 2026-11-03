# SystolicNN-Zynq — RTL Vector Export Verification

Date: November 2, 2026

## Objective
Generate reproducible integer reference vectors for
SystemVerilog verification of the FPGA accelerator.

## Configuration
- Neural network: 784-2-2
- Input and weight format: Q1.15
- Hidden pre-activation format: Q4.12
- Sigmoid: 32-entry lookup table
- Accumulation: signed 64-bit, 30 fractional bits

## Results
- Images exported: 2115
- Input Q1.15 saturations: 12678
- Inputs equal to 1.0: 12678
- Result: PASS

## Conclusion
All observed input saturation events are attributable
to exact positive 1.0 values.

## Next Step
Validate generated ROM file lengths and use the vectors
to verify the multiplier, MAC, and two-PE RTL.
