# MarkovChip
A Multi-Step Markov Transition Matrix IP Block

###### Custom Area-Optimized AI Accelerator Designed for Chipathon (GF180MCU)

## 📖 Theory and Architecture

This project implements a **16-State 12-bit Stochastic Multi-Step Markov Transition Matrix Accelerator**. The underlying theory and design principles are broken down as follows:

### 1. Markov Chain & Multi-Step Transitions
A Markov Chain represents a system where the probability of transitioning to the next state depends only on the current state. These transition probabilities are captured in a Transition Matrix ($A$). To find the state probabilities after $r$ steps, we multiply the initial state probability vector ($B_0$) by the matrix $A$ raised to the power of $r$:

$$ B_r = B_0 \times A^r $$

The MarkovChip natively computes this multi-step matrix multiplication in hardware for a 16-state system.

### 2. Stochastic Computing (Area Optimization)
Traditional digital implementations of matrix multiplication using floating-point or fixed-point arithmetic require large multiplier circuits, consuming significant chip area. Instead, this accelerator uses **Stochastic Computing**.

In stochastic computing, continuous probabilities (e.g., values between 0 and 1) are represented as streams of random bits. For instance, a probability of $0.75$ is represented by a bitstream where $75\%$ of the bits are `1`. 

Multiplication of two independent probabilities ($X$ and $Y$) is achieved using a simple **AND gate**:

$$ E[X \cdot Y] = P(X=1) \times P(Y=1) $$

By replacing large multipliers with basic logic gates, the chip achieves a highly **Area-Optimized** footprint.

### 3. LFSR & Whitening Layer
To generate the random bitstreams, the design utilizes 12-bit **Linear Feedback Shift Registers (LFSR)**. A known issue with raw LFSR sequences is correlation, which compounds errors over multi-step Markov iterations. 

To mitigate this, the chip includes a **Whitening/Scrambling Layer** that decorrelates the LFSR outputs. This layer ensures that the stochastic streams remain independent, keeping the error rate extremely low even across 20+ steps.

### 4. Feedback Loop
For multi-step computations, the chip features an integrated feedback loop. The output bitstreams of one iteration (step) are fed back directly as the input probabilities for the next step. This allows the accelerator to autonomously compute deep transition probabilities (e.g., $A^{10}$ or $A^{20}$) without host CPU intervention.

## ⚙️ Gate-Level Synthesis

To synthesize the design to gate-level using **Yosys** with the GF180MCU PDK, follow these commands. Ensure you have Yosys installed and the `gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib` liberty file available in the root project directory.

Run `yosys` in your terminal to open the Yosys prompt, then execute the following:

```tcl
# 1. Read all required RTL files
read_verilog RTL/Adder_Tree_16in.v \
             RTL/Bit_Generator_Circuit.v \
             RTL/Comparator.v \
             RTL/counter.v \
             RTL/LFSR.v \
             RTL/LFSR_Enabler.v \
             RTL/Markov_Chain_Accelerator.v \
             RTL/memory_enable_decoder.v \
             RTL/priority_encoder.v \
             RTL/Result_Shift_Memory.v \
             RTL/Row_Col_BGC.v \
             RTL/Shift_Register.v \
             RTL/Stochastic_Timer.v

# 2. Read the GF180MCU standard cell liberty file
read_liberty -lib gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib

# 3. Synthesize the top module (flattening the hierarchy)
synth -top Markov_Chain_Accelerator -flatten

# 4. Map to the technology library using ABC
abc -liberty gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib
clean

# 5. Write the output gate-level netlist
write_verilog Netlist_Markov_Chain_Accelerator.v
```

## 👥 Team Members

| Name | Department | Institution | GitHub |
| :--- | :---: | :---: | :---: |
| **Mushfiq Raiyun** | CSE | BUET | [@mqrknoetonoob](https://github.com/mqrknoetonoob) |
| **Diganto Saha** | EEE | BUET | [@byte-sized-me](https://github.com/byte-sized-me) |
| **Arnob Pal** | EEE | BUET | [@LogicForgeSys](https://github.com/LogicForgeSys) |
| **Sheikh Rahiqul Islam Swapnil** | CSE | BUET | [@DaechwitaWhyTho](https://github.com/DaechwitaWhyTho) |
| **Sajid Ibne Zaman** | CSE | BUET | [@TheGameTripleS](https://github.com/TheGameTripleS) |
