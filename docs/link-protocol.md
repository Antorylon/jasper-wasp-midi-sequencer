# Jasper / Wasp LINK notes

This document records the protocol assumptions used by the firmware.

## Electrical level

LINK uses approximately **5 V TTL logic**.

The interface uses 10 kΩ series resistors between the Pro Micro and the Jasper LINK signal lines.

When the MCU is not actively driving LINK, the firmware places T and A–F in high-impedance INPUT mode with internal pull-ups disabled.

## Signals

The firmware uses:

- T — repeating keyboard frame / trigger signal
- A–D — note encoding
- E–F — octave / upper note-code information

The observed Jasper frame is approximately 19 ms long.

The generated timing used by the firmware is approximately:

- T HIGH: 13 ms
- T LOW: 6 ms

## Note code mapping

The firmware uses the following MIDI-note-to-LINK-code table:

```text
MIDI 36..47 -> 48,43,42,41,40,39,38,37,36,35,34,33
MIDI 48..59 -> 32,27,26,25,24,23,22,21,20,19,18,17
MIDI 60..72 -> 16,11,10,9,8,7,6,5,4,3,2,1,0
```

MIDI note 36 is intentionally ignored in normal operation because the lowest C has a hardware decoding limitation in the Wasp/Jasper arrangement.

## Keyboard coexistence

LINK is monophonic and is not designed for two independent active drivers at the same time.

For this reason the firmware:

- drives LINK only when required
- releases LINK back to INPUT / high impedance when idle
- listens to the native Jasper keyboard only in modes where the MCU is not simultaneously driving the bus

## Envelope retrigger behavior

The Wasp/Jasper envelope circuitry may fail to retrigger if consecutive notes are too tightly spaced. This is also observable from the native keyboard.

The sequencer therefore requests note release early enough to leave a short retrigger gap before the next note.

## Safe LINK release

If the firmware needs to release LINK while T is already LOW, it can release immediately.

If T is HIGH, release is postponed until the next HIGH-to-LOW transition. This avoids truncating the active frame and substantially improves reliable sequence playback.

## Scope

This is an engineering note for the firmware implementation, not a complete reverse-engineered specification of every Wasp/Jasper LINK signal behavior.
