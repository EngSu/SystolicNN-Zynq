# Accelerator Controller FSM

## 1. Purpose

The accelerator controller is responsible for controlling the complete neural network inference sequence.

The controller coordinates:

- Input loading
- Weight loading
- Hidden layer computation
- Sigmoid activation
- Output layer computation
- Prediction generation
- Completion signaling

The same FSM sequence will be implemented in:

- C behavioral simulator
- Verilog RTL controller

---

# 2. Accelerator Execution Flow

The complete inference operation:

START

|

v

LOAD INPUT IMAGE

|

v

LOAD HIDDEN WEIGHTS

|

v

HIDDEN LAYER COMPUTATION

|

v

SIGMOID ACTIVATION

|

v

LOAD OUTPUT WEIGHTS

|

v

OUTPUT LAYER COMPUTATION

|

v

SOFTMAX

|

v

PREDICTION

|

v

DONE

---

# 3. FSM States

The controller contains the following states:

| State              | Description                               |
| ------------------ | ----------------------------------------- |
| RESET              | Initialize accelerator registers          |
| IDLE               | Wait for start command                    |
| LOAD_INPUT         | Load 784 input pixels                     |
| LOAD_HIDDEN_WEIGHT | Load hidden layer weights                 |
| HIDDEN_MAC         | Execute hidden layer systolic computation |
| SIGMOID            | Apply sigmoid LUT                         |
| LOAD_OUTPUT_WEIGHT | Load output layer weights                 |
| OUTPUT_MAC         | Execute output layer systolic computation |
| SOFTMAX            | Calculate output probabilities            |
| PREDICT            | Select maximum probability                |
| DONE               | Signal completion                         |

---

# 4. State Transition Diagram

          RESET

            |

            v


          IDLE

            |

         start=1

            |

            v


      LOAD_INPUT

            |

            v

LOAD_HIDDEN_WEIGHT

            |

            v


      HIDDEN_MAC

            |

            v


        SIGMOID

            |

            v

LOAD_OUTPUT_WEIGHT

            |

            v


      OUTPUT_MAC

            |

            v


        SOFTMAX

            |

            v


       PREDICT

            |

            v


         DONE

            |

            v


         IDLE

---

# 5. State Details

## RESET

Actions:

- Clear accumulators
- Reset PE registers
- Reset counters
- Clear status flags

---

## IDLE

The accelerator waits for:

start = 1

When start is received:

state <= LOAD_INPUT

---

## LOAD_INPUT

Input:

784 pixel values

Stored in:

Input Buffer

---

## LOAD_HIDDEN_WEIGHT

Load:

784 x 2 hidden layer weights

Memory organization:

Weight Memory
|
|
+--> PE0 weights
|
+--> PE1 weights

---

## HIDDEN_MAC

Operation:

hidden_accumulator[j] += input[i] \* weight[i][j]

j = 0,1

i = 0...783

Latency:

784 cycles

---

## SIGMOID

The hidden layer produces:

hidden_pre_activation[0]

hidden_pre_activation[1]

The LUT converts:

sigmoid(x)

Output:

hidden_activation[0]

hidden_activation[1]

---

## OUTPUT_MAC

Operation:

output[k] += hidden[i] \* weight[i][k]

i = 0,1

k = 0,1

Latency:

2 cycles

---

## SOFTMAX

Convert output scores into probabilities:

class_probability[0]

class_probability[1]

---

## PREDICT

Select:

prediction =
argmax(probability)

---

## DONE

The controller asserts:

done = 1

The ARM processor can read the result through AXI.
