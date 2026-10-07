#include <MIDIUSB.h>
#include <EEPROM.h>

// =====================================================
// HARDWARE
// =====================================================

const uint8_t PIN_T = 2;
const uint8_t PIN_A = 3;
const uint8_t PIN_B = 4;
const uint8_t PIN_C = 5;
const uint8_t PIN_D = 6;
const uint8_t PIN_E = 7;
const uint8_t PIN_F = 8;

const uint8_t DATA_PINS[6] = {
  PIN_A, PIN_B, PIN_C,
  PIN_D, PIN_E, PIN_F
};

const uint8_t PIN_INT_CLOCK_LED = 9;
const uint8_t PIN_EXT_CLOCK_LED = 16;

const uint8_t PIN_BUZZER = 10;
const uint8_t PIN_SET = A0;

const bool SET_ACTIVE_LEVEL = HIGH;


// =====================================================
// JASPER KEY CODES
// =====================================================

const uint8_t KEY_C1  = 32;
const uint8_t KEY_CS1 = 27;
const uint8_t KEY_D1  = 26;
const uint8_t KEY_DS1 = 25;
const uint8_t KEY_E1  = 24;

const uint8_t KEY_F1  = 23;
const uint8_t KEY_FS1 = 22;
const uint8_t KEY_G1  = 21;
const uint8_t KEY_GS1 = 20;
const uint8_t KEY_A1  = 19;
const uint8_t KEY_AS1 = 18;
const uint8_t KEY_B1  = 17;

const uint8_t KEY_C2  = 16;
const uint8_t KEY_CS2 = 11;
const uint8_t KEY_D2  = 10;
const uint8_t KEY_DS2 = 9;
const uint8_t KEY_E2  = 8;

const uint8_t KEY_F2  = 7;
const uint8_t KEY_FS2 = 6;
const uint8_t KEY_G2  = 5;
const uint8_t KEY_GS2 = 4;
const uint8_t KEY_A2  = 3;
const uint8_t KEY_AS2 = 2;
const uint8_t KEY_B2  = 1;

const uint8_t KEY_C3  = 0;


// =====================================================
// UI KEYS
// =====================================================

const uint8_t KEY_MIDI_SETUP = KEY_CS1;
const uint8_t KEY_TEMPO      = KEY_DS1;

const uint8_t KEY_SEQ_SETUP  = KEY_FS1;
const uint8_t KEY_STEP_REC   = KEY_GS1;
const uint8_t KEY_LIVE_REC   = KEY_AS1;

const uint8_t KEY_ARP        = KEY_CS2;

const uint8_t KEY_PLAY       = KEY_C3;


// =====================================================
// DIGIT KEYS
// =====================================================

const uint8_t KEY_DIGIT_0 = KEY_C1;
const uint8_t KEY_DIGIT_1 = KEY_D1;
const uint8_t KEY_DIGIT_2 = KEY_E1;
const uint8_t KEY_DIGIT_3 = KEY_F1;
const uint8_t KEY_DIGIT_4 = KEY_G1;
const uint8_t KEY_DIGIT_5 = KEY_A1;
const uint8_t KEY_DIGIT_6 = KEY_B1;
const uint8_t KEY_DIGIT_7 = KEY_C2;
const uint8_t KEY_DIGIT_8 = KEY_D2;
const uint8_t KEY_DIGIT_9 = KEY_E2;


// =====================================================
// MIDI / LINK
// =====================================================

const uint8_t MIDI_MIN = 36;
const uint8_t MIDI_MAX = 72;

const uint8_t BROKEN_C0 = 36;

const uint8_t LINK_CODE[37] = {
  48,43,42,41,40,39,38,37,36,35,34,33,
  32,27,26,25,24,23,22,21,20,19,18,17,
  16,11,10,9,8,7,6,5,4,3,2,1,0
};

const uint32_t T_HIGH_US = 13000UL;
const uint32_t T_LOW_US  = 6000UL;


// =====================================================
// CLOCK
// =====================================================

const uint8_t PPQN = 24;
const uint32_t EXTERNAL_CLOCK_TIMEOUT_MS = 500UL;

bool externalClockActive = false;

uint32_t lastExternalClockMs = 0;
uint32_t lastExternalPulseUs = 0;
uint32_t externalPulseIntervalUs = 20833UL;

uint32_t nextInternalPulseUs = 0;
uint32_t lastMasterPulseUs = 0;

uint8_t quarterClockPhase = 0;

bool midiTransportRunning = true;


// =====================================================
// CLOCK LEDS
// =====================================================

const uint32_t CLOCK_LED_PULSE_MS = 60UL;

bool intClockLedOn = false;
bool extClockLedOn = false;

uint32_t intClockLedOffMs = 0;
uint32_t extClockLedOffMs = 0;


// =====================================================
// EEPROM
// =====================================================

const int EEPROM_MIDI_CHANNEL_ADDR = 0;
const int EEPROM_TEMPO_ADDR        = 2;

const int EEPROM_SEQ_MAGIC_ADDR = 4;
const int EEPROM_SEQ_DATA_ADDR  = 5;

const uint8_t EEPROM_SEQ_MAGIC = 0xC3;

const int EEPROM_SEQ_DIV_ADDR     = 210;
const int EEPROM_SEQ_BARS_ADDR    = 211;
const int EEPROM_FREE_REC_ADDR    = 212;
const int EEPROM_ARP_ENABLE_ADDR  = 213;
const int EEPROM_ARP_TYPE_ADDR    = 214;
const int EEPROM_ARP_DIV_ADDR     = 215;


// =====================================================
// SETTINGS
// =====================================================

uint8_t midiChannel = 1;
uint16_t tempoBPM = 120;

uint8_t seqDivision = 3;
uint8_t seqBars = 1;

bool liveFreeStart = false;


// =====================================================
// LINK STATE
// =====================================================

bool linkActive = false;
bool triggerHigh = false;
bool linkReleasePending = false;

uint32_t nextTriggerTime = 0;

int16_t currentNote = -1;


// =====================================================
// MIDI NOTE STACK
// =====================================================

uint8_t noteHoldCount[128];

uint8_t noteStack[37];
uint8_t noteStackCount = 0;


// =====================================================
// FAILSAFE
// =====================================================

const uint32_t MIDI_FAILSAFE_MS = 30000UL;
uint32_t lastMidiActivityMs = 0;


// =====================================================
// SEQUENCER
// =====================================================

const uint8_t MAX_SEQ_STEPS = 64;
const uint8_t SEQ_REST = 0xFF;

struct SeqStep
{
  uint8_t note;
  uint16_t gateTicks;
};

SeqStep sequenceData[MAX_SEQ_STEPS];
SeqStep sequenceRecordBuffer[MAX_SEQ_STEPS];

bool sequenceValid = false;

const uint32_t SEQ_RETRIGGER_GAP_US = 45000UL;
const uint32_t SEQ_RELEASE_MARGIN_US = T_HIGH_US;
const uint32_t SEQ_MIN_GATE_US = 5000UL;


// =====================================================
// PLAYBACK
// =====================================================

bool seqPlaying = false;

uint16_t seqPlaybackTick = 0;

bool seqGateActive = false;
uint32_t seqGateOffUs = 0;


// =====================================================
// STEP RECORD
// =====================================================

const uint32_t REST_HOLD_MS = 700UL;
const uint32_t CLEAR_HOLD_MS = 1000UL;
const uint32_t FACTORY_RESET_HOLD_MS = 5000UL;

uint8_t stepRecordIndex = 0;

// Step Record remains active after SET is released.
bool stepRecordLatched = false;


// =====================================================
// LIVE RECORD
// =====================================================

enum LiveRecordState
{
  LIVE_IDLE,
  LIVE_COUNT_IN,
  LIVE_WAIT_FIRST_NOTE,
  LIVE_RECORDING,
  LIVE_WAIT_JASPER_RELEASE
};

enum LiveInputSource
{
  LIVE_SOURCE_NONE,
  LIVE_SOURCE_MIDI,
  LIVE_SOURCE_JASPER
};

LiveRecordState liveRecordState = LIVE_IDLE;
LiveInputSource liveInputSource = LIVE_SOURCE_NONE;

const uint16_t COUNT_IN_TICKS = PPQN * 4;

uint16_t liveCountInTick = 0;
uint16_t liveRecordTick = 0;

int16_t liveActiveNote = -1;
uint8_t liveActiveStep = 0;
uint16_t liveActiveStartTick = 0;

bool liveJasperKeyActive = false;
uint8_t liveJasperKeyCode = 0;

bool liveJasperLastTrigger = false;
uint32_t liveJasperLastPulseMs = 0;

bool autoPlaybackWaitingForJasperRelease = false;


// =====================================================
// ARPEGGIATOR
// =====================================================

enum ArpType
{
  ARP_UP = 1,
  ARP_DOWN = 2,
  ARP_UP_DOWN = 3,
  ARP_DOWN_UP = 4,
  ARP_ORDER = 5,
  ARP_RANDOM = 6
};

bool arpEnabled = false;
uint8_t arpType = ARP_UP;
uint8_t arpDivision = 3;

int8_t arpIndex = 0;
int8_t arpDirection = 1;

bool arpGateActive = false;
uint32_t arpGateOffUs = 0;


// =====================================================
// SET MODE
// =====================================================

enum ProgramMode
{
  PROGRAM_NONE,
  PROGRAM_MIDI_CHANNEL,
  PROGRAM_TEMPO,
  PROGRAM_SEQ_SETUP,
  PROGRAM_STEP_RECORD,
  PROGRAM_ARP_SETUP
};

ProgramMode programMode = PROGRAM_NONE;

bool setMode = false;
bool lastSetPressed = false;

bool setStopLatch = false;

bool setKeyActive = false;
uint8_t setKeyCode = 0;

uint32_t setKeyPressMs = 0;
uint32_t lastSetKeyPulseMs = 0;

bool lastKeyboardTrigger = false;

bool suppressCurrentKeyRelease = false;

bool restHoldConfirmed = false;

bool clearHoldArmed = false;
bool factoryResetDone = false;

bool playPending = false;
bool liveRecPending = false;


// =====================================================
// NUMERIC INPUT
// =====================================================

uint8_t midiDigits[2];
uint8_t midiDigitCount = 0;

uint8_t tempoDigits[3];
uint8_t tempoDigitCount = 0;


// =====================================================
// DIN MIDI PARSER
// =====================================================

uint8_t dinRunningStatus = 0;
uint8_t dinData1 = 0;
uint8_t dinDataCount = 0;


// =====================================================
// FORWARD DECLARATIONS
// =====================================================

void handleNoteOff(uint8_t note);
void resetArpTraversal();
void startSequencer();


// =====================================================
// DIVISION HELPERS
// =====================================================

uint8_t divisionToTicks(uint8_t division)
{
  switch (division)
  {
    case 1: return 24;
    case 2: return 12;
    default: return 6;
  }
}

uint8_t getTicksPerStep()
{
  return divisionToTicks(seqDivision);
}

uint8_t getArpTicksPerStep()
{
  return divisionToTicks(arpDivision);
}

uint16_t getPatternTicks()
{
  return (uint16_t)seqBars * 4 * PPQN;
}

uint8_t getActiveSteps()
{
  return getPatternTicks() / getTicksPerStep();
}


// =====================================================
// SOUND FEEDBACK
// =====================================================

void beepOnce()
{
  tone(PIN_BUZZER, 2200, 60);
  delay(90);
}

void beep(uint8_t count)
{
  for (uint8_t i = 0; i < count; i++)
    beepOnce();
}

void parameterTone()
{
  tone(PIN_BUZZER, 1700, 45);
  delay(60);
}

void acceptedTone()
{
  beep(2);
}

void onTone()
{
  tone(PIN_BUZZER, 3000, 140);
  delay(170);
}

void offTone()
{
  tone(PIN_BUZZER, 700, 90);
  delay(140);
  tone(PIN_BUZZER, 700, 90);
  delay(120);
}

void errorBeep()
{
  tone(PIN_BUZZER, 650, 500);
  delay(550);
}

void cancelTone()
{
  tone(PIN_BUZZER, 850, 120);
  delay(140);
}

void clearArmedTone()
{
  tone(PIN_BUZZER, 1500, 70);
}

void clearTone()
{
  tone(PIN_BUZZER, 1200, 50);
  delay(60);
  tone(PIN_BUZZER, 1800, 50);
  delay(60);
  tone(PIN_BUZZER, 2400, 100);
  delay(120);
}

void factoryResetTone()
{
  tone(PIN_BUZZER, 2600, 100);
  delay(150);
  tone(PIN_BUZZER, 1800, 130);
  delay(180);
  tone(PIN_BUZZER, 900, 350);
  delay(380);
}

void liveRecordStartCue()
{
  tone(PIN_BUZZER, 3100, 55);
}

void playbackStartCue()
{
  tone(PIN_BUZZER, 1050, 110);
}


// =====================================================
// METRONOME
// =====================================================

void countInClick(uint8_t beat)
{
  if (beat == 4)
    tone(PIN_BUZZER, 2600, 90);
  else
    tone(PIN_BUZZER, 1500, 45);
}

void recordingMetronomeClick(uint8_t beat)
{
  if (beat == 1)
    tone(PIN_BUZZER, 2300, 55);
  else
    tone(PIN_BUZZER, 1400, 35);
}


// =====================================================
// CLOCK LEDS
// =====================================================

void internalClockLedPulse()
{
  digitalWrite(PIN_EXT_CLOCK_LED, LOW);
  extClockLedOn = false;

  digitalWrite(PIN_INT_CLOCK_LED, HIGH);
  intClockLedOn = true;

  intClockLedOffMs = millis() + CLOCK_LED_PULSE_MS;
}

void externalClockLedPulse()
{
  digitalWrite(PIN_INT_CLOCK_LED, LOW);
  intClockLedOn = false;

  digitalWrite(PIN_EXT_CLOCK_LED, HIGH);
  extClockLedOn = true;

  extClockLedOffMs = millis() + CLOCK_LED_PULSE_MS;
}

void updateClockLeds()
{
  uint32_t now = millis();

  if (
    intClockLedOn &&
    (int32_t)(now - intClockLedOffMs) >= 0
  )
  {
    digitalWrite(PIN_INT_CLOCK_LED, LOW);
    intClockLedOn = false;
  }

  if (
    extClockLedOn &&
    (int32_t)(now - extClockLedOffMs) >= 0
  )
  {
    digitalWrite(PIN_EXT_CLOCK_LED, LOW);
    extClockLedOn = false;
  }
}


// =====================================================
// LINK LOW LEVEL
// =====================================================

void releaseLinkBus()
{
  digitalWrite(PIN_T, LOW);
  pinMode(PIN_T, INPUT);

  for (uint8_t i = 0; i < 6; i++)
  {
    digitalWrite(DATA_PINS[i], LOW);
    pinMode(DATA_PINS[i], INPUT);
  }

  linkActive = false;
  triggerHigh = false;
  linkReleasePending = false;
}

void setLinkCode(uint8_t code)
{
  for (uint8_t bit = 0; bit < 6; bit++)
  {
    digitalWrite(
      DATA_PINS[bit],
      (code & (1 << bit)) ? HIGH : LOW
    );

    pinMode(DATA_PINS[bit], OUTPUT);
  }
}

uint8_t readLinkCode()
{
  uint8_t code = 0;

  if (digitalRead(PIN_A)) code |= 1 << 0;
  if (digitalRead(PIN_B)) code |= 1 << 1;
  if (digitalRead(PIN_C)) code |= 1 << 2;
  if (digitalRead(PIN_D)) code |= 1 << 3;
  if (digitalRead(PIN_E)) code |= 1 << 4;
  if (digitalRead(PIN_F)) code |= 1 << 5;

  return code;
}


// =====================================================
// LINK OUTPUT
// =====================================================

void startLinkNote(uint8_t note)
{
  if (
    note < MIDI_MIN ||
    note > MIDI_MAX ||
    note == BROKEN_C0
  )
    return;

  uint8_t code = LINK_CODE[note - MIDI_MIN];

  linkReleasePending = false;

  digitalWrite(PIN_T, LOW);
  pinMode(PIN_T, OUTPUT);

  setLinkCode(code);

  currentNote = note;

  triggerHigh = false;
  linkActive = true;

  nextTriggerTime = micros() + 100UL;
}

void changeLinkNote(uint8_t note)
{
  if (
    note < MIDI_MIN ||
    note > MIDI_MAX ||
    note == BROKEN_C0
  )
    return;

  uint8_t code = LINK_CODE[note - MIDI_MIN];

  linkReleasePending = false;

  setLinkCode(code);

  currentNote = note;
}

void stopLink()
{
  currentNote = -1;
  releaseLinkBus();
}

void requestSafeLinkRelease()
{
  if (!linkActive)
    return;

  if (!triggerHigh)
  {
    stopLink();
    return;
  }

  linkReleasePending = true;
}

void updateLinkTrigger()
{
  if (
    !linkActive ||
    setMode
  )
    return;

  uint32_t now = micros();

  if ((int32_t)(now - nextTriggerTime) < 0)
    return;

  if (!triggerHigh)
  {
    digitalWrite(PIN_T, HIGH);

    triggerHigh = true;
    nextTriggerTime = now + T_HIGH_US;
  }
  else
  {
    digitalWrite(PIN_T, LOW);

    triggerHigh = false;

    if (linkReleasePending)
    {
      currentNote = -1;
      releaseLinkBus();
      return;
    }

    nextTriggerTime = now + T_LOW_US;
  }
}


// =====================================================
// NOTE STACK
// =====================================================

void removeNoteFromStack(uint8_t note)
{
  for (uint8_t i = 0; i < noteStackCount; i++)
  {
    if (noteStack[i] == note)
    {
      for (uint8_t j = i; j < noteStackCount - 1; j++)
        noteStack[j] = noteStack[j + 1];

      noteStackCount--;
      return;
    }
  }
}

void pushNoteToStack(uint8_t note)
{
  removeNoteFromStack(note);

  if (noteStackCount < 37)
  {
    noteStack[noteStackCount] = note;
    noteStackCount++;
  }
}

void clearNoteStack()
{
  memset(
    noteHoldCount,
    0,
    sizeof(noteHoldCount)
  );

  noteStackCount = 0;
}


// =====================================================
// LINK -> MIDI NOTE
// =====================================================

int16_t linkCodeToMidiNote(uint8_t code)
{
  for (uint8_t note = 48; note <= 72; note++)
  {
    if (LINK_CODE[note - MIDI_MIN] == code)
      return note;
  }

  return -1;
}


// =====================================================
// LIVE RECORD TIMING
// =====================================================

uint16_t getLiveCurrentTick()
{
  if (liveRecordState != LIVE_RECORDING)
    return liveRecordTick;

  uint32_t pulseUs;

  if (externalClockActive)
  {
    pulseUs = externalPulseIntervalUs;
  }
  else
  {
    pulseUs =
      60000000UL /
      (
        (uint32_t)tempoBPM *
        PPQN
      );
  }

  uint16_t tick = liveRecordTick;

  uint32_t elapsed =
    micros() -
    lastMasterPulseUs;

  if (elapsed > pulseUs / 2)
    tick++;

  uint16_t maxTick = getPatternTicks();

  if (tick > maxTick)
    tick = maxTick;

  return tick;
}


// =====================================================
// QUANTIZATION
// =====================================================

uint8_t quantizeLiveStep(uint16_t tick)
{
  uint8_t ticksPerStep = getTicksPerStep();

  uint16_t rounded =
    (
      tick +
      ticksPerStep / 2
    ) /
    ticksPerStep;

  uint8_t activeSteps = getActiveSteps();

  if (rounded >= activeSteps)
    rounded = activeSteps - 1;

  return (uint8_t)rounded;
}


// =====================================================
// LIVE RECORD
// =====================================================

void finalizeLiveNote(uint16_t endTick)
{
  if (liveActiveNote < 0)
    return;

  if (endTick <= liveActiveStartTick)
    endTick = liveActiveStartTick + 1;

  sequenceData[
    liveActiveStep
  ].gateTicks =
    endTick -
    liveActiveStartTick;

  liveActiveNote = -1;
}

void beginActualLiveRecording(
  LiveInputSource source
)
{
  liveInputSource = source;
  liveRecordState = LIVE_RECORDING;

  liveRecordTick = 0;
  liveActiveNote = -1;

  lastMasterPulseUs = micros();

  midiTransportRunning = true;

  if (!externalClockActive)
  {
    quarterClockPhase = 0;

    nextInternalPulseUs =
      micros() +
      (
        60000000UL /
        (
          (uint32_t)tempoBPM *
          PPQN
        )
      );
  }

  liveRecordStartCue();
}

void liveRecordNoteOn(
  uint8_t note,
  LiveInputSource source
)
{
  if (liveRecordState == LIVE_WAIT_FIRST_NOTE)
    beginActualLiveRecording(source);

  if (liveRecordState != LIVE_RECORDING)
    return;

  if (liveInputSource == LIVE_SOURCE_NONE)
    liveInputSource = source;

  if (liveInputSource != source)
    return;

  uint16_t nowTick = getLiveCurrentTick();

  if (liveActiveNote >= 0)
    finalizeLiveNote(nowTick);

  uint8_t step =
    quantizeLiveStep(nowTick);

  uint16_t quantizedTick =
    (uint16_t)step *
    getTicksPerStep();

  sequenceData[step].note = note;
  sequenceData[step].gateTicks = 1;

  liveActiveNote = note;
  liveActiveStep = step;
  liveActiveStartTick = quantizedTick;
}

void liveRecordNoteOff(
  uint8_t note,
  LiveInputSource source
)
{
  if (liveRecordState != LIVE_RECORDING)
    return;

  if (liveInputSource != source)
    return;

  if (liveActiveNote != note)
    return;

  finalizeLiveNote(getLiveCurrentTick());
}


// =====================================================
// JASPER LIVE KEYBOARD
// =====================================================

void handleJasperLiveNoteOn(uint8_t code)
{
  int16_t note = linkCodeToMidiNote(code);

  if (note < 0)
    return;

  liveRecordNoteOn(
    (uint8_t)note,
    LIVE_SOURCE_JASPER
  );
}

void handleJasperLiveNoteOff(uint8_t code)
{
  int16_t note = linkCodeToMidiNote(code);

  if (note < 0)
    return;

  liveRecordNoteOff(
    (uint8_t)note,
    LIVE_SOURCE_JASPER
  );
}

void readJasperLiveKeyboard()
{
  if (liveInputSource == LIVE_SOURCE_MIDI)
    return;

  bool trigger = digitalRead(PIN_T);
  uint32_t nowMs = millis();

  if (
    trigger &&
    !liveJasperLastTrigger
  )
  {
    delayMicroseconds(50);

    uint8_t code = readLinkCode();

    liveJasperLastPulseMs = nowMs;

    if (!liveJasperKeyActive)
    {
      liveJasperKeyActive = true;
      liveJasperKeyCode = code;

      handleJasperLiveNoteOn(code);
    }
    else if (code != liveJasperKeyCode)
    {
      uint8_t oldCode = liveJasperKeyCode;

      handleJasperLiveNoteOff(oldCode);

      liveJasperKeyCode = code;

      handleJasperLiveNoteOn(code);
    }
  }

  liveJasperLastTrigger = trigger;

  if (
    liveJasperKeyActive &&
    nowMs -
      liveJasperLastPulseMs >
      50
  )
  {
    uint8_t oldCode = liveJasperKeyCode;

    liveJasperKeyActive = false;

    handleJasperLiveNoteOff(oldCode);

    if (autoPlaybackWaitingForJasperRelease)
    {
      autoPlaybackWaitingForJasperRelease = false;

      liveRecordState = LIVE_IDLE;

      playbackStartCue();

      startSequencer();
    }
  }
}


// =====================================================
// SEQUENCE EEPROM
// =====================================================

void saveSequence()
{
  int address = EEPROM_SEQ_DATA_ADDR;

  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    EEPROM.update(
      address++,
      sequenceData[i].note
    );

    EEPROM.update(
      address++,
      lowByte(
        sequenceData[i].gateTicks
      )
    );

    EEPROM.update(
      address++,
      highByte(
        sequenceData[i].gateTicks
      )
    );
  }

  EEPROM.update(
    EEPROM_SEQ_MAGIC_ADDR,
    EEPROM_SEQ_MAGIC
  );

  sequenceValid = true;
}

void loadSequence()
{
  if (
    EEPROM.read(
      EEPROM_SEQ_MAGIC_ADDR
    ) !=
    EEPROM_SEQ_MAGIC
  )
  {
    sequenceValid = false;

    for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
    {
      sequenceData[i].note = SEQ_REST;
      sequenceData[i].gateTicks = 0;
    }

    return;
  }

  int address = EEPROM_SEQ_DATA_ADDR;

  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    uint8_t note =
      EEPROM.read(address++);

    uint8_t lo =
      EEPROM.read(address++);

    uint8_t hi =
      EEPROM.read(address++);

    sequenceData[i].note = note;
    sequenceData[i].gateTicks = word(hi, lo);

    if (
      note != SEQ_REST &&
      (
        note < 37 ||
        note > 72
      )
    )
    {
      sequenceValid = false;
      return;
    }
  }

  sequenceValid = true;
}


// =====================================================
// CLEAR SEQUENCE
// =====================================================

void clearSequenceData()
{
  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    sequenceData[i].note = SEQ_REST;
    sequenceData[i].gateTicks = 0;
  }

  sequenceValid = false;

  EEPROM.update(
    EEPROM_SEQ_MAGIC_ADDR,
    0
  );
}

void clearSequence()
{
  seqPlaying = false;
  seqGateActive = false;

  stopLink();

  clearSequenceData();

  clearTone();
}


// =====================================================
// ARP RESET
// =====================================================

void resetArpTraversal()
{
  arpDirection = 1;
  arpIndex = 0;

  if (
    arpType == ARP_DOWN ||
    arpType == ARP_DOWN_UP
  )
  {
    arpDirection = -1;

    if (noteStackCount > 0)
      arpIndex = noteStackCount - 1;
  }
}


// =====================================================
// INTERNAL CLOCK RESET
// =====================================================

void resetInternalClock()
{
  quarterClockPhase = 0;
  nextInternalPulseUs = micros();
}


// =====================================================
// FACTORY RESET
// =====================================================

void factoryReset()
{
  seqPlaying = false;
  seqGateActive = false;

  stepRecordLatched = false;

  liveRecordState = LIVE_IDLE;
  liveInputSource = LIVE_SOURCE_NONE;

  arpGateActive = false;

  clearNoteStack();
  stopLink();

  midiChannel = 1;
  tempoBPM = 120;

  seqDivision = 3;
  seqBars = 1;

  liveFreeStart = false;

  arpEnabled = false;
  arpType = ARP_UP;
  arpDivision = 3;

  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    sequenceData[i].note = SEQ_REST;
    sequenceData[i].gateTicks = 0;
  }

  sequenceValid = false;

  EEPROM.update(
    EEPROM_SEQ_MAGIC_ADDR,
    0
  );

  int address = EEPROM_SEQ_DATA_ADDR;

  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    EEPROM.update(address++, SEQ_REST);
    EEPROM.update(address++, 0);
    EEPROM.update(address++, 0);
  }

  EEPROM.update(
    EEPROM_MIDI_CHANNEL_ADDR,
    midiChannel
  );

  EEPROM.put(
    EEPROM_TEMPO_ADDR,
    tempoBPM
  );

  EEPROM.update(
    EEPROM_SEQ_DIV_ADDR,
    seqDivision
  );

  EEPROM.update(
    EEPROM_SEQ_BARS_ADDR,
    seqBars
  );

  EEPROM.update(
    EEPROM_FREE_REC_ADDR,
    0
  );

  EEPROM.update(
    EEPROM_ARP_ENABLE_ADDR,
    0
  );

  EEPROM.update(
    EEPROM_ARP_TYPE_ADDR,
    arpType
  );

  EEPROM.update(
    EEPROM_ARP_DIV_ADDR,
    arpDivision
  );

  resetArpTraversal();
  resetInternalClock();

  factoryResetTone();
}


// =====================================================
// NEXT SEQUENCE NOTE
// =====================================================

uint16_t ticksToNextSequenceNote(
  uint8_t currentStep
)
{
  uint8_t activeSteps = getActiveSteps();
  uint8_t ticksPerStep = getTicksPerStep();

  for (
    uint8_t distance = 1;
    distance <= activeSteps;
    distance++
  )
  {
    uint8_t p =
      (
        currentStep +
        distance
      ) %
      activeSteps;

    if (sequenceData[p].note != SEQ_REST)
    {
      return
        (uint16_t)distance *
        ticksPerStep;
    }
  }

  return getPatternTicks();
}


// =====================================================
// PLAY SEQUENCE STEP
// =====================================================

void playSequenceStep(uint8_t step)
{
  SeqStep &event = sequenceData[step];

  if (event.note == SEQ_REST)
    return;

  if (linkActive)
    stopLink();

  startLinkNote(event.note);

  uint32_t pulseUs;

  if (externalClockActive)
  {
    pulseUs = externalPulseIntervalUs;
  }
  else
  {
    pulseUs =
      60000000UL /
      (
        (uint32_t)tempoBPM *
        PPQN
      );
  }

  uint16_t gateTicks = event.gateTicks;

  if (gateTicks == 0)
    gateTicks = getTicksPerStep();

  uint32_t desiredUs =
    (uint32_t)gateTicks *
    pulseUs;

  uint32_t requestUs;

  if (desiredUs > SEQ_RELEASE_MARGIN_US)
    requestUs = desiredUs - SEQ_RELEASE_MARGIN_US;
  else
    requestUs = SEQ_MIN_GATE_US;

  uint16_t nextTicks =
    ticksToNextSequenceNote(step);

  uint32_t nextNoteUs =
    (uint32_t)nextTicks *
    pulseUs;

  if (
    nextNoteUs >
    (
      SEQ_RETRIGGER_GAP_US +
      SEQ_RELEASE_MARGIN_US
    )
  )
  {
    uint32_t latestSafeRequest =
      nextNoteUs -
      SEQ_RETRIGGER_GAP_US -
      SEQ_RELEASE_MARGIN_US;

    if (requestUs > latestSafeRequest)
      requestUs = latestSafeRequest;
  }

  if (requestUs < SEQ_MIN_GATE_US)
    requestUs = SEQ_MIN_GATE_US;

  seqGateOffUs =
    micros() +
    requestUs;

  seqGateActive = true;
}


// =====================================================
// SEQUENCER CONTROL
// =====================================================

void stopSequencer()
{
  seqPlaying = false;
  seqPlaybackTick = 0;
  seqGateActive = false;

  stopLink();
}

void pauseSequencer()
{
  seqPlaying = false;
  seqGateActive = false;
  arpGateActive = false;

  stopLink();
}

void startSequencer()
{
  if (!sequenceValid)
  {
    errorBeep();
    return;
  }

  midiTransportRunning = true;

  clearNoteStack();
  stopLink();

  seqPlaybackTick = 0;
  seqGateActive = false;
  seqPlaying = true;

  if (sequenceData[0].note != SEQ_REST)
    playSequenceStep(0);

  seqPlaybackTick = 1;
}

void startSequencerMidiSynced()
{
  if (!sequenceValid)
    return;

  clearNoteStack();
  stopLink();

  seqPlaybackTick = 0;
  seqGateActive = false;
  seqPlaying = true;
}

void continueSequencerMidiSynced()
{
  if (!sequenceValid)
    return;

  seqGateActive = false;
  seqPlaying = true;

  stopLink();
}

void updateSequencerGate()
{
  if (
    !seqPlaying ||
    !seqGateActive
  )
    return;

  if (
    (int32_t)(
      micros() -
      seqGateOffUs
    ) >= 0
  )
  {
    requestSafeLinkRelease();
    seqGateActive = false;
  }
}


// =====================================================
// STEP RECORD
// =====================================================

void beginStepRecord()
{
  programMode = PROGRAM_STEP_RECORD;
  stepRecordLatched = true;
  stepRecordIndex = 0;

  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    sequenceRecordBuffer[i].note = SEQ_REST;
    sequenceRecordBuffer[i].gateTicks = 0;
  }

  parameterTone();
}

void cancelStepRecord()
{
  stepRecordIndex = 0;

  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    sequenceRecordBuffer[i].note = SEQ_REST;
    sequenceRecordBuffer[i].gateTicks = 0;
  }

  stepRecordLatched = false;
  programMode = PROGRAM_NONE;
  setKeyActive = false;
  restHoldConfirmed = false;

  releaseLinkBus();

  cancelTone();
}

void finishStepRecord()
{
  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
    sequenceData[i] = sequenceRecordBuffer[i];

  saveSequence();

  stepRecordIndex = 0;
  stepRecordLatched = false;
  programMode = PROGRAM_NONE;
  setKeyActive = false;

  releaseLinkBus();

  acceptedTone();
}

void recordStep(uint8_t note)
{
  if (stepRecordIndex >= getActiveSteps())
    return;

  sequenceRecordBuffer[
    stepRecordIndex
  ].note = note;

  if (note == SEQ_REST)
  {
    sequenceRecordBuffer[
      stepRecordIndex
    ].gateTicks = 0;
  }
  else
  {
    sequenceRecordBuffer[
      stepRecordIndex
    ].gateTicks =
      getTicksPerStep();
  }

  stepRecordIndex++;

  if (stepRecordIndex >= getActiveSteps())
    finishStepRecord();
}


// =====================================================
// LIVE RECORD ARM / FINISH
// =====================================================

void clearLiveBuffer()
{
  for (uint8_t i = 0; i < MAX_SEQ_STEPS; i++)
  {
    sequenceData[i].note = SEQ_REST;
    sequenceData[i].gateTicks = 0;
  }
}

void armLiveRecord()
{
  seqPlaying = false;
  seqGateActive = false;

  arpGateActive = false;

  clearNoteStack();
  stopLink();

  clearLiveBuffer();

  liveInputSource = LIVE_SOURCE_NONE;
  liveActiveNote = -1;

  liveCountInTick = 0;
  liveRecordTick = 0;

  liveJasperKeyActive = false;

  liveJasperLastTrigger =
    digitalRead(PIN_T);

  liveJasperLastPulseMs =
    millis();

  autoPlaybackWaitingForJasperRelease = false;

  midiTransportRunning = true;

  if (liveFreeStart)
    liveRecordState = LIVE_WAIT_FIRST_NOTE;
  else
    liveRecordState = LIVE_COUNT_IN;

  if (!externalClockActive)
  {
    quarterClockPhase = 0;
    nextInternalPulseUs = micros();
  }
}

void finishLiveRecord()
{
  finalizeLiveNote(
    getPatternTicks()
  );

  saveSequence();

  clearNoteStack();

  if (
    liveInputSource == LIVE_SOURCE_JASPER &&
    liveJasperKeyActive
  )
  {
    liveRecordState =
      LIVE_WAIT_JASPER_RELEASE;

    autoPlaybackWaitingForJasperRelease = true;

    return;
  }

  liveRecordState = LIVE_IDLE;

  stopLink();

  playbackStartCue();

  startSequencer();
}


// =====================================================
// NORMAL MIDI LAST NOTE
// =====================================================

void updateLastNote()
{
  if (
    seqPlaying ||
    arpEnabled
  )
    return;

  if (noteStackCount == 0)
  {
    stopLink();
    return;
  }

  uint8_t note =
    noteStack[
      noteStackCount - 1
    ];

  if (!linkActive)
    startLinkNote(note);
  else if (currentNote != note)
    changeLinkNote(note);
}


// =====================================================
// MIDI NOTE ON
// =====================================================

void handleNoteOn(
  uint8_t note,
  uint8_t velocity
)
{
  if (velocity == 0)
  {
    handleNoteOff(note);
    return;
  }

  if (
    note == BROKEN_C0 ||
    note < MIDI_MIN ||
    note > MIDI_MAX
  )
    return;

  if (
    liveRecordState == LIVE_WAIT_FIRST_NOTE ||
    liveRecordState == LIVE_RECORDING
  )
  {
    if (liveInputSource == LIVE_SOURCE_JASPER)
      return;

    liveRecordNoteOn(
      note,
      LIVE_SOURCE_MIDI
    );

    if (noteHoldCount[note] < 255)
      noteHoldCount[note]++;

    pushNoteToStack(note);

    if (!linkActive)
      startLinkNote(note);
    else
      changeLinkNote(note);

    return;
  }

  if (seqPlaying)
    return;

  if (noteHoldCount[note] < 255)
    noteHoldCount[note]++;

  pushNoteToStack(note);

  if (arpEnabled)
  {
    resetArpTraversal();
    return;
  }

  updateLastNote();
}


// =====================================================
// MIDI NOTE OFF
// =====================================================

void handleNoteOff(uint8_t note)
{
  if (
    liveRecordState == LIVE_RECORDING &&
    liveInputSource == LIVE_SOURCE_MIDI
  )
  {
    liveRecordNoteOff(
      note,
      LIVE_SOURCE_MIDI
    );

    if (noteHoldCount[note] > 0)
      noteHoldCount[note]--;

    if (noteHoldCount[note] == 0)
      removeNoteFromStack(note);

    if (noteStackCount == 0)
    {
      stopLink();
    }
    else
    {
      changeLinkNote(
        noteStack[
          noteStackCount - 1
        ]
      );
    }

    return;
  }

  if (seqPlaying)
    return;

  if (note == BROKEN_C0)
    return;

  if (noteHoldCount[note] > 0)
    noteHoldCount[note]--;

  if (noteHoldCount[note] == 0)
    removeNoteFromStack(note);

  if (arpEnabled)
  {
    resetArpTraversal();

    if (noteStackCount == 0)
      stopLink();

    return;
  }

  updateLastNote();
}


// =====================================================
// ARPEGGIATOR
// =====================================================

uint8_t buildArpList(uint8_t *notes)
{
  uint8_t count = noteStackCount;

  if (count == 0)
    return 0;

  for (uint8_t i = 0; i < count; i++)
    notes[i] = noteStack[i];

  if (arpType == ARP_ORDER)
    return count;

  for (uint8_t i = 1; i < count; i++)
  {
    uint8_t value = notes[i];
    int8_t j = i - 1;

    while (
      j >= 0 &&
      notes[j] > value
    )
    {
      notes[j + 1] = notes[j];
      j--;
    }

    notes[j + 1] = value;
  }

  return count;
}

uint8_t getNextArpNote()
{
  uint8_t notes[37];

  uint8_t count =
    buildArpList(notes);

  if (count == 0)
    return 0xFF;

  if (count == 1)
    return notes[0];

  if (arpType == ARP_RANDOM)
    return notes[random(count)];

  if (
    arpIndex < 0 ||
    arpIndex >= count
  )
  {
    resetArpTraversal();
  }

  uint8_t note = notes[arpIndex];

  if (
    arpType == ARP_UP ||
    arpType == ARP_ORDER
  )
  {
    arpIndex++;

    if (arpIndex >= count)
      arpIndex = 0;
  }

  else if (arpType == ARP_DOWN)
  {
    arpIndex--;

    if (arpIndex < 0)
      arpIndex = count - 1;
  }

  else if (arpType == ARP_UP_DOWN)
  {
    arpIndex += arpDirection;

    if (arpIndex >= count)
    {
      arpDirection = -1;
      arpIndex = count - 2;
    }
    else if (arpIndex < 0)
    {
      arpDirection = 1;
      arpIndex = 1;
    }
  }

  else if (arpType == ARP_DOWN_UP)
  {
    arpIndex += arpDirection;

    if (arpIndex < 0)
    {
      arpDirection = 1;
      arpIndex = 1;
    }
    else if (arpIndex >= count)
    {
      arpDirection = -1;
      arpIndex = count - 2;
    }
  }

  return note;
}

void playArpStep()
{
  if (
    !arpEnabled ||
    seqPlaying ||
    liveRecordState != LIVE_IDLE ||
    noteStackCount == 0
  )
    return;

  uint8_t note = getNextArpNote();

  if (note == 0xFF)
    return;

  if (linkActive)
    stopLink();

  startLinkNote(note);

  uint32_t pulseUs;

  if (externalClockActive)
  {
    pulseUs = externalPulseIntervalUs;
  }
  else
  {
    pulseUs =
      60000000UL /
      (
        (uint32_t)tempoBPM *
        PPQN
      );
  }

  uint32_t stepUs =
    (uint32_t)getArpTicksPerStep() *
    pulseUs;

  uint32_t requestUs =
    SEQ_MIN_GATE_US;

  if (
    stepUs >
    (
      SEQ_RETRIGGER_GAP_US +
      SEQ_RELEASE_MARGIN_US +
      SEQ_MIN_GATE_US
    )
  )
  {
    requestUs =
      stepUs -
      SEQ_RETRIGGER_GAP_US -
      SEQ_RELEASE_MARGIN_US;
  }

  arpGateOffUs =
    micros() +
    requestUs;

  arpGateActive = true;
}

void updateArpGate()
{
  if (!arpGateActive)
    return;

  if (
    (int32_t)(
      micros() -
      arpGateOffUs
    ) >= 0
  )
  {
    requestSafeLinkRelease();
    arpGateActive = false;
  }
}


// =====================================================
// MASTER CLOCK
// =====================================================

void processMasterClockPulse()
{
  lastMasterPulseUs = micros();

  if (
    externalClockActive &&
    !midiTransportRunning
  )
  {
    quarterClockPhase++;

    if (quarterClockPhase >= PPQN)
      quarterClockPhase = 0;

    return;
  }

  // COUNT-IN
  if (liveRecordState == LIVE_COUNT_IN)
  {
    if (liveCountInTick % PPQN == 0)
    {
      uint8_t beat =
        (
          liveCountInTick /
          PPQN
        ) + 1;

      countInClick(beat);
    }

    liveCountInTick++;

    if (liveCountInTick >= COUNT_IN_TICKS)
    {
      liveRecordState = LIVE_RECORDING;
      liveInputSource = LIVE_SOURCE_NONE;
      liveRecordTick = 0;
      liveActiveNote = -1;

      liveRecordStartCue();
    }
  }

  // LIVE RECORD
  else if (liveRecordState == LIVE_RECORDING)
  {
    if (!liveFreeStart)
    {
      if (liveRecordTick % PPQN == 0)
      {
        uint8_t beat =
          (
            (
              liveRecordTick /
              PPQN
            ) % 4
          ) + 1;

        recordingMetronomeClick(beat);
      }
    }

    liveRecordTick++;

    if (liveRecordTick >= getPatternTicks())
      finishLiveRecord();
  }

  // SEQUENCER
  else if (seqPlaying)
  {
    uint8_t ticksPerStep =
      getTicksPerStep();

    if (
      seqPlaybackTick %
      ticksPerStep ==
      0
    )
    {
      uint8_t step =
        seqPlaybackTick /
        ticksPerStep;

      if (step < getActiveSteps())
        playSequenceStep(step);
    }

    seqPlaybackTick++;

    if (seqPlaybackTick >= getPatternTicks())
      seqPlaybackTick = 0;
  }

  // ARPEGGIATOR
  else if (
    arpEnabled &&
    noteStackCount > 0
  )
  {
    uint8_t arpTicks =
      getArpTicksPerStep();

    if (
      quarterClockPhase %
      arpTicks ==
      0
    )
    {
      playArpStep();
    }
  }

  quarterClockPhase++;

  if (quarterClockPhase >= PPQN)
    quarterClockPhase = 0;
}


// =====================================================
// INTERNAL CLOCK
// =====================================================

uint32_t getInternalPulsePeriodUs()
{
  return
    60000000UL /
    (
      (uint32_t)tempoBPM *
      PPQN
    );
}

void updateInternalClock()
{
  if (externalClockActive)
    return;

  uint32_t now = micros();
  uint32_t period =
    getInternalPulsePeriodUs();

  if (
    (int32_t)(
      now -
      nextInternalPulseUs
    ) < 0
  )
    return;

  if (quarterClockPhase == 0)
    internalClockLedPulse();

  processMasterClockPulse();

  nextInternalPulseUs += period;

  if (
    (int32_t)(
      now -
      nextInternalPulseUs
    ) >
    1000000L
  )
  {
    nextInternalPulseUs = now + period;
  }
}


// =====================================================
// MIDI REALTIME
// =====================================================

void handleMidiRealtime(uint8_t status)
{
  // CLOCK
  if (status == 0xF8)
  {
    uint32_t now = micros();

    if (lastExternalPulseUs != 0)
    {
      uint32_t measured =
        now -
        lastExternalPulseUs;

      if (
        measured > 1000UL &&
        measured < 500000UL
      )
      {
        externalPulseIntervalUs =
          (
            externalPulseIntervalUs *
            3UL +
            measured
          ) / 4UL;
      }
    }

    lastExternalPulseUs = now;

    if (!externalClockActive)
    {
      externalClockActive = true;
      quarterClockPhase = 0;

      digitalWrite(
        PIN_INT_CLOCK_LED,
        LOW
      );

      intClockLedOn = false;
    }

    lastExternalClockMs = millis();

    if (quarterClockPhase == 0)
      externalClockLedPulse();

    processMasterClockPulse();

    return;
  }

  // START
  if (status == 0xFA)
  {
    midiTransportRunning = true;
    quarterClockPhase = 0;

    if (
      sequenceValid &&
      liveRecordState == LIVE_IDLE &&
      !setMode &&
      !stepRecordLatched
    )
    {
      startSequencerMidiSynced();
    }

    return;
  }

  // CONTINUE
  if (status == 0xFB)
  {
    midiTransportRunning = true;

    if (
      sequenceValid &&
      liveRecordState == LIVE_IDLE &&
      !setMode &&
      !stepRecordLatched
    )
    {
      continueSequencerMidiSynced();
    }

    return;
  }

  // STOP
  if (status == 0xFC)
  {
    midiTransportRunning = false;

    pauseSequencer();

    arpGateActive = false;

    stopLink();

    return;
  }
}


// =====================================================
// EXTERNAL CLOCK TIMEOUT
// =====================================================

void updateExternalClockTimeout()
{
  if (!externalClockActive)
    return;

  if (
    millis() -
    lastExternalClockMs >
    EXTERNAL_CLOCK_TIMEOUT_MS
  )
  {
    externalClockActive = false;
    lastExternalPulseUs = 0;

    digitalWrite(
      PIN_EXT_CLOCK_LED,
      LOW
    );

    extClockLedOn = false;

    resetInternalClock();
  }
}


// =====================================================
// MIDI MESSAGE PROCESSING
// =====================================================

bool correctMidiChannel(uint8_t status)
{
  return
    (
      (
        status &
        0x0F
      ) + 1
    ) ==
    midiChannel;
}

void handleMidiMessage(
  uint8_t status,
  uint8_t data1,
  uint8_t data2
)
{
  lastMidiActivityMs = millis();

  if (
    setMode ||
    stepRecordLatched
  )
    return;

  uint8_t command =
    status &
    0xF0;

  if (
    command >= 0x80 &&
    command <= 0xE0
  )
  {
    if (!correctMidiChannel(status))
      return;
  }

  if (command == 0x90)
  {
    handleNoteOn(data1, data2);
    return;
  }

  if (command == 0x80)
  {
    handleNoteOff(data1);
    return;
  }

  if (command == 0xB0)
  {
    if (
      data1 == 120 ||
      data1 == 123
    )
    {
      clearNoteStack();

      stopSequencer();

      liveRecordState = LIVE_IDLE;
      liveInputSource = LIVE_SOURCE_NONE;

      arpGateActive = false;

      stopLink();
    }
  }
}


// =====================================================
// DIN MIDI IN -> DIN THRU
// =====================================================

void processDinMidiByte(uint8_t b)
{
  // Every DIN MIDI byte is sent to D1/TX1.
  Serial1.write(b);

  if (b >= 0xF8)
  {
    handleMidiRealtime(b);
    return;
  }

  if (b & 0x80)
  {
    dinRunningStatus = b;
    dinDataCount = 0;
    return;
  }

  if (dinRunningStatus == 0)
    return;

  uint8_t command =
    dinRunningStatus &
    0xF0;

  if (
    command == 0x80 ||
    command == 0x90 ||
    command == 0xB0
  )
  {
    if (dinDataCount == 0)
    {
      dinData1 = b;
      dinDataCount = 1;
    }
    else
    {
      handleMidiMessage(
        dinRunningStatus,
        dinData1,
        b
      );

      dinDataCount = 0;
    }
  }
}


// =====================================================
// USB MIDI -> DIN OUT
// =====================================================

void sendUsbMidiPacketToDin(
  const midiEventPacket_t &packet
)
{
  uint8_t cin =
    packet.header &
    0x0F;

  switch (cin)
  {
    // 1 byte
    case 0x5:
    case 0xF:
      Serial1.write(packet.byte1);
      break;

    // 2 bytes
    case 0x2:
    case 0x6:
    case 0xC:
    case 0xD:
      Serial1.write(packet.byte1);
      Serial1.write(packet.byte2);
      break;

    // 3 bytes
    case 0x3:
    case 0x4:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xE:
      Serial1.write(packet.byte1);
      Serial1.write(packet.byte2);
      Serial1.write(packet.byte3);
      break;

    default:
      break;
  }
}


// =====================================================
// USB MIDI
// =====================================================

void readUsbMidi()
{
  midiEventPacket_t rx;

  do
  {
    rx = MidiUSB.read();

    if (rx.header != 0)
    {
      // USB -> physical MIDI OUT
      sendUsbMidiPacketToDin(rx);

      // Local processing
      if (rx.byte1 >= 0xF8)
      {
        handleMidiRealtime(
          rx.byte1
        );
      }
      else
      {
        handleMidiMessage(
          rx.byte1,
          rx.byte2,
          rx.byte3
        );
      }
    }

  } while (rx.header != 0);
}


// =====================================================
// DIGIT DECODER
// =====================================================

int8_t linkCodeToDigit(uint8_t code)
{
  if (code == KEY_DIGIT_0) return 0;
  if (code == KEY_DIGIT_1) return 1;
  if (code == KEY_DIGIT_2) return 2;
  if (code == KEY_DIGIT_3) return 3;
  if (code == KEY_DIGIT_4) return 4;
  if (code == KEY_DIGIT_5) return 5;
  if (code == KEY_DIGIT_6) return 6;
  if (code == KEY_DIGIT_7) return 7;
  if (code == KEY_DIGIT_8) return 8;
  if (code == KEY_DIGIT_9) return 9;

  return -1;
}


// =====================================================
// MIDI CHANNEL SETUP
// =====================================================

void beginMidiSetup()
{
  programMode = PROGRAM_MIDI_CHANNEL;
  midiDigitCount = 0;

  parameterTone();
}

void processMidiDigit(uint8_t digit)
{
  midiDigits[
    midiDigitCount++
  ] = digit;

  if (midiDigitCount < 2)
    return;

  uint8_t channel =
    midiDigits[0] *
    10 +
    midiDigits[1];

  if (
    channel >= 1 &&
    channel <= 16
  )
  {
    midiChannel = channel;

    EEPROM.update(
      EEPROM_MIDI_CHANNEL_ADDR,
      midiChannel
    );

    acceptedTone();
  }
  else
  {
    errorBeep();
  }

  midiDigitCount = 0;
  programMode = PROGRAM_NONE;
}


// =====================================================
// TEMPO
// =====================================================

void beginTempoSetup()
{
  programMode = PROGRAM_TEMPO;
  tempoDigitCount = 0;

  parameterTone();
}

void processTempoDigit(uint8_t digit)
{
  tempoDigits[
    tempoDigitCount++
  ] = digit;

  if (tempoDigitCount < 3)
    return;

  uint16_t value =
    tempoDigits[0] *
    100 +
    tempoDigits[1] *
    10 +
    tempoDigits[2];

  if (
    value >= 30 &&
    value <= 300
  )
  {
    tempoBPM = value;

    EEPROM.put(
      EEPROM_TEMPO_ADDR,
      tempoBPM
    );

    if (!externalClockActive)
      resetInternalClock();

    acceptedTone();
  }
  else
  {
    errorBeep();
  }

  tempoDigitCount = 0;
  programMode = PROGRAM_NONE;
}


// =====================================================
// SEQ SETUP
// =====================================================

void beginSeqSetup()
{
  programMode = PROGRAM_SEQ_SETUP;

  clearHoldArmed = false;
  factoryResetDone = false;

  parameterTone();
}

void setSeqDivision(uint8_t division)
{
  seqDivision = division;

  EEPROM.update(
    EEPROM_SEQ_DIV_ADDR,
    seqDivision
  );

  acceptedTone();

  programMode = PROGRAM_NONE;
}

void setSeqBars(uint8_t bars)
{
  seqBars = bars;

  EEPROM.update(
    EEPROM_SEQ_BARS_ADDR,
    seqBars
  );

  acceptedTone();

  programMode = PROGRAM_NONE;
}

void toggleFreeLiveRecord()
{
  liveFreeStart = !liveFreeStart;

  EEPROM.update(
    EEPROM_FREE_REC_ADDR,
    liveFreeStart ? 1 : 0
  );

  if (liveFreeStart)
    onTone();
  else
    offTone();

  programMode = PROGRAM_NONE;
}


// =====================================================
// ARP SETUP
// =====================================================

void beginArpSetup()
{
  programMode = PROGRAM_ARP_SETUP;

  parameterTone();
}

void toggleArp()
{
  arpEnabled = !arpEnabled;

  EEPROM.update(
    EEPROM_ARP_ENABLE_ADDR,
    arpEnabled ? 1 : 0
  );

  clearNoteStack();
  stopLink();

  resetArpTraversal();

  if (arpEnabled)
    onTone();
  else
    offTone();

  programMode = PROGRAM_NONE;
}

void setArpDivision(uint8_t division)
{
  arpDivision = division;

  EEPROM.update(
    EEPROM_ARP_DIV_ADDR,
    arpDivision
  );

  resetArpTraversal();

  acceptedTone();

  programMode = PROGRAM_NONE;
}

void setArpType(uint8_t type)
{
  if (
    type < 1 ||
    type > 6
  )
  {
    errorBeep();

    programMode = PROGRAM_NONE;
    return;
  }

  arpType = type;

  EEPROM.update(
    EEPROM_ARP_TYPE_ADDR,
    arpType
  );

  resetArpTraversal();

  acceptedTone();

  programMode = PROGRAM_NONE;
}


// =====================================================
// CANCEL INCOMPLETE NORMAL SET PROGRAM
// =====================================================

bool cancelCurrentProgram()
{
  bool cancelled = false;

  if (programMode == PROGRAM_MIDI_CHANNEL)
  {
    midiDigitCount = 0;
    cancelled = true;
  }

  else if (programMode == PROGRAM_TEMPO)
  {
    tempoDigitCount = 0;
    cancelled = true;
  }

  else if (programMode == PROGRAM_SEQ_SETUP)
  {
    cancelled = true;
  }

  else if (programMode == PROGRAM_ARP_SETUP)
  {
    cancelled = true;
  }

  // Step Record survives SET release.
  programMode = PROGRAM_NONE;

  if (cancelled)
    cancelTone();

  return cancelled;
}


// =====================================================
// SET KEY PRESS
// =====================================================

void processSetKeyPress(uint8_t code)
{
  if (programMode == PROGRAM_NONE)
  {
    if (code == KEY_MIDI_SETUP)
    {
      suppressCurrentKeyRelease = true;
      beginMidiSetup();
      return;
    }

    if (code == KEY_TEMPO)
    {
      suppressCurrentKeyRelease = true;
      beginTempoSetup();
      return;
    }

    if (code == KEY_SEQ_SETUP)
    {
      suppressCurrentKeyRelease = true;
      beginSeqSetup();
      return;
    }

    if (code == KEY_STEP_REC)
    {
      suppressCurrentKeyRelease = true;
      beginStepRecord();
      return;
    }

    if (code == KEY_LIVE_REC)
    {
      liveRecPending = true;
      parameterTone();
      return;
    }

    if (code == KEY_ARP)
    {
      suppressCurrentKeyRelease = true;
      beginArpSetup();
      return;
    }

    if (code == KEY_PLAY)
    {
      playPending = true;
      return;
    }

    return;
  }

  int8_t digit =
    linkCodeToDigit(code);

  if (programMode == PROGRAM_MIDI_CHANNEL)
  {
    if (digit >= 0)
      processMidiDigit(digit);

    return;
  }

  if (programMode == PROGRAM_TEMPO)
  {
    if (digit >= 0)
      processTempoDigit(digit);

    return;
  }

  if (programMode == PROGRAM_SEQ_SETUP)
  {
    if (digit < 0)
      return;

    if (digit == 0)
    {
      clearHoldArmed = false;
      factoryResetDone = false;
      return;
    }

    if (
      digit >= 1 &&
      digit <= 3
    )
    {
      setSeqDivision(digit);
      return;
    }

    if (digit == 4)
    {
      setSeqBars(1);
      return;
    }

    if (digit == 5)
    {
      setSeqBars(2);
      return;
    }

    if (digit == 6)
    {
      setSeqBars(4);
      return;
    }

    if (digit == 7)
    {
      toggleFreeLiveRecord();
      return;
    }

    errorBeep();
    programMode = PROGRAM_NONE;
    return;
  }

  if (programMode == PROGRAM_ARP_SETUP)
  {
    if (digit < 0)
      return;

    if (digit == 0)
    {
      toggleArp();
      return;
    }

    if (
      digit >= 1 &&
      digit <= 3
    )
    {
      setArpDivision(digit);
      return;
    }

    if (
      digit >= 4 &&
      digit <= 9
    )
    {
      setArpType(
        digit - 3
      );

      return;
    }
  }
}


// =====================================================
// SILENT SET EXIT
// =====================================================

void exitSetModeSilent()
{
  setMode = false;
  programMode = PROGRAM_NONE;
  setKeyActive = false;
  suppressCurrentKeyRelease = false;
  restHoldConfirmed = false;
  clearHoldArmed = false;
  factoryResetDone = false;
  playPending = false;
  liveRecPending = false;
  midiDigitCount = 0;
  tempoDigitCount = 0;

  releaseLinkBus();
}


// =====================================================
// SET KEY RELEASE
// =====================================================

void processSetKeyRelease(
  uint8_t code,
  uint32_t heldMs
)
{
  if (
    playPending &&
    code == KEY_PLAY
  )
  {
    playPending = false;

    if (!sequenceValid)
    {
      errorBeep();
      return;
    }

    exitSetModeSilent();

    playbackStartCue();

    startSequencer();

    return;
  }

  if (
    liveRecPending &&
    code == KEY_LIVE_REC
  )
  {
    return;
  }

  if (suppressCurrentKeyRelease)
  {
    suppressCurrentKeyRelease = false;
    return;
  }

  if (
    programMode == PROGRAM_SEQ_SETUP &&
    code == KEY_DIGIT_0
  )
  {
    if (factoryResetDone)
      return;

    if (heldMs >= CLEAR_HOLD_MS)
    {
      clearSequence();
      programMode = PROGRAM_NONE;
      return;
    }

    cancelTone();
    programMode = PROGRAM_NONE;
    return;
  }

  if (programMode == PROGRAM_STEP_RECORD)
  {
    if (
      code == KEY_PLAY &&
      heldMs >= REST_HOLD_MS
    )
    {
      recordStep(SEQ_REST);
      return;
    }

    int16_t note =
      linkCodeToMidiNote(code);

    if (note >= 0)
      recordStep((uint8_t)note);
  }
}


// =====================================================
// JASPER CONFIG / STEP KEYBOARD READER
// =====================================================

void readJasperSetKeyboard()
{
  bool trigger =
    digitalRead(PIN_T);

  uint32_t nowMs =
    millis();

  if (
    trigger &&
    !lastKeyboardTrigger
  )
  {
    lastSetKeyPulseMs = nowMs;

    if (!setKeyActive)
    {
      delayMicroseconds(50);

      setKeyCode = readLinkCode();

      setKeyActive = true;
      setKeyPressMs = nowMs;

      suppressCurrentKeyRelease = false;
      restHoldConfirmed = false;
      clearHoldArmed = false;
      factoryResetDone = false;

      processSetKeyPress(setKeyCode);
    }
  }

  lastKeyboardTrigger = trigger;

  // Step Record REST indication
  if (
    setKeyActive &&
    programMode == PROGRAM_STEP_RECORD &&
    setKeyCode == KEY_PLAY &&
    !restHoldConfirmed &&
    nowMs - setKeyPressMs >= REST_HOLD_MS
  )
  {
    beepOnce();
    restHoldConfirmed = true;
  }

  // Clear armed
  if (
    setKeyActive &&
    programMode == PROGRAM_SEQ_SETUP &&
    setKeyCode == KEY_DIGIT_0 &&
    !clearHoldArmed &&
    nowMs - setKeyPressMs >= CLEAR_HOLD_MS
  )
  {
    clearHoldArmed = true;
    clearArmedTone();
  }

  // Factory reset
  if (
    setKeyActive &&
    programMode == PROGRAM_SEQ_SETUP &&
    setKeyCode == KEY_DIGIT_0 &&
    !factoryResetDone &&
    nowMs - setKeyPressMs >= FACTORY_RESET_HOLD_MS
  )
  {
    factoryResetDone = true;

    factoryReset();

    programMode = PROGRAM_NONE;
  }

  // Key release
  if (
    setKeyActive &&
    nowMs - lastSetKeyPulseMs > 50
  )
  {
    uint8_t code = setKeyCode;

    uint32_t held =
      nowMs -
      setKeyPressMs;

    setKeyActive = false;

    processSetKeyRelease(
      code,
      held
    );

    restHoldConfirmed = false;
    clearHoldArmed = false;
  }
}


// =====================================================
// SET MODE
// =====================================================

void enterSetMode()
{
  clearNoteStack();
  stopLink();

  setMode = true;
  programMode = PROGRAM_NONE;

  midiDigitCount = 0;
  tempoDigitCount = 0;

  setKeyActive = false;
  suppressCurrentKeyRelease = false;
  restHoldConfirmed = false;
  clearHoldArmed = false;
  factoryResetDone = false;
  playPending = false;
  liveRecPending = false;

  lastKeyboardTrigger =
    digitalRead(PIN_T);

  lastSetKeyPulseMs =
    millis();

  beep(1);
}


// =====================================================
// SET RELEASE
// =====================================================

void exitSetMode()
{
  bool startLive =
    liveRecPending;

  // Step Record stays active after SET release.
  if (
    stepRecordLatched &&
    programMode == PROGRAM_STEP_RECORD
  )
  {
    setMode = false;
    setKeyActive = false;
    suppressCurrentKeyRelease = false;
    restHoldConfirmed = false;
    clearHoldArmed = false;
    factoryResetDone = false;
    playPending = false;
    liveRecPending = false;
    midiDigitCount = 0;
    tempoDigitCount = 0;

    releaseLinkBus();

    lastKeyboardTrigger =
      digitalRead(PIN_T);

    lastSetKeyPulseMs =
      millis();

    return;
  }

  // Live Record
  if (startLive)
  {
    setMode = false;
    programMode = PROGRAM_NONE;
    setKeyActive = false;
    suppressCurrentKeyRelease = false;
    restHoldConfirmed = false;
    clearHoldArmed = false;
    factoryResetDone = false;
    playPending = false;
    liveRecPending = false;
    midiDigitCount = 0;
    tempoDigitCount = 0;

    releaseLinkBus();

    armLiveRecord();

    return;
  }

  // Normal SET program
  bool cancelled =
    cancelCurrentProgram();

  setMode = false;
  setKeyActive = false;
  suppressCurrentKeyRelease = false;
  restHoldConfirmed = false;
  clearHoldArmed = false;
  factoryResetDone = false;
  playPending = false;
  liveRecPending = false;
  midiDigitCount = 0;
  tempoDigitCount = 0;

  releaseLinkBus();

  if (cancelled)
    return;

  beep(3);
}


// =====================================================
// SET BUTTON
// =====================================================

void updateSetMode()
{
  bool pressed =
    (
      digitalRead(PIN_SET) ==
      SET_ACTIVE_LEVEL
    );

  // Press
  if (
    pressed &&
    !lastSetPressed
  )
  {
    // SET during standalone Step Record = cancel.
    if (stepRecordLatched)
    {
      cancelStepRecord();
      setStopLatch = true;
    }

    else if (
      seqPlaying ||
      liveRecordState != LIVE_IDLE
    )
    {
      stopSequencer();

      liveRecordState = LIVE_IDLE;
      liveInputSource = LIVE_SOURCE_NONE;

      autoPlaybackWaitingForJasperRelease = false;

      clearNoteStack();
      stopLink();

      setStopLatch = true;
    }

    else
    {
      enterSetMode();
    }
  }

  // Hold SET
  if (
    setMode &&
    pressed
  )
  {
    readJasperSetKeyboard();
  }

  // Release SET
  if (
    !pressed &&
    lastSetPressed
  )
  {
    if (setStopLatch)
    {
      setStopLatch = false;
    }
    else if (setMode)
    {
      exitSetMode();
    }
  }

  lastSetPressed = pressed;
}


// =====================================================
// FAILSAFE
// =====================================================

void updateFailsafe()
{
  if (!linkActive)
    return;

  if (
    setMode ||
    stepRecordLatched ||
    seqPlaying ||
    liveRecordState != LIVE_IDLE
  )
    return;

  if (
    millis() -
    lastMidiActivityMs >=
    MIDI_FAILSAFE_MS
  )
  {
    clearNoteStack();
    stopLink();
  }
}


// =====================================================
// LOAD SETTINGS
// =====================================================

void loadSettings()
{
  uint8_t ch =
    EEPROM.read(
      EEPROM_MIDI_CHANNEL_ADDR
    );

  if (
    ch >= 1 &&
    ch <= 16
  )
  {
    midiChannel = ch;
  }

  uint16_t savedTempo;

  EEPROM.get(
    EEPROM_TEMPO_ADDR,
    savedTempo
  );

  if (
    savedTempo >= 30 &&
    savedTempo <= 300
  )
  {
    tempoBPM = savedTempo;
  }

  uint8_t div =
    EEPROM.read(
      EEPROM_SEQ_DIV_ADDR
    );

  if (
    div >= 1 &&
    div <= 3
  )
  {
    seqDivision = div;
  }

  uint8_t bars =
    EEPROM.read(
      EEPROM_SEQ_BARS_ADDR
    );

  if (
    bars == 1 ||
    bars == 2 ||
    bars == 4
  )
  {
    seqBars = bars;
  }

  liveFreeStart =
    (
      EEPROM.read(
        EEPROM_FREE_REC_ADDR
      ) == 1
    );

  arpEnabled =
    (
      EEPROM.read(
        EEPROM_ARP_ENABLE_ADDR
      ) == 1
    );

  uint8_t type =
    EEPROM.read(
      EEPROM_ARP_TYPE_ADDR
    );

  if (
    type >= 1 &&
    type <= 6
  )
  {
    arpType = type;
  }

  uint8_t arpDiv =
    EEPROM.read(
      EEPROM_ARP_DIV_ADDR
    );

  if (
    arpDiv >= 1 &&
    arpDiv <= 3
  )
  {
    arpDivision = arpDiv;
  }
  else
  {
    arpDivision = 3;
  }

  loadSequence();
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  pinMode(
    PIN_INT_CLOCK_LED,
    OUTPUT
  );

  pinMode(
    PIN_EXT_CLOCK_LED,
    OUTPUT
  );

  digitalWrite(
    PIN_INT_CLOCK_LED,
    LOW
  );

  digitalWrite(
    PIN_EXT_CLOCK_LED,
    LOW
  );

  pinMode(
    PIN_BUZZER,
    OUTPUT
  );

  pinMode(
    PIN_SET,
    INPUT
  );

  // Fail-open LINK
  releaseLinkBus();

  // D0/RX1 = DIN MIDI IN
  // D1/TX1 = DIN MIDI OUT / THRU
  Serial1.begin(31250);

  clearNoteStack();

  loadSettings();

  resetArpTraversal();
  resetInternalClock();

  randomSeed(
    analogRead(A1)
  );

  lastMidiActivityMs =
    millis();

  lastSetPressed =
    (
      digitalRead(PIN_SET) ==
      SET_ACTIVE_LEVEL
    );
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // DIN MIDI IN / THRU
  while (Serial1.available())
  {
    processDinMidiByte(
      Serial1.read()
    );
  }

  // USB MIDI -> Jasper + DIN OUT
  readUsbMidi();

  // SET button
  updateSetMode();

  // Standalone Step Record after SET release.
  if (
    stepRecordLatched &&
    !setMode
  )
  {
    readJasperSetKeyboard();
  }

  // Jasper keyboard during Live Record.
  if (
    !setMode &&
    !stepRecordLatched &&
    (
      liveRecordState == LIVE_COUNT_IN ||
      liveRecordState == LIVE_WAIT_FIRST_NOTE ||
      liveRecordState == LIVE_RECORDING ||
      liveRecordState == LIVE_WAIT_JASPER_RELEASE
    )
  )
  {
    readJasperLiveKeyboard();
  }

  // Clock
  updateExternalClockTimeout();
  updateInternalClock();

  // Note gates
  updateSequencerGate();
  updateArpGate();

  // LINK output
  updateLinkTrigger();

  // Clock LEDs
  updateClockLeds();

  // Safety
  updateFailsafe();
}
