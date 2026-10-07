# ATmega328P – Timer1 Register Reference

## TCCR1A – Timer/Counter1 Control Register A (8-bit)

| Bit  | 7      | 6      | 5      | 4      | 3 | 2 | 1     | 0     |
|------|--------|--------|--------|--------|---|---|-------|-------|
| Name | COM1A1 | COM1A0 | COM1B1 | COM1B0 | – | – | WGM11 | WGM10 |

- **COM1A1, COM1A0** (bits 7–6): what pin OC1A (PB1) does on events
- **COM1B1, COM1B0** (bits 5–4): same for pin OC1B (PB2)
- **WGM11, WGM10** (bits 1–0): low half of the mode

## TCCR1B – Timer/Counter1 Control Register B (8-bit)

| Bit  | 7     | 6     | 5 | 4     | 3     | 2    | 1    | 0    |
|------|-------|-------|---|-------|-------|------|------|------|
| Name | ICNC1 | ICES1 | – | WGM13 | WGM12 | CS12 | CS11 | CS10 |

- **ICNC1, ICES1** (bits 7–6): input capture options
- **WGM13, WGM12** (bits 4–3): high half of the mode
- **CS12, CS11, CS10** (bits 2–0): clock source / prescaler (non-zero starts the timer)

> The 4 WGM bits are split across two registers:
> WGM13, WGM12 → TCCR1B (bits 4, 3) · WGM11, WGM10 → TCCR1A (bits 1, 0).
> Together they form the 4-bit mode number in the *Waveform Generation Mode* table.

### Clock Select – CS12:CS10 (TCCR1B, bits 2–0)

| CS12 | CS11 | CS10 | Clock source                     |
|------|------|------|----------------------------------|
| 0    | 0    | 0    | No clock (timer stopped)         |
| 0    | 0    | 1    | clk ÷ 1                          |
| 0    | 1    | 0    | clk ÷ 8                          |
| 0    | 1    | 1    | clk ÷ 64                         |
| 1    | 0    | 0    | clk ÷ 256                        |
| 1    | 0    | 1    | clk ÷ 1024                       |
| 1    | 1    | 0    | External pin T1 (PD5), falling edge |
| 1    | 1    | 1    | External pin T1 (PD5), rising edge  |

### Compare Output Mode, non-PWM – COM1A1:COM1A0 (TCCR1A, bits 7–6)

| COM1A1 | COM1A0 | On compare match, OC1A (PB1)…                 |
|--------|--------|-----------------------------------------------|
| 0      | 0      | Disconnected – PB1 is a normal I/O pin        |
| 0      | 1      | Toggles                                       |
| 1      | 0      | Cleared (LOW)                                 |
| 1      | 1      | Set (HIGH)                                    |

## 16-bit value registers

| Register  | Role                                   |
|-----------|----------------------------------------|
| **TCNT1** | The counter itself                     |
| **OCR1A** | Compare value A (can also be TOP)      |
| **OCR1B** | Compare value B                        |
| **ICR1**  | Input capture value (can also be TOP)  |

## TIMSK1 – Interrupt enable (8-bit)

| Bit  | 5     | 2      | 1      | 0     |
|------|-------|--------|--------|-------|
| Name | ICIE1 | OCIE1B | OCIE1A | TOIE1 |

Enables: input capture · compare B · compare A · overflow interrupts.
Also requires `sei()` to enable interrupts globally.

## TIFR1 – Interrupt flags (8-bit)

| Bit  | 5    | 2     | 1     | 0    |
|------|------|-------|-------|------|
| Name | ICF1 | OCF1B | OCF1A | TOV1 |

Set to 1 by hardware on each event. Cleared by **writing 1** to the bit.

## Pins

| Chip pin | Timer function | Configure as output with |
|----------|----------------|--------------------------|
| PB1      | OC1A           | DDRB, bit 1              |
| PB2      | OC1B           | DDRB, bit 2              |
| PD5      | T1 (ext. clock input) | –                 |

## Formula

```
OCR1A (or ICR1) = (F_CPU / prescaler) × time − 1
```
