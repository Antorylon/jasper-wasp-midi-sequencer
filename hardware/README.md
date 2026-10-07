# Hardware

The current prototype is built around an **Arduino Pro Micro (ATmega32U4, 5 V / 16 MHz)** and connects to Jasper through the LINK header.

## Functional blocks

1. Jasper LINK interface
2. DIN MIDI IN with 6N137 isolation
3. DIN MIDI OUT / software THRU
4. SET control input
5. piezo buzzer
6. internal/external clock indicators
7. regulated 5 V supply

## Suggested prototype BOM

- 1 × Arduino Pro Micro, 5 V / 16 MHz
- 1 × 6N137
- 1 × 5-pin DIN socket for MIDI IN
- 1 × 5-pin DIN socket for MIDI OUT
- 7 × approximately 10 kΩ resistors for LINK signal protection/interface
- 2 × 220 Ω resistors for MIDI OUT
- 1 × 220 Ω resistor for MIDI IN LED current limiting
- 1 × 10 kΩ pull-up for 6N137 output
- 1 × 1N4148 for reverse protection across 6N137 input LED
- 100 nF ceramic decoupling capacitors
- bulk electrolytic capacitors for the local 5 V supply as required
- 1 × passive piezo buzzer
- 1 × SET switch or touch-sensor module
- clock indicator LED(s) with appropriate current-limiting resistors
- optional 5 V linear regulator and heatsink when powering from the synth's 12 V rail

## Power

The prototype has been tested around a local regulated 5 V rail. If deriving 5 V from Jasper's internal 12 V supply with a linear regulator, thermal dissipation must be considered.

Also verify the exact USB/VCC topology of the Pro Micro clone before using USB while externally powering the board. Avoid back-feeding the host computer's USB 5 V rail.

## LINK protection

The prototype uses approximately 10 kΩ series resistors on T and A–F. These are intentionally retained even though the MCU operates at the same nominal 5 V logic level.

The firmware's default idle condition is high impedance, not a driven logic level.

## Status

This is currently prototype documentation, not a production-ready PCB design. Verify the wiring against [../docs/wiring.md](../docs/wiring.md) before building.
