//Including Libraries
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
TinyGPSPlus gps;        // The TinyGPS++ object
SoftwareSerial ss(3, 4);// The serial connection to the GPS device
double latitude;
double longitude;

SoftwareSerial serialSIM800(5, 14);// The serial connection to the GSM device
//double latitude;

Adafruit_MPU6050 mpu;

//Declaring sensor Pins
byte buzzer = 11;
byte led = 15;
byte  shock_Sensor = 16;
byte  flame_Sensor = 6;

//Declaring status variables
byte  shock_Sensor_State = 0;
byte  flame_Sensor_State = 1;
byte  is_Car_Flipped = 0;
byte  is_Shoked = 0;
byte  is_Flame = 1;
byte is_Special_Scenario = 0;
byte is_Careless = 0;


void setup() {
  // Declaring I/O Devices
  pinMode(shock_Sensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  serialSIM800.begin(9600); //Start software Serial for GSM
  ss.begin(9600);//Start software Serial for GPS

  while (!Serial);

  // Try to initialize!
  if (!mpu.begin()) {
    while (1) {
      delay(10);
    }
  }

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);

  lcd.init();// Initialize LCD
  
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Accident Alert");
  lcd.setCursor(5, 1);
  lcd.print("System");
}

void loop() {

  read_Shock();
  if (is_Shoked == 1) {
    tone(buzzer, 1000); // Send 1KHz sound signal...
    digitalWrite(led, HIGH); // Turn ON LED
    delay(800);
    noTone(buzzer);     // Stop sound...
    is_Careless = 0;

    unsigned long start = millis();
    int shock_Increment = 0;
    do {

      lcd.clear();
      lcd.setCursor(5, 1);
      int fifteen_Countdown = (15000 - (millis() - start)) / 1000;
      lcd.print(fifteen_Countdown);
      lcd.print(" Sec");
      delay(50);

      shock_Increment++;

      read_Gyro();
      read_Flame();

      if (is_Car_Flipped == 1) {
        byte  a = 0;
        do {
          a++;
          delay(50);

          lcd.clear();
          lcd.setCursor(5, 1);
          lcd.print((5000 - (a * 50)) / 1000);
          lcd.print(" Sec");

          read_Flame();
          if (is_Flame == 1) {
            is_Special_Scenario = 1;
          }

        } while (a < 100);
        // run accident execution

        if (is_Special_Scenario == 1) {
          C_R_F();
          is_Special_Scenario = 0;
        }
        else {
          C_R();
        }
        //C_R();
      }

      if (is_Flame == 1) {
        byte  b = 0;

        do {       
          b++;
          delay(50);

          lcd.clear();
          lcd.setCursor(5, 1);
          lcd.print((10000 - (b * 50)) / 1000);
          lcd.print(" Sec");

          read_Gyro();
          if (is_Car_Flipped == 1) {
            byte  a = 0;
            do {
              a++;
              delay(50);

              lcd.clear();
              lcd.setCursor(5, 1);
              lcd.print((5000 - (a * 50)) / 1000);
              lcd.print(" Sec");
            } while (a < 100);
            // run accident execution
            C_R_F();
          }

        } while (b < 200);
        // run * accident execution
        C_F();
      }




    } while (millis() - start < 15000);
    //run possible accident execution
    C();
  }


  read_Gyro();
  if (is_Car_Flipped == 1) {
    tone(buzzer, 1000); // Send 1KHz sound signal...
    digitalWrite(led, HIGH);
    delay(800);
    noTone(buzzer);     // Stop sound...
    is_Careless = 1;
    int flip_Increment = 0;
    unsigned long start = millis();
    do {

      lcd.clear();
      lcd.setCursor(5, 1);
      int fifteen_Countdown = (15000 - (millis() - start)) / 1000;
      lcd.print(fifteen_Countdown);
      lcd.print(" Sec");
      delay(50);//50

      flip_Increment++;

      read_Shock();
      read_Flame();

      if (is_Shoked == 1) {
        byte  a = 0;
        do {
          a++;
          delay(50);

          lcd.clear();
          lcd.setCursor(5, 1);
          lcd.print((5000 - (a * 50)) / 1000);
          lcd.print(" Sec");
          //   Serial.println("Inside 5 sec count down");

          read_Flame();
          if (is_Flame == 1) {
            is_Special_Scenario = 1;
          }

        } while (a < 100);
        if (is_Special_Scenario == 1) {
          C_R_F();
          is_Special_Scenario = 0;
        }
        else {
          C_R();
        }

      }


      if (is_Flame == 1) {
        byte  b = 0;

        do {
          b++;
          delay(50);

          lcd.clear();
          lcd.setCursor(5, 1);
          lcd.print((10000 - (b * 50)) / 1000);
          lcd.print(" Sec");


          read_Shock();
          if (is_Shoked == 1) {
            byte  a = 0;
            do {
              a++;
              delay(50);

              lcd.clear();
              lcd.setCursor(5, 1);
              lcd.print((5000 - (a * 50)) / 1000);
              lcd.print(" Sec");

            } while (a < 100);
            C_R_F();
          }

        } while (b < 200);
        R_F();
        // run * accident execution
      }

    } while (millis() - start < 15000);
    //run possible accident execution
    R();

  }


  delay(100);
}

void read_Shock() {
  shock_Sensor_State = digitalRead(shock_Sensor);
  if (shock_Sensor_State == 1) {
    is_Shoked = 1;
  }
  else {
    is_Shoked = 0;
  }
}

void read_Flame() {
  flame_Sensor_State = digitalRead(flame_Sensor);
  if (flame_Sensor_State == 0) {
    is_Flame = 1;
  }
  else {
    is_Flame = 0;
  }
}

void read_Gyro() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  if ((a.acceleration.y) < -3) {
    //  Serial.println("Right Flip");
    is_Car_Flipped = 1;
  }
  else if ((a.acceleration.y) > 3) {
    //  Serial.println("Left Flip");
    is_Car_Flipped = 1;
  }
  else if ((a.acceleration.x) < -3) {
    // Serial.println("Back Flip");
    is_Car_Flipped = 1;
  }
  else if ((a.acceleration.x) > 3) {
    // Serial.println("Front Flip");
    is_Car_Flipped = 1;
  }

  else {
    is_Car_Flipped = 0;
  }
}

void find_GPS() {
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());

  } while (millis() - start < 2000);



  if (gps.location.isUpdated()) {

    latitude = (gps.location.lat());
    longitude = (gps.location.lng());

  }

  Serial.println(latitude, 6);
  Serial.println(longitude, 6);
}

void C_R_F() {
  show_Accident_Detected_LCD();

  find_GPS();

  for (int i = 0; i < 2; i++) {
    while (!Serial);

    //Being serial communication with Arduino and SIM800
    serialSIM800.begin(9600);
    delay(1000);

    //Set SMS format to ASCII
    serialSIM800.write("AT+CMGF=1\r\n");
    delay(1000);
    if (i == 0) {
      //Send new SMS command and message number
      serialSIM800.write("AT+CMGS=\"0768217964\"\r\n");  ///////////////////////////////////////////////////// Emergency Unit Number
    }
    else {
      serialSIM800.write("AT+CMGS=\"0705608924\"\r\n");  ///////////////////////////////////////////////////// Fire Brigade
    }
    delay(1000);


    //Send SMS content
    if (is_Careless == 1) {
      serialSIM800.write("A Car has faced a Collision, a Rollover and a fire due to the Carelessness in following location: ");
    }
    else {
      serialSIM800.write("A Car has faced a Collision, a Rollover and a fire in following location: ");
    }

    serialSIM800.write("https://www.google.com/maps/place/");
    delay(1000);
    serialSIM800.print(latitude, 6); //Send SMS content
    serialSIM800.print(",");     //Send SMS content
    serialSIM800.print(longitude, 6); //Send SMS content

    //Send Ctrl+Z / ESC to denote SMS message is complete
    serialSIM800.write((char)26);
    delay(1000);
    delay(5000);
  }
  asm volatile ("  jmp 0"); // Reset Arduino

}

void C_R() {
  show_Accident_Detected_LCD();
  find_GPS();

  while (!Serial);

  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);

  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);

  //Send new SMS command and message number
  serialSIM800.write("AT+CMGS=\"0768217964\"\r\n");  ///////////////////////////////////////////////////////////// Emergency Unit Number
  delay(1000);


  //  //Send SMS content
  if (is_Careless == 1) {
    serialSIM800.write("A Car has faced a Collision, and a Rollover due to the Carelessness in following location: ");
  }

  else {
    serialSIM800.write("A Car has faced a Collision, and a Rollover in following location: ");
  }

  serialSIM800.write("https://www.google.com/maps/place/");
  delay(1000);
  serialSIM800.print(latitude, 6); //Send SMS content
  serialSIM800.print(",");     //Send SMS content
  serialSIM800.print(longitude, 6); //Send SMS content

  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);

  delay(100);        // delay in between reads for stability
  asm volatile ("  jmp 0");
}

void C_F() {
  show_Accident_Detected_LCD();
  find_GPS();

  for (int i = 0; i < 2; i++) {

    while (!Serial);

    //Being serial communication with Arduino and SIM800
    serialSIM800.begin(9600);
    delay(1000);

    //Set SMS format to ASCII
    serialSIM800.write("AT+CMGF=1\r\n");
    delay(1000);

    //Send new SMS command and message number
    if (i == 0) {
      serialSIM800.write("AT+CMGS=\"0768217964\"\r\n");    ///////////////////////////////////////////////////////// Emergency Unit
    }
    else {
      serialSIM800.write("AT+CMGS=\"0705608924\"\r\n");  ///////////////////////////////////////////////////////////// Fire Brigade
    }
    delay(1000);

    //Send SMS content
    serialSIM800.write("A Car has faced a Collision and a fire at following location : ");
    serialSIM800.write("https://www.google.com/maps/place/");
    delay(1000);
    serialSIM800.print(latitude, 6); //Send SMS content
    serialSIM800.print(",");     //Send SMS content
    serialSIM800.print(longitude, 6); //Send SMS content

    //Send Ctrl+Z / ESC to denote SMS message is complete
    serialSIM800.write((char)26);
    delay(1000);

    // Serial.println("SMS Sent!");
    //  delay(100);        // delay in between reads for stability
    delay(5000);
  }

  asm volatile ("  jmp 0");// Reset Arduino
}

void C() {
  show_Accident_Detected_LCD();
  find_GPS();

  while (!Serial);

  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);

  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);

  //Send new SMS command and message number
  serialSIM800.write("AT+CMGS=\"0768217964\"\r\n");  ///////////////////////////////////////////////////////////// Emergency Unit Number
  delay(1000);


  //Send SMS content
  serialSIM800.write("A Car has faced a Collision in following location: ");
  serialSIM800.write("https://www.google.com/maps/place/");
  delay(1000);
  serialSIM800.print(latitude, 6); //Send SMS content
  serialSIM800.print(",");     //Send SMS content
  serialSIM800.print(longitude, 6); //Send SMS content

  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);

  // Serial.println("SMS Sent!");
  delay(100);        // delay in between reads for stability
  asm volatile ("  jmp 0");// Reset Arduino
}



void R_F() {
  show_Accident_Detected_LCD();
  find_GPS();


  for (int i = 0; i < 2; i++) {
    while (!Serial);

    //Being serial communication with Arduino and SIM800
    serialSIM800.begin(9600);
    delay(1000);

    //Set SMS format to ASCII
    serialSIM800.write("AT+CMGF=1\r\n");
    delay(1000);

    //Send new SMS command and message number
    if (i == 0) {
      serialSIM800.write("AT+CMGS=\"0768217964\"\r\n");    /////////////////////////////////////////////////////////// Emergency Unit Number
    }
    else {
      serialSIM800.write("AT+CMGS=\"0705608924\"\r\n");  ////////////////////////////////////////////////////////////// Fire Brigade
    }
    delay(1000);

    //Send SMS content
    serialSIM800.write("A Car has faced a Rollover and a fire at following location : ");
    serialSIM800.write("https://www.google.com/maps/place/");
    delay(1000);
    serialSIM800.print(latitude, 6); //Send SMS content
    serialSIM800.print(",");     //Send SMS content
    serialSIM800.print(longitude, 6); //Send SMS content

    //Send Ctrl+Z / ESC to denote SMS message is complete
    serialSIM800.write((char)26);
    delay(1000);
    delay(5000);

  }

  asm volatile ("  jmp 0");// Reset Arduino
}

void R() {
  show_Accident_Detected_LCD();
  find_GPS();

  while (!Serial);

  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);

  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);

  //Send new SMS command and message number
  serialSIM800.write("AT+CMGS=\"0768217964\"\r\n");    ////////////////////////////////////////////////////////////// Emergency Unit Number
  delay(1000);

  //Send SMS content
  serialSIM800.write("A Car has faced a Rollover at following location : ");
  serialSIM800.write("https://www.google.com/maps/place/");
  delay(1000);
  serialSIM800.print(latitude, 6); //Send SMS content
  serialSIM800.print(",");     //Send SMS content
  serialSIM800.print(longitude, 6); //Send SMS content

  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);

  // Serial.println("SMS Sent!");
  delay(100);        // delay in between reads for stability

  asm volatile ("  jmp 0");// Reset Arduino
}

void show_Accident_Detected_LCD() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Accident");
  lcd.setCursor(3, 1);
  lcd.print("Detected.!");
}
