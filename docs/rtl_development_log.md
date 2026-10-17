# RTL Development Log

## October 17, 2026 — Q1.15 Multiplier

### Objective
Implement a signed 16-bit fixed-point multiplier
for the SystolicNN-Zynq MAC datapath.

### Design Specification
- Input A: signed 16-bit Q1.15
- Input B: signed 16-bit Q1.15
- Product: signed 32-bit with 30 fractional bits
- Architecture: combinational multiplier

### Implementation
Status: In progress

### Verification
Status: Not run

Planned test cases:
- Positive × Positive
- Positive × Negative
- Negative × Negative
- Multiplication by zero
- Minimum and maximum representable operands

### Issues and Decisions
To be completed after simulation.

### Next Step
Develop a self-checking SystemVerilog testbench.

## Verification Update

- Testbench: tb_multiplier_q15.sv
- Simulator: Vivado XSim
- Result: [PASS or FAIL]
- Observed issues: [describe findings]
- Changes made: [describe fixes]

## Q1.15 Multiplier Verification

Module: rtl/mac/multiplier_q15.v
Testbench: tb/mac/tb_multiplier_q15.sv
Simulator: Vivado XSim

Design:
- 16-bit signed operands
- 32-bit signed product
- Combinational arithmetic

Verification:
- Seven directed test cases
- Positive and negative multiplication
- Zero multiplication
- Signed boundary conditions

Actual result: PASS
