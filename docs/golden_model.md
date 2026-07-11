# Floating Point Golden Reference Model

## Purpose

The golden model provides the software reference implementation used to verify all future hardware implementations.

The RTL accelerator output must match this model within fixed-point quantization tolerance.

## Network Architecture

Input:

784 features

Hidden Layer:

2 neurons

Activation:

Sigmoid

Output Layer:

2 neurons

Activation:

Softmax

Topology:

784 → 2 → 2

## Computation

Hidden layer:

H = X × W_hidden

Activation:

H = sigmoid(H)

Output layer:

Y = H × W_output

Classification:

Prediction = argmax(Y)

## Reference Implementation

Language:

C

The original C implementation performs:

- Weight loading
- Input loading
- Matrix multiplication
- Sigmoid activation
- Softmax calculation
- Output verification

## Data Format

Input:

Floating point

Range:

0 to 1

Weights:

Floating point

Range:

-1 to +1
