<p align="center"><img src="https://github.com/moffenzeefmodular/VCVrack-submission/blob/main/doc/images/BusMultScreenshot.png" height="638"></p>

---

# Bus Mult VCV Rack User Manual

---

# Overview

**Bus Mult** is a passive-style buffered multiple with two independent inputs and five outputs. IN1 fans out to the first three outputs. IN2 fans out to the last two. If IN2 is left unpatched, it normalizes to IN1, giving you five copies of a single signal.

Place multiple Bus Mults directly adjacent to one another and the normalization chain continues automatically — the last output value of the left module feeds into IN1 of the right module when IN1 is unpatched. This lets you build an arbitrarily long multiple chain across a row of modules without running a single patch cable between them.

---

# CV Ranges

All inputs and outputs pass signals at their original voltage. No scaling is applied.

---

# Controls

Bus Mult has no knobs or switches.

---

# I/O

### Inputs

* **IN1:** The primary input. Its signal is sent to OUT1, OUT2, and OUT3. When a Bus Mult is placed immediately to the left and IN1 is unpatched, it inherits the chained voltage from that neighbor automatically.

* **IN2:** The secondary input. Its signal is sent to OUT4 and OUT5. When unpatched, IN2 normalizes to IN1, so all five outputs carry the same signal.

### Outputs

* **OUT1 / OUT2 / OUT3:** Copies of IN1.

* **OUT4 / OUT5:** Copies of IN2 (or IN1 if IN2 is unpatched).

---

# Chaining

When two or more Bus Mult modules are placed side by side with no gap, they chain automatically. The rightmost output value of the left module (the IN2 / OUT4–5 bus) passes silently into the right module's IN1 when IN1 is unpatched. You can extend a single signal across as many Bus Mults as you need without any patch cables connecting them.

To break the chain on a specific module, simply patch a cable into its IN1.

---
