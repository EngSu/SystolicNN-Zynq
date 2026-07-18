# Accelerator Dataflow

## 1. Complete Neural Network Inference Flow

             MNIST Image

          784 input pixels


                |
                v


          Input Buffer


                |
                |
                v

================================================

          Hidden Layer Accelerator

================================================

Input Vector:

X[0:783]

Weight Matrix:

W1[784][2]

Computation:

hidden_pre_activation =
X × W1

Hardware:

          Input Stream

               |

    +----------+----------+

    |                     |

    v                     v

PE0 PE1

Neuron 0 Neuron 1

               |

               v

hidden_pre_activation[2]

================================================

                |
                v


         Sigmoid LUT


                |
                v


    hidden_activation[2]

================================================

          Output Layer Accelerator

================================================

Input:

hidden_activation[2]

Weight Matrix:

W2[2][2]

Hardware:

          Hidden Stream

                |

    +-----------+-----------+

    |                       |

    v                       v

PE0 PE1

Class 0 Class 1

                |

                v


         output_score[2]

================================================

                |
                v


             Softmax


                |
                v


           Prediction

          Class 0 / Class 1

---

# 2. Layer-by-Layer Data Movement

## Hidden Layer

Input:

784 pixels

Weights:

784 x 2

Processing:

PE0 calculates neuron 0

PE1 calculates neuron 1

Result:

hidden_pre_activation[0]

hidden_pre_activation[1]

---

## Activation Layer

Input:

hidden_pre_activation[2]

Operation:

sigmoid(x)

Output:

hidden_activation[2]

---

## Output Layer

Input:

hidden_activation[2]

Weights:

2 x 2

Processing:

PE0 calculates class 0

PE1 calculates class 1

Output:

output_score[2]

---

## Final Classification

Softmax converts:

output_score[2]

into:

probability[0]

probability[1]

Prediction:

argmax(probability)

---
