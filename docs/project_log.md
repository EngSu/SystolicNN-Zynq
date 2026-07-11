# Project Development Log


## July 2026


## Project Started


Project:

SystolicNN-Zynq


Goal:

Develop FPGA accelerated FFNN inference engine on Zynq SoC.


Hardware:

ZedBoard

FPGA:

XC7Z020-CLG484-1


Tool:

Vivado 2025


Network:

784 → 2 → 2


Input:

Normalized MNIST pixels

Range:

0-1


Weights:

Floating point

Range:

-1 to +1


Hidden Activation:

Sigmoid LUT


Development Approach:

Floating-point model

↓

Fixed-point model

↓

C++ simulator

↓

RTL

↓

SoC integration

↓

Linux driver

