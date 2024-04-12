# arduino-test

## easy func examples

+ ```pinMode(3, OUTPUT)```: set pin 3 to output
+ ```digitalWrite(3, HIGH)```: turn on pin 3 (if 4 example is a LED)
+ ```delay(500)```: wait 500 ms (nop)
+ ```pinMode(2, INPUT)```: set pin 2 as input (ex btn)
+ ```digitalRead(2)```: read value
+ ```digitalWrite(3, digitalRead(2))```: ex of turn on/off with a btn
+ ```int button```: var declaration
+ ```Serial.begin(9600)```: 9600 symbol / seconds, initialization
+ ```Serial.println("Hello Arduino!")```: write to serial

## PWM - Pulse width Modulation

+ enabled pins: 3 5 6 9 10 11
+ ```analogWrite(3, range)```: range 0, 255

## easy func (cont.)

+ ```int pot = analogRead(A0)```: ex from a potentiometer
+ ```int pwm = map(pot, 0, 1023, 0, 255)```: ex of mapping

## syntax

+ ```for```: like c++
+ ```variables```: like c++
+ ```if```: like c++
