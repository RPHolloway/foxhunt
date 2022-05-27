// Display Pins
#define LED0 8
#define LED1 9
#define LED2 10
#define LED3 11
#define NABL 12

// Zero-Crossing Trigger Pins
#define TRIGGER 2
#define TRIGGER_MODE RISING

// Antenna Switch Pins
#define ANTENNA0 A0
#define ANTENNA1 A1
#define ANTENNA2 A2
#define ANTENNA3 A3

// Filter Capacitor Pins
#define CAPACITOR0 7
#define CAPACITOR1 6
#define CAPACITOR2 5

#define TEST_SIGNAL 13

uint8_t state = 0;

#define SAMPLE_COUNT 1
uint8_t samples[SAMPLE_COUNT] = {0};
uint8_t* samplePtr = samples;

/* ---State Machine--- */
/**
 * @brief State machine to drive the antennas and capacitors.
 */
ISR(TIMER0_COMPA_vect)
{
  state &= 0b1111;

  switch (state)
  {
    case 0:
      AntennaSet(ANTENNA0);
      CapacitorSet(0);
      
      #ifdef TEST_SIGNAL
      digitalWrite(TEST_SIGNAL, LOW);
      #endif
      break;

    case 2:
      CapacitorSet(1);
      break;

    case 4:
      AntennaSet(ANTENNA1);
      CapacitorSet(2);
      break;

    case 6:
      CapacitorSet(3);
      break;

    case 8:
      AntennaSet(ANTENNA2);
      CapacitorSet(4);
      
      #ifdef TEST_SIGNAL
      digitalWrite(TEST_SIGNAL, HIGH);
      #endif
      break;

    case 10:
      CapacitorSet(5);
      break;

    case 12:
      AntennaSet(ANTENNA3);
      CapacitorSet(6);
      break;

    case 14:
      CapacitorSet(7);
      break;

    default:
      break;
  }

  state++;
}

/**
 * @brief Initialize the timer to drive the antenna and filter
 *        state machine.
 */
void StateMachineInit(void)
{
  cli(); // stop interrupts
  TCCR0A = 0; // clear TCCR0A
  TCCR0B = 0; // clear TCCR0B
  TCNT0 = 0; // initialize counter to 0
  OCR0A = 25; // (16*10^6) / (9600*64) - 1
  TCCR0A |= (1 << WGM01); // turn on CTC mode
  TCCR0B |= (1 << CS01) | (1 << CS00); // 64 bit prescaler
  TIMSK0 |= (1 << OCIE0A); // enable timer compare interrupt
  sei(); // start interrupts
}

/* ---Blink--- */
/**
 * @brief Timer 1 interrupt to blink the LED when the signal is lost.
 */
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(NABL, !digitalRead(NABL));
}

/**
 * @brief Initialize the timer to blink the LED when the signal is lost.
 */
void BlinkInit(void)
{
  cli(); // stop interrupts
  TCCR1A = 0; // clear TCCR0A
  TCCR1B = 0; // clear TCCR0B
  TCNT1 = 0; // initialize counter to 0
  OCR1A = 7812; // (16*10^6) / (2*1024) - 1
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 bit prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei(); // start interrupts
}

/* ---Display--- */
/**
 * @brief Set the display to display the choosen LED.
 * 
 * @param led LED number (0-15) to display.
 */
void DisplaySet(uint8_t led)
{
  digitalWrite(NABL, LOW);
  digitalWrite(LED0, (led & 0b0001) >> 0);
  digitalWrite(LED1, (led & 0b0010) >> 1);
  digitalWrite(LED2, (led & 0b0100) >> 2);
  digitalWrite(LED3, (led & 0b1000) >> 3);
}

/**
 * @brief Turn off the display
 */
void DisplayOff(void)
{
  digitalWrite(NABL, HIGH);
}

/**
 * @brief Initialize the display
 */
void DisplayInit(void)
{
  pinMode(NABL, OUTPUT);
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

/*--- Trigger ---*/
/**
 * @brief Zero crossing interrupt
 */
void Trigger(void)
{
  // If the samplePtr has passed the top of the buffer
  // reset it to the front of the buffer.
  if (samplePtr == (samples + SAMPLE_COUNT))
  {
    samplePtr = samples;
  }
  
  // Sample the state
  *samplePtr = state;
  // Move the pointer to the next slot in the buffer
  samplePtr++;

  // Sum the buffer
  uint8_t total = 0;
  for (uint8_t i=0; i < SAMPLE_COUNT; i++)
  {
    total += samples[i];
  }

  // Display the average value of the buffer
  DisplaySet(total/SAMPLE_COUNT);
  TCNT1 = 0;
}

/**
 * @brief Initialize the zero crossing trigger interrupt
 */
void TriggerInit(void)
{
  pinMode(TRIGGER, INPUT);
  attachInterrupt(digitalPinToInterrupt(TRIGGER), Trigger, TRIGGER_MODE);
}

/* ---Antenna--- */
/**
 * @brief Turn off all antenna switches.
 */
void AntennaOff(void)
{
  digitalWrite(ANTENNA0, LOW);
  digitalWrite(ANTENNA1, LOW);
  digitalWrite(ANTENNA2, LOW);
  digitalWrite(ANTENNA3, LOW);
}

/**
 * @brief Turn foo all antenna switches and turn on
 *        the requested interrupt.
 * 
 * @param antenna pin number of the antenna switch to enable
 */
void AntennaSet(uint8_t antenna)
{
  AntennaOff();
  digitalWrite(antenna, HIGH);
}

/**
 * @brief Initialize the antenna switches.
 */
void AntennaInit(void)
{
  pinMode(ANTENNA0, OUTPUT);
  pinMode(ANTENNA1, OUTPUT);
  pinMode(ANTENNA2, OUTPUT);
  pinMode(ANTENNA3, OUTPUT);

  digitalWrite(ANTENNA0, LOW);
  digitalWrite(ANTENNA1, LOW);
  digitalWrite(ANTENNA2, LOW);
  digitalWrite(ANTENNA3, LOW);
}

/* ---Capacitors--- */
/**
 * @brief Set the filter to use the choosen capacitor.
 * 
 * @param capacitor capacitor (0-7) to use
 */
void CapacitorSet(uint8_t capacitor)
{
  digitalWrite(CAPACITOR0, (capacitor & 0b001) >> 0);
  digitalWrite(CAPACITOR1, (capacitor & 0b010) >> 1);
  digitalWrite(CAPACITOR2, (capacitor & 0b100) >> 2);
}

/**
 * @brief Initialize the filter capacitors
 */
void CapacitorInit(void)
{
  pinMode(CAPACITOR0, OUTPUT);
  pinMode(CAPACITOR1, OUTPUT);
  pinMode(CAPACITOR2, OUTPUT);

  digitalWrite(CAPACITOR0, LOW);
  digitalWrite(CAPACITOR1, LOW);
  digitalWrite(CAPACITOR2, LOW);
}

void setup() {
  #ifdef TEST_SIGNAL
  pinMode(TEST_SIGNAL, OUTPUT);
  digitalWrite(TEST_SIGNAL, LOW);
  #endif
  
  AntennaInit();
  CapacitorInit();
  DisplayInit();
  BlinkInit();
  TriggerInit();
  StateMachineInit();
}

void loop() { }