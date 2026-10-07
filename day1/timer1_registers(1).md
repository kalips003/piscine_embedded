# ATmega328P – Timer1 Register Reference

---

## TCCR1A – Timer/Counter1 Control Register A (8-bit)

| Bit  | 7      | 6      | 5      | 4      | 3 | 2 | 1     | 0     |
|------|--------|--------|--------|--------|---|---|-------|-------|
| Name | COM1A1 | COM1A0 | COM1B1 | COM1B0 | – | – | WGM11 | WGM10 |

- **COM1A1, COM1A0** (TCCR1A bits 7–6): what pin OC1A (PB1) does on events → see *COM1A tables*
- **COM1B1, COM1B0** (TCCR1A bits 5–4): what pin OC1B (PB2) does on events → see *COM1B tables*
- **WGM11, WGM10** (TCCR1A bits 1–0): low half of the mode → see *WGM table*

---

## TCCR1B – Timer/Counter1 Control Register B (8-bit)

| Bit  | 7     | 6     | 5 | 4     | 3     | 2    | 1    | 0    |
|------|-------|-------|---|-------|-------|------|------|------|
| Name | ICNC1 | ICES1 | – | WGM13 | WGM12 | CS12 | CS11 | CS10 |

- **ICNC1** (TCCR1B bit 7): input capture noise canceler → see *Input capture bits*
- **ICES1** (TCCR1B bit 6): input capture edge select → see *Input capture bits*
- **WGM13, WGM12** (TCCR1B bits 4–3): high half of the mode → see *WGM table*
- **CS12, CS11, CS10** (TCCR1B bits 2–0): clock source / prescaler (non-zero starts the timer) → see *Clock Select table*

> The 4 WGM bits are split across two registers:
> WGM13, WGM12 → TCCR1B (bits 4, 3) · WGM11, WGM10 → TCCR1A (bits 1, 0).
> Together they form the 4-bit mode number in the *WGM table*.

---

## TCCR1C – Timer/Counter1 Control Register C (8-bit)

| Bit  | 7     | 6     | 5 | 4 | 3 | 2 | 1 | 0 |
|------|-------|-------|---|---|---|---|---|---|
| Name | FOC1A | FOC1B | – | – | – | – | – | – |

- **FOC1A / FOC1B** (TCCR1C bits 7 / 6): writing 1 forces an immediate compare match on OC1A / OC1B (pin acts per COM bits, no flag, no interrupt, no counter reset). Only works in non-PWM modes.

---

## WGM table – Waveform Generation Mode

Bits: **WGM13** (TCCR1B bit 4) · **WGM12** (TCCR1B bit 3) · **WGM11** (TCCR1A bit 1) · **WGM10** (TCCR1A bit 0)

| Mode | WGM13 | WGM12 | WGM11 | WGM10 | Mode of operation                    | TOP    | OCR1x updated at | TOV1 set at |
|------|-------|-------|-------|-------|--------------------------------------|--------|------------------|-------------|
| 0    | 0     | 0     | 0     | 0     | Normal                               | 0xFFFF | Immediate        | MAX         |
| 1    | 0     | 0     | 0     | 1     | PWM, Phase Correct, 8-bit            | 0x00FF | TOP              | BOTTOM      |
| 2    | 0     | 0     | 1     | 0     | PWM, Phase Correct, 9-bit            | 0x01FF | TOP              | BOTTOM      |
| 3    | 0     | 0     | 1     | 1     | PWM, Phase Correct, 10-bit           | 0x03FF | TOP              | BOTTOM      |
| 4    | 0     | 1     | 0     | 0     | CTC                                  | OCR1A  | Immediate        | MAX         |
| 5    | 0     | 1     | 0     | 1     | Fast PWM, 8-bit                      | 0x00FF | BOTTOM           | TOP         |
| 6    | 0     | 1     | 1     | 0     | Fast PWM, 9-bit                      | 0x01FF | BOTTOM           | TOP         |
| 7    | 0     | 1     | 1     | 1     | Fast PWM, 10-bit                     | 0x03FF | BOTTOM           | TOP         |
| 8    | 1     | 0     | 0     | 0     | PWM, Phase and Frequency Correct     | ICR1   | BOTTOM           | BOTTOM      |
| 9    | 1     | 0     | 0     | 1     | PWM, Phase and Frequency Correct     | OCR1A  | BOTTOM           | BOTTOM      |
| 10   | 1     | 0     | 1     | 0     | PWM, Phase Correct                   | ICR1   | TOP              | BOTTOM      |
| 11   | 1     | 0     | 1     | 1     | PWM, Phase Correct                   | OCR1A  | TOP              | BOTTOM      |
| 12   | 1     | 1     | 0     | 0     | CTC                                  | ICR1   | Immediate        | MAX         |
| 13   | 1     | 1     | 0     | 1     | (Reserved)                           | –      | –                | –           |
| 14   | 1     | 1     | 1     | 0     | Fast PWM                             | ICR1   | BOTTOM           | TOP         |
| 15   | 1     | 1     | 1     | 1     | Fast PWM                             | OCR1A  | BOTTOM           | TOP         |

- **MAX** = 0xFFFF (65535), the highest value the counter can hold.
- **BOTTOM** = 0.
- **TOP** = end of the counting cycle (depends on the mode, see column).
- **"OCR1x updated at"**: in PWM modes, a new value written to OCR1A/OCR1B is buffered and only takes effect at that moment (avoids glitches).
- **TOV1** = overflow flag (TIFR1 bit 0).

---

## Clock Select – CS12:CS10 (TCCR1B bits 2–0)

| CS12 | CS11 | CS10 | Clock source                        |
|------|------|------|-------------------------------------|
| 0    | 0    | 0    | No clock (timer stopped)            |
| 0    | 0    | 1    | clk ÷ 1                             |
| 0    | 1    | 0    | clk ÷ 8                             |
| 0    | 1    | 1    | clk ÷ 64                            |
| 1    | 0    | 0    | clk ÷ 256                           |
| 1    | 0    | 1    | clk ÷ 1024                          |
| 1    | 1    | 0    | External pin T1 (PD5), falling edge |
| 1    | 1    | 1    | External pin T1 (PD5), rising edge  |

---

## COM1A tables – pin OC1A (PB1)

Bits: **COM1A1** (TCCR1A bit 7) · **COM1A0** (TCCR1A bit 6)
The meaning depends on the mode (WGM table).

### Non-PWM modes (Normal, CTC: modes 0, 4, 12)

| COM1A1 | COM1A0 | On compare match (TCNT1 == OCR1A), OC1A…       |
|--------|--------|------------------------------------------------|
| 0      | 0      | Disconnected – PB1 is a normal I/O pin         |
| 0      | 1      | Toggles                                        |
| 1      | 0      | Cleared (LOW)                                  |
| 1      | 1      | Set (HIGH)                                     |

### Fast PWM modes (5, 6, 7, 14, 15)

| COM1A1 | COM1A0 | OC1A behavior                                                                 |
|--------|--------|-------------------------------------------------------------------------------|
| 0      | 0      | Disconnected – PB1 is a normal I/O pin                                        |
| 0      | 1      | Modes 14, 15: toggle on compare match. Other modes: disconnected              |
| 1      | 0      | Clear (LOW) on compare match, set (HIGH) at BOTTOM → **non-inverting**        |
| 1      | 1      | Set (HIGH) on compare match, clear (LOW) at BOTTOM → **inverting**            |

### Phase Correct / Phase and Frequency Correct PWM modes (1, 2, 3, 8, 9, 10, 11)

| COM1A1 | COM1A0 | OC1A behavior                                                                  |
|--------|--------|--------------------------------------------------------------------------------|
| 0      | 0      | Disconnected – PB1 is a normal I/O pin                                         |
| 0      | 1      | Modes 9, 11: toggle on compare match. Other modes: disconnected                |
| 1      | 0      | Clear (LOW) on match while counting up, set (HIGH) on match while counting down |
| 1      | 1      | Set (HIGH) on match while counting up, clear (LOW) on match while counting down |

---

## COM1B tables – pin OC1B (PB2)

Bits: **COM1B1** (TCCR1A bit 5) · **COM1B0** (TCCR1A bit 4)
Same as COM1A, but compares against **OCR1B** and drives **OC1B (PB2)**.
One difference: the "toggle" option (`01`) only exists in non-PWM modes for OC1B.

### Non-PWM modes (Normal, CTC: modes 0, 4, 12)

| COM1B1 | COM1B0 | On compare match (TCNT1 == OCR1B), OC1B…       |
|--------|--------|------------------------------------------------|
| 0      | 0      | Disconnected – PB2 is a normal I/O pin         |
| 0      | 1      | Toggles                                        |
| 1      | 0      | Cleared (LOW)                                  |
| 1      | 1      | Set (HIGH)                                     |

### Fast PWM modes (5, 6, 7, 14, 15)

| COM1B1 | COM1B0 | OC1B behavior                                                          |
|--------|--------|------------------------------------------------------------------------|
| 0      | 0      | Disconnected – PB2 is a normal I/O pin                                 |
| 0      | 1      | Disconnected (no toggle for OC1B in PWM)                               |
| 1      | 0      | Clear (LOW) on compare match, set (HIGH) at BOTTOM → **non-inverting** |
| 1      | 1      | Set (HIGH) on compare match, clear (LOW) at BOTTOM → **inverting**     |

### Phase Correct / Phase and Frequency Correct PWM modes (1, 2, 3, 8, 9, 10, 11)

| COM1B1 | COM1B0 | OC1B behavior                                                                   |
|--------|--------|---------------------------------------------------------------------------------|
| 0      | 0      | Disconnected – PB2 is a normal I/O pin                                          |
| 0      | 1      | Disconnected (no toggle for OC1B in PWM)                                        |
| 1      | 0      | Clear (LOW) on match while counting up, set (HIGH) on match while counting down |
| 1      | 1      | Set (HIGH) on match while counting up, clear (LOW) on match while counting down |

> Whatever the COM setting, the pin must be configured as output in DDRB
> (DDRB bit 1 for OC1A/PB1, DDRB bit 2 for OC1B/PB2), or nothing comes out.

---

## Input capture bits (TCCR1B bits 7–6)

| Bit                    | Value | Meaning                                                                 |
|------------------------|-------|-------------------------------------------------------------------------|
| **ICNC1** (TCCR1B bit 7) | 0   | Noise canceler off                                                      |
|                        | 1     | Noise canceler on: the ICP1 pin (PB0) must be stable for 4 samples (adds 4 clock cycles of delay) |
| **ICES1** (TCCR1B bit 6) | 0   | Capture on **falling** edge of ICP1 (PB0)                               |
|                        | 1     | Capture on **rising** edge of ICP1 (PB0)                                |

On a capture: TCNT1 is copied into ICR1, and flag ICF1 (TIFR1 bit 5) is set.

---

## 16-bit value registers

| Register  | Role                                   |
|-----------|----------------------------------------|
| **TCNT1** | The counter itself                     |
| **OCR1A** | Compare value A (can also be TOP)      |
| **OCR1B** | Compare value B                        |
| **ICR1**  | Input capture value (can also be TOP)  |

---

## TIMSK1 – Interrupt enable (8-bit)

| Bit  | 7 | 6 | 5     | 4 | 3 | 2      | 1      | 0     |
|------|---|---|-------|---|---|--------|--------|-------|
| Name | – | – | ICIE1 | – | – | OCIE1B | OCIE1A | TOIE1 |

| Bit                      | When set to 1, enables the interrupt for… | ISR name in C          |
|--------------------------|-------------------------------------------|------------------------|
| **ICIE1** (TIMSK1 bit 5)  | Input capture                             | `TIMER1_CAPT_vect`     |
| **OCIE1B** (TIMSK1 bit 2) | Compare match B (TCNT1 == OCR1B)          | `TIMER1_COMPB_vect`    |
| **OCIE1A** (TIMSK1 bit 1) | Compare match A (TCNT1 == OCR1A)          | `TIMER1_COMPA_vect`    |
| **TOIE1** (TIMSK1 bit 0)  | Overflow                                  | `TIMER1_OVF_vect`      |

Also requires `sei()` to enable interrupts globally.

---

## TIFR1 – Interrupt flags (8-bit)

| Bit  | 7 | 6 | 5    | 4 | 3 | 2     | 1     | 0    |
|------|---|---|------|---|---|-------|-------|------|
| Name | – | – | ICF1 | – | – | OCF1B | OCF1A | TOV1 |

| Bit                    | Set to 1 by hardware when…                  |
|------------------------|---------------------------------------------|
| **ICF1** (TIFR1 bit 5)  | An input capture happens                    |
| **OCF1B** (TIFR1 bit 2) | TCNT1 == OCR1B                              |
| **OCF1A** (TIFR1 bit 1) | TCNT1 == OCR1A                              |
| **TOV1** (TIFR1 bit 0)  | Overflow (moment depends on mode, see WGM table) |

Flags are set even if the interrupt is not enabled.
Cleared by **writing 1** to the bit, or automatically when the ISR runs.

---

## Pins

| Chip pin | Timer function          | Configure with      |
|----------|-------------------------|---------------------|
| PB1      | OC1A (compare output A) | DDRB bit 1 = 1 (output) |
| PB2      | OC1B (compare output B) | DDRB bit 2 = 1 (output) |
| PB0      | ICP1 (input capture)    | DDRB bit 0 = 0 (input)  |
| PD5      | T1 (external clock)     | DDRD bit 5 = 0 (input)  |

---

## Formula

```
OCR1A (or ICR1) = (F_CPU / prescaler) × time − 1
```
