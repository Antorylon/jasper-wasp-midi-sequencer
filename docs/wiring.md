# Wiring

This document describes the current prototype wiring used by the firmware.

## Controller

- Arduino Pro Micro
- ATmega32U4
- 5 V / 16 MHz

## Jasper LINK

All LINK signal wires use approximately **10 kΩ series resistors**.

| Pro Micro | Jasper LINK |
|---|---|
| D2 | T |
| D3 | A |
| D4 | B |
| D5 | C |
| D6 | D |
| D7 | E |
| D8 | F |
| GND | GND |

The firmware deliberately switches these pins to INPUT with no internal pull-ups whenever the interface is not actively driving LINK.

## DIN MIDI IN

The tested input circuit uses a **6N137**.

Current wiring:

- 6N137 pin 8 -> +5 V
- 6N137 pin 5 -> GND
- 100 nF decoupling capacitor between pins 8 and 5
- 6N137 pin 6 -> Pro Micro D0 / RX1
- 10 kΩ pull-up from pin 6 to +5 V
- DIN pin 4 -> 220 Ω -> 6N137 pin 2
- 6N137 pin 3 -> DIN pin 5
- reverse-protection 1N4148 across the optocoupler LED:
  - cathode to pin 2
  - anode to pin 3

**Important:** in the tested build, 6N137 pin 7 is not tied to ground.

The MIDI input side remains galvanically isolated from the synth/controller side through the optocoupler.

## DIN MIDI OUT / THRU

Current 5 V output wiring:

- +5 V -> 220 Ω -> DIN pin 4
- Pro Micro D1 / TX1 -> 220 Ω -> DIN pin 5
- GND -> DIN pin 2

D1 carries both:

- software THRU for data arriving on DIN MIDI IN
- USB MIDI converted to the physical 31.25 kbaud DIN MIDI stream

## Clock LEDs

- D9 -> internal-clock LED
- D16 -> external-clock LED

The firmware assumes separate active-high LED channels. If a different two-colour LED topology is used, the polarity or drive logic may need to be changed.

## Buzzer

- D10 -> passive piezo buzzer

The buzzer is used only for UI feedback, metronome/count-in and status cues.

## SET input

- A0 -> SET sensor/button

Current firmware configuration:

```cpp
const bool SET_ACTIVE_LEVEL = HIGH;
pinMode(PIN_SET, INPUT);
```

This matches the current capacitive-touch SET sensor.

If SET is later replaced with a mechanical button to ground, it is preferable to change the firmware to active LOW and use `INPUT_PULLUP`.

## Power

The controller and MIDI circuitry require a clean regulated 5 V supply.

The current hardware experiment uses 12 V available inside the synth and a linear 5 V regulator. When powering the Pro Micro externally, take care not to unintentionally back-feed the USB 5 V rail.

During development, verify the actual Pro Micro clone's USB/VCC power topology before simultaneously connecting external 5 V power and USB.

## Decoupling

Recommended minimum local decoupling:

- 100 nF close to the Pro Micro supply
- 100 nF close to the 6N137 supply
- bulk capacitance on the 5 V rail as appropriate

## Safety note

The Jasper LINK interface is part of the synth's keyboard/control circuitry. Verify all wiring with the synth powered off before connecting the interface.
