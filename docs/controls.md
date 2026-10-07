# Controls

The interface intentionally uses no display. Configuration is performed by holding **SET** and using the Jasper keyboard as a control surface.

## Numeric keys

| Jasper key | Digit |
|---|---:|
| C1 | 0 |
| D1 | 1 |
| E1 | 2 |
| F1 | 3 |
| G1 | 4 |
| A1 | 5 |
| B1 | 6 |
| C2 | 7 |
| D2 | 8 |
| E2 | 9 |

## Main functions

| Jasper key | Function |
|---|---|
| C#1 | MIDI channel setup |
| D#1 | Internal tempo |
| F#1 | Sequencer setup |
| G#1 | Step Record |
| A#1 | Live Record |
| C#2 | Arpeggiator setup |
| C3 | Play saved sequence |

## MIDI channel

`SET -> C#1 -> two digits`

Examples:

- `01` = MIDI channel 1
- `16` = MIDI channel 16

The value is stored only after both digits have been entered.

## Tempo

`SET -> D#1 -> three digits`

Valid range:

- 030–300 BPM

The value is stored only after all three digits have been entered.

## Sequencer setup

Enter with:

`SET -> F#1`

Then choose:

| Digit | Function |
|---:|---|
| 0 | clear / factory reset hold function |
| 1 | 1/4 resolution |
| 2 | 1/8 resolution |
| 3 | 1/16 resolution |
| 4 | 1 bar |
| 5 | 2 bars |
| 6 | 4 bars |
| 7 | toggle Free Live Record |

### Clear sequence

`SET -> F#1 -> hold 0 for at least 1 second, then release`

A short cue is produced at the 1-second threshold. The sequence is actually cleared when the key is released.

### Factory reset

`SET -> F#1 -> hold 0 for at least 5 seconds`

Factory reset occurs at the 5-second threshold and restores the default persistent settings.

## Step Record

`SET -> G#1`

After selecting Step Record, **SET may be released**. Step Record remains active independently.

- short key press = record that note
- hold C3 for >= 700 ms = REST
- after the last required step = automatically save and exit
- press SET before completion = cancel the temporary recording; the previously saved pattern remains unchanged

Active step count depends on resolution and bar length:

| Bars | 1/4 | 1/8 | 1/16 |
|---:|---:|---:|---:|
| 1 | 4 | 8 | 16 |
| 2 | 8 | 16 | 32 |
| 4 | 16 | 32 | 64 |

## Live Record

`SET -> A#1`

SET is only used to arm the function and does not need to remain held during recording.

### Normal Live Record

When Free Live Record is OFF:

1. one full 4/4 count-in
2. recording starts
3. the selected 1 / 2 / 4 bars are recorded
4. pattern is saved automatically
5. playback starts automatically

The metronome accents beat 1.

### Free Live Record

Toggle:

`SET -> F#1 -> 7`

When enabled:

- no count-in
- no metronome
- the recorder waits indefinitely for the first note
- the first note defines the pattern start
- recording continues for the selected pattern length
- the pattern is saved and looped automatically

This mode is intended for playing along with an external device that is already supplying MIDI Clock.

## Arpeggiator

Enter with:

`SET -> C#2`

Then choose:

| Digit | Function |
|---:|---|
| 0 | arpeggiator ON/OFF |
| 1 | 1/4 |
| 2 | 1/8 |
| 3 | 1/16 |
| 4 | UP |
| 5 | DOWN |
| 6 | UP/DOWN |
| 7 | DOWN/UP |
| 8 | ORDER |
| 9 | RANDOM |

The arpeggiator operates on notes held through external MIDI.

## Playback

`SET -> C3`

Starts the saved sequence.

## Audible feedback

The buzzer uses a small vocabulary of tones:

- function selected: short tone
- accepted/saved: two short tones
- ON: one clearly high tone
- OFF: two low tones
- error: long low tone
- cancelled incomplete entry: one short low tone
- normal SET entry: one tone
- normal SET exit: three tones

Musically timed cues such as record-start and playback-start are non-blocking.
