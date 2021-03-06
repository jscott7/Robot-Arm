// Listen to bytes on serial port and drive motor shield accordingly
// Based on sample found here: http://playground.arduino.cc/Main/AdafruitMotorShield

// 8-bit bus after the 74HC595 shift register (not Arduino pins)
// These are used to set the direction of the bridge driver
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_A 0
#define MOTOR4_B 6

// Arduino pins for the shift register
#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8

// Arduino pins for the PWM signals
#define MOTOR1_PWM 11
#define MOTOR2_PWM 3
#define MOTOR3_PWM 6
#define MOTOR4_PWM 5

// Codes for motor function
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

byte inByte;

void setup()
{
  Serial.begin(9600);
  Serial.println("Started");
}

void loop()
{
  while(Serial.available() > 0)
  {
    inByte = Serial.read();
    
    // Debugging
    if (inByte > 0)
    {
      Serial.println(inByte);
    }
  }

  if (inByte == 97 ) // 'a'
  {
    motor(1, FORWARD, 255);
  }
  if (inByte == 115) // 's'
  {
    motor(1, BACKWARD, 255);
  }
  if (inByte == 101) //'e'
  {
    motor(2, FORWARD, 255);
  }
  if (inByte == 100) // 'd'
  {
    motor(2, BACKWARD, 255);
  }
  if (inByte == 116) //'t'
  {
    motor(3, FORWARD, 255);
  }
  if (inByte == 103) // 'g'
  {
    motor(3, BACKWARD, 255);
  } 
  if (inByte == 117) //'u'
  {
    motor(4, FORWARD, 255);
  }
  if (inByte == 106) // 'j'
  {
    motor(4, BACKWARD, 255);
  } 
  if (inByte == 32)
  {
     motor(1, RELEASE, 0);
     motor(2, RELEASE, 0);
     motor(3, RELEASE, 0);
     motor(4, RELEASE, 0);
  }
  
}

void motor(int nMotor, int command, int speed)
{
  int motorA, motorB;
  if (nMotor >=1 && nMotor <=4)
  {
    switch(nMotor)
    {
      case 1:
        motorA = MOTOR1_A;
        motorB = MOTOR1_B;
        break;
      case 2:
        motorA = MOTOR2_A;
        motorB = MOTOR2_B;
        break;
      case 3:
        motorA = MOTOR3_A;
        motorB = MOTOR3_B;
        break;
      case 4:
        motorA = MOTOR4_A;
        motorB = MOTOR4_B;
        break;
      default:
        break;
    }
    
    switch(command)
    {
      case FORWARD:
        motor_output(motorA, HIGH, speed);
        motor_output(motorB, LOW, -1);
        break;
      case BACKWARD:
        motor_output(motorA, LOW, speed);
        motor_output(motorB, HIGH, -1);
        break;
      case RELEASE:
        motor_output(motorA, LOW, 0);
        motor_output(motorB, LOW, -1);
        break;
      default:
        break;
    }
  }
}

void motor_output(int output, int high_low, int speed)
{
  int motorPWM;
  
  switch (output)
  {
   case MOTOR1_A:
   case MOTOR1_B:
     motorPWM = MOTOR1_PWM;
     break;
   case MOTOR2_A:
   case MOTOR2_B:
      motorPWM = MOTOR2_PWM;
      break;
   case MOTOR3_A:
   case MOTOR3_B:
      motorPWM = MOTOR3_PWM;
      break;
   case MOTOR4_A:
   case MOTOR4_B:
      motorPWM = MOTOR4_PWM;
      break;
   default:
     // Use apeed as error flag, -3333 = invalid output
     speed = -3333;
     break;
  }
  
  if (speed != 3333)
  {
    // Set the direction of the shift regester on the motorshield, 
    // even if the speed = -1
    // In that case direction will be set but not the PWM
    
    shiftWrite(output, high_low);
    
    // set the PWM only if it is valid
    if (speed >= 0 && speed <= 255)
    {
      analogWrite(motorPWM, speed);
    }
  }
}

// The output is pin 0..7 (pin behind the shift register)
void shiftWrite(int output, int high_low)
{
 static int latch_copy;
 static int shift_register_initialised = false;
 
 if (!shift_register_initialised)
 { 
   // set pins for shift register to output
   pinMode(MOTORLATCH, OUTPUT);
   pinMode(MOTORENABLE, OUTPUT);
   pinMode(MOTORDATA, OUTPUT);
   pinMode(MOTORCLK, OUTPUT);
   
   // default value (low)
   digitalWrite(MOTORDATA, LOW);
   digitalWrite(MOTORLATCH, LOW);
   digitalWrite(MOTORCLK, LOW);
   
   // enable the shift register, set enable pin low
   digitalWrite(MOTORENABLE, LOW);
 
   // start with all output of the shift register low
   latch_copy = 0; 
   
   shift_register_initialised = true;
 }
 
 bitWrite(latch_copy, output, high_low);
 
 // Use the default Ardunio shiftOut() function to shift the bits with
 // MOTORCLK as clock pulse
 // the 74HC595 shiftregister wants the MSB first
 // After that, generate a latch pulse with MOTORLATCH
 shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
 delayMicroseconds(5); // Safety
 digitalWrite(MOTORLATCH, HIGH);
 delayMicroseconds (5);
 digitalWrite(MOTORLATCH, LOW);

}
