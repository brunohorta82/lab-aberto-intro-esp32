#include <Arduino.h>

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  static unsigned long timestamp = millis();
  if (millis() - timestamp > 1000ul)
  {
    int value = touchRead(13);
    if (value < 70 && value > 30)
    {
      Serial.println("Toque Leve");
    }
    else if (value < 30)
    {
      Serial.println("Toque forte");
    }
    Serial.println(value);
    timestamp = millis();
  }
}