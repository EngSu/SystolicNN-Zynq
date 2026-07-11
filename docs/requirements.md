# System Requirements

## Functional Requirements

The system shall:

1. Accept a normalized MNIST image input.

2. Execute neural network inference.

3. Compute hidden layer MAC operations.

4. Apply sigmoid activation.

5. Compute output layer.

6. Return predicted class.

7. Allow ARM processor control through Linux.

---

# Hardware Requirements

Target:

ZedBoard

FPGA:

XC7Z020-CLG484-1

Development Tool:

Vivado 2025

---

# Neural Network Requirements

Topology:

784 input neurons

↓

2 hidden neurons

↓

2 output neurons

Input range:

0-1

Weight range:

-1 to +1

Activation:

Hidden:

Sigmoid LUT

Output:

Argmax classification
