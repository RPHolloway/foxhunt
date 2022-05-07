#define LED0 8
#define LED1 9
#define LED2 10
#define LED3 11
#define NABL 12

#define TIMER 13
#define TRIGGER 2

uint8_t state = 0;

ISR(TIMER0_COMPA_vect)
{
  digitalWrite(TIMER, !digitalRead(TIMER));

  state &= 0b1111;

  switch (state)
  {
    case 0:
      // Turn off antenna 3
      // Turn on antenna 0
      // Turn on capacitor 0
      break;

    case 2:
      // Turn on capacitor 1
      break;

    case 4:
      // Turn off antenna 0
      // Turn on antenna 1
      // Turn on capacitor 2
      break;

    case 6:
      // Turn on capacitor 3
      break;

    case 8:
      // Turn off antenna 1
      // Turn on antenna 2
      // Turn on capacitor 4
      break;

    case 10:
      // Turn on capactior 5
      break;

    case 12:
      // Turn off antenna 2
      // Turn on antenna 3
      // Turn on capacitor 6
      break;

    case 14:
      // Turn on capacitor 7
      break;

    default:
      break;
  }

  state++;
}

void TimerInit(void)
{
  pinMode(TIMER, OUTPUT);

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

void Display(uint8_t led)
{
  digitalWrite(LED0, (led & 0b0001) >> 0);
  digitalWrite(LED1, (led & 0b0010) >> 1);
  digitalWrite(LED2, (led & 0b0100) >> 2);
  digitalWrite(LED3, (led & 0b1000) >> 3);
}

void DisplayOff(void)
{
  digitalWrite(NABL, HIGH);
}

void DisplayInit(void)
{
  pinMode(NABL, OUTPUT);
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void Trigger(void)
{
  Display(state);
}

void TriggerInit(void)
{
  pinMode(TRIGGER, INPUT);
  attachInterrupt(digitalPinToInterrupt(TRIGGER), Trigger, RISING);
}

void setup() {
  DisplayInit();
  TriggerInit();
  TimerInit();
}

void loop() { }