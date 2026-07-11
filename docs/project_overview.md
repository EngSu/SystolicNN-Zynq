# Project Overview

## Project Name

SystolicNN-Zynq

## Objective

Design and implement an FPGA-based accelerator for feed-forward neural network inference using a systolic array architecture on a Xilinx Zynq-7000 SoC.

The objective is to demonstrate complete hardware/software co-design including:

- Neural network inference
- Fixed-point conversion
- Hardware acceleration
- FPGA implementation
- SoC communication
- Embedded Linux control

## Motivation

Neural network inference requires intensive matrix multiplication operations.

FPGA accelerators can improve performance by exploiting:

- Parallel computation
- Pipelining
- Dedicated hardware resources

This project explores the design of a custom neural network accelerator from algorithm level to deployed hardware.

## Scope

The project focuses on inference acceleration.

Training is performed externally and trained parameters are provided as floating-point weights and biases.

## Responsibilities

Hardware/software implementation:

- C++ reference model
- Fixed-point conversion
- Hardware architecture
- RTL implementation
- Verification
- AXI integration
- Linux driver
- Performance evaluation
