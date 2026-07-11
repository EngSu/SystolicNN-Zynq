# Verification Strategy

## Verification Flow

Floating Point C Model

↓

Floating Point C++ Model

↓

Fixed Point C++ Model

↓

RTL Simulation

↓

FPGA Hardware

## Comparison

For each input image:

Compare:

- Hidden layer outputs
- Output layer values
- Final classification

## Acceptance Criteria

Classification:

Must match reference model.

Fixed point:

Allow small numerical error due to quantization.
