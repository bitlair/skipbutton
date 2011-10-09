#define BAUDRATE 600UL

enum msgcode
{
  MSG_START   = 0x55,
  MSG_END     = 0xCC,
  MSG_SKIP    = 0x01,
  MSG_VOLDOWN = 0x02,
  MSG_VOLUP   = 0x03
};

void setup()
{
  //power pin, make high to keep the mosfet open
  digitalWrite(2, HIGH);

  //bootloader select pin, make input
  pinMode(6, INPUT);
  digitalWrite(6, LOW);

  //skip pin
  pinMode(5, INPUT);
  digitalWrite(5, HIGH); //enable internal pullup

  //voldown pin
  pinMode(4, INPUT);
  digitalWrite(4, HIGH); //enable internal pullup

  //volup pin
  pinMode(3, INPUT);
  digitalWrite(3, HIGH); //enable internal pullup

  //tx pin
  digitalWrite(1, HIGH); //make high, disable the transmitter
}

void loop()
{
  uint8_t buttons = (button(5) << 2) | (button(4) << 1) | button(3);

  if (buttons == 4) //skip
  {
    digitalWrite(2, HIGH);
    Transmit(MSG_SKIP);
  }
  else if (buttons == 2) //voldown
  {
    digitalWrite(2, HIGH);
    Transmit(MSG_VOLDOWN);
  }
  else if (buttons == 1) //volup
  {
    digitalWrite(2, HIGH);
    Transmit(MSG_VOLUP);
  }
  else //combination or nothing, meh
  {
    //turn power off
    digitalWrite(2, LOW);
  }

  DelayMicroseconds(100000);
}

void Transmit(uint8_t code)
{
  digitalWrite(1, LOW);
  DelayMicroseconds(15000000UL / BAUDRATE);
  digitalWrite(1, HIGH);
  DelayMicroseconds(15000000UL / BAUDRATE);

  Serial.begin(BAUDRATE);
  
  Serial.write((uint8_t)MSG_START);
  Serial.write((uint8_t)~MSG_START);
  Serial.write((uint8_t)code);
  Serial.write((uint8_t)~code);
  Serial.write((uint8_t)MSG_END);
  Serial.write((uint8_t)~MSG_END);

  Serial.end();
}

void DelayMicroseconds(unsigned long delay)
{
  unsigned long now = micros();
  while ((micros() - now) < delay);
}

//invert pin, low is button pressed
uint8_t button(uint8_t pin)
{
  if (digitalRead(pin))
    return 0;
  else
    return 1;
}
