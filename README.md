# SystolicNN-Zynq

## FPGA-Based Feed-Forward Neural Network Accelerator Using a Systolic Array on Zynq SoC

## Overview

SystolicNN-Zynq is an FPGA-based hardware accelerator designed for neural network inference on the Xilinx Zynq-7000 SoC.

The project implements a feed-forward neural network classifier using:

- Custom RTL hardware accelerator
- Systolic array architecture
- Fixed-point arithmetic
- AXI communication
- Embedded Linux driver
- C++ software simulation

## Target Hardware

Board:

ZedBoard

FPGA:

XC7Z020-CLG484-1

## Neural Network

Topology:

784 → 2 → 2

Input:

Normalized MNIST pixels

Range:

0-1

Hidden Layer:

2 neurons

Activation:

Sigmoid LUT

Output Layer:

2 neurons

Activation:

Softmax during training

Argmax implemented in hardware

## Development Flow

Floating-point model

↓

Fixed-point model

↓

C++ cycle-accurate simulator

↓

RTL accelerator

↓

Zynq SoC integration

↓

Linux driver

↓

Performance evaluation

## Status

Phase 0: Project Planning

Completed
