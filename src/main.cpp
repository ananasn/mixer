// #include <Arduino.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <TimerMs.h>
// #include "GyverRelay.h"


// bool DEBUG = true;


// // PINS list
// int THERMAL_SENSOR = 4;
// int MIXER_PIN = 26;
// int PUMP_LEFT_PIN = 32;
// int PUMP_RIGHT_PIN = 33;
// int HEATER_PIN = 27;
// int LEFT_HEATER_PIN = 22;
// int RIGHT_HEATER_PIN = 21;

// // Network variables list
// bool start;
// bool pump_left;
// bool pump_right;
// bool mixer;
// bool heater;
// bool left_heater;
// bool right_heater;
// float tleft;
// float tright;
// float tcenter;
// float tctrl;

// // Constants settings
// float tleft_target = 50;
// float tright_target = 58;
// float tcenter_target = 60;
// float time_left = 168000;
// float time_right = 192000;
// float time_wait = 300000;

// // States
// const uint8_t INITIAL = 0;
// const uint8_t INIIAL_HEAT = 1;
// const uint8_t FLOW = 2;
// const uint8_t MAIN_HEAT = 3;
// const uint8_t WAIT = 4;
// uint8_t state = INITIAL;

// // Timers
// TimerMs timer_left(time_left, 0, 1);
// TimerMs timer_right(time_right, 0, 1);
// TimerMs timer_wait(time_wait, 0, 1);

// // Sensors
// OneWire  ds(THERMAL_SENSOR);
// DallasTemperature sensors(&ds);
// byte addr_tleft[8] = {0x28, 0xff, 0xed, 0x7, 0xd6, 0x1, 0x3c, 0xe9};
// byte addr_tright[8] = {0x28, 0x78, 0xbf, 0x7, 0xd6, 0x1, 0x3c, 0x90};
// byte addr_tcenter[8] = {0x28, 0x3d, 0x30, 0x7, 0xd6, 0x1, 0x3c, 0xaa};
// byte addr_tctrl[8] = {0x28, 0xd1, 0x82, 0x7, 0xd6, 0x1, 0x3c, 0x32};
// byte addr[8];

// //Regulators
// GyverRelay lregulator(REVERSE);
// GyverRelay rregulator(REVERSE);


// void setup() 
// {
//   // Set right heater regulator parameters
//   pinMode(LEFT_HEATER_PIN, OUTPUT);
//   pinMode(RIGHT_HEATER_PIN, OUTPUT);
//   rregulator.setpoint = tright_target;
//   rregulator.hysteresis = 1;
//   rregulator.k = 0;

//   //Set left heater regulator parameters
//   lregulator.setpoint = tleft_target;
//   lregulator.hysteresis = 1;
//   lregulator.k = 0;

//   Serial.begin(115200);
//   sensors.setResolution(12);
//   timer_left.setTimerMode();
//   timer_right.setTimerMode();
//   timer_wait.setTimerMode();
//   pinMode(PUMP_LEFT_PIN, OUTPUT);
//   pinMode(PUMP_RIGHT_PIN, OUTPUT);
//   pinMode(HEATER_PIN, OUTPUT);
//   pinMode(MIXER_PIN, OUTPUT);
//   start = true;
// }


// // Debug function for 1-wire adresses exploring
// void search_addrs() 
// {
//   while (true) 
//   {
//     if ( ds.search(addr)) 
//     {
//       Serial.println("ADDR:");
//         for (int i=0; i < 8; i++) 
//         {
//           Serial.print(addr[i], HEX);
//           Serial.print(" ");
//         }
//       Serial.println();
//     }
//     else 
//     {
//       Serial.println("No more addresses.");
//       Serial.println();
//       ds.reset_search();
//       delay(250);
//       return;
//     }
//   }
// }

// // Prints all status variables to Serial
// void info() 
// {
//   Serial.print(tleft);
//   Serial.print(" ");
//   Serial.print(tright);
//   Serial.print(" ");
//   Serial.print(tcenter);
//   Serial.print(" ");
//   Serial.print(tctrl);
//   Serial.print(" ");
//   Serial.print(pump_left);
//   Serial.print(" ");
//   Serial.print(pump_right);
//   Serial.print(" ");
//   Serial.print(mixer);
//   Serial.print(" ");
//   Serial.print(heater);
//   Serial.print(" ");
//   Serial.print(left_heater);
//   Serial.print(" ");
//   Serial.println(right_heater);
// }

// void regulate_left_heater(boolean value) {
//   digitalWrite(LEFT_HEATER_PIN, value);
//   left_heater = value;
// }

// void regulate_right_heater(boolean value) {
//   digitalWrite(RIGHT_HEATER_PIN, value);
//   right_heater = value;
// }

// // Reads temperatures from 1-wire bus
// void read_temperature() 
// {
//   sensors.requestTemperatures();
//   delay(200);
//   tleft = sensors.getTempC(addr_tleft);
//   tright = sensors.getTempC(addr_tright);
//   tcenter = sensors.getTempC(addr_tcenter);
//   tctrl = sensors.getTempC(addr_tctrl);
// }

// void pump_on(int pin) 
// {
//   digitalWrite(pin, HIGH);
//   if (pin == PUMP_LEFT_PIN) pump_left = true;
//   else pump_right = true;
// }

// void pump_off(int pin) 
// {
//   digitalWrite(pin, LOW);
//   if (pin == PUMP_LEFT_PIN) pump_left = false;
//   else pump_right = false;
// }

// void mixer_on() 
// {
//   digitalWrite(MIXER_PIN, HIGH);
//   mixer = true;
// }

// void mixer_off() 
// {
//   digitalWrite(MIXER_PIN, LOW);
//   mixer = false;
// }

// void heater_on() 
// {
//   digitalWrite(HEATER_PIN, HIGH);
//   heater = true;
// }

// void heater_off() 
// {
//   digitalWrite(HEATER_PIN, LOW);
//   heater = false;
// }


// void loop() 
// {
//   read_temperature();
//   switch (state) 
//   {

//     case INITIAL:
//       if (DEBUG) Serial.println("INITIAL");
//       if (start == true) state = INIIAL_HEAT;
//       break;

//     case INIIAL_HEAT:
//       if (DEBUG) Serial.println("INITIAL_HEAT");
//         rregulator.input = tright;
//         lregulator.input = tleft;
//         regulate_left_heater(!lregulator.getResult());
//         regulate_right_heater(!rregulator.getResult());
//       if ((tleft >= tleft_target) and (tright >= tright_target)) 
//       {
//           regulate_right_heater(HIGH); //off
//           regulate_left_heater(HIGH);  //off
//           pump_on(PUMP_LEFT_PIN);
//           pump_on(PUMP_RIGHT_PIN);
//           timer_left.start();
//           timer_right.start();
//           state = FLOW;
//       }
//       break;

//     case FLOW:
//       if (DEBUG) Serial.println("FLOW");
//       if (timer_left.tick()) 
//       {
//         pump_off(PUMP_LEFT_PIN);
//       }
//       if (timer_right.tick()) 
//       {
//         pump_off(PUMP_RIGHT_PIN);
//       }
//       if (timer_right.elapsed() and timer_left.elapsed()) 
//       {
//         heater_on();
//         mixer_on();
//         state = MAIN_HEAT;
//       }
//       break;

//     case MAIN_HEAT:
//       if (DEBUG) Serial.println("MAIN_HEAT");
//       if (tcenter >= tcenter_target) 
//       {
//         mixer_off();
//         heater_off();
//         timer_wait.start();
//         state = WAIT;
//       }
//       break;

//     case WAIT:
//       if (DEBUG) Serial.println("WAIT");
//       if (timer_wait.tick()) {
//         start = false;
//         state = INITIAL;
//       }
//       break;
//   }
//   info();
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ModbusServerWiFi.h>
#include <TimerMs.h>

bool DEBUG = true;

float ML_PER_SEC = 32.5;

// PINS list
int THERMAL_SENSOR = 4;
int MIXER_PIN = 26;
int PUMP_LEFT_PIN = 32;
int PUMP_RIGHT_PIN = 33;
int HEATER_PIN = 27;

// Network variables list
bool start;
bool pump_left;
bool pump_right;
bool mixer;
bool heater;
float tleft;
float tright;
float tcenter;
float tctrl;

// Constants settings
float tleft_target = 35;
float tright_target = 35;
float tcenter_target = 40;
float time_left = ((5 * 1000) / ML_PER_SEC) * 1000;
float time_right = ((5 * 1000) / ML_PER_SEC) * 1000;
float time_wait = 60000;

// States
const uint8_t INITIAL = 0;
const uint8_t INIIAL_HEAT = 1;
const uint8_t FLOW = 2;
const uint8_t MAIN_HEAT = 3;
const uint8_t WAIT = 4;
uint8_t state = INITIAL;

// Timers
TimerMs timer_left(time_left, 0, 1);
TimerMs timer_right(time_right, 0, 1);
TimerMs timer_wait(time_wait, 0, 1);

// Sensors
OneWire ds(THERMAL_SENSOR);
DallasTemperature sensors(&ds);
byte addr_tleft[8] = {0x28, 0xff, 0xed, 0x7, 0xd6, 0x1, 0x3c, 0xe9};
byte addr_tright[8] = {0x28, 0x78, 0xbf, 0x7, 0xd6, 0x1, 0x3c, 0x90};
byte addr_tcenter[8] = {0x28, 0x3d, 0x30, 0x7, 0xd6, 0x1, 0x3c, 0xaa};
byte addr_tctrl[8] = {0x28, 0xd1, 0x82, 0x7, 0xd6, 0x1, 0x3c, 0x32};
byte addr[8];

// Wi-Fi
char ssid[] = "ichiro-home";
char pass[] = "suzumiaharuhi";

// Modbus server
ModbusServerWiFi MBserver;
IPAddress lIP;                     // assigned local IP

// Work address
IPAddress local_IP(10, 77, 70, 120);
IPAddress gateway(10, 77, 70, 1);
// Home address
// IPAddress local_IP(192, 168, 1, 120);
// IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

const uint8_t MY_SERVER(1);

// Reads temperatures from 1-wire bus
void read_temperature() 
{
  sensors.requestTemperatures();

  delay(200);

  tleft = sensors.getTempC(addr_tleft);
  tright = sensors.getTempC(addr_tright);
  tcenter = sensors.getTempC(addr_tcenter);
  tctrl = sensors.getTempC(addr_tctrl);
}

uint16_t convert_temp(float temp)
{
  uint16_t res = 0;
  
  if (temp < 0) {
    return res;
  } else {
    return (uint16_t)(temp * 100);
  }
}

// Worker function for serverID=1, function code 0x03 or 0x04
ModbusMessage FC03(ModbusMessage request) {
  uint16_t addr = 0;        // Start address to read
  uint16_t wrds = 0;        // Number of words to read
  ModbusMessage response;

  // Get addr and words from data array. Values are MSB-first, get() will convert to binary
  request.get(2, addr);
  request.get(4, wrds);

  // Prepare response
  response.add(
    request.getServerID(), 
    request.getFunctionCode(), 
    (uint8_t)(wrds * 2)
  );

  response.add(convert_temp(tleft));
  response.add(convert_temp(tright));
  response.add(convert_temp(tcenter));
  response.add(convert_temp(tctrl));

  // Return the data response
  return response;
}

// Debug function for 1-wire adresses exploring
void search_addrs() 
{
  while (true) 
  {
    if ( ds.search(addr)) 
    {
      Serial.println("ADDR:");
        for (int i=0; i < 8; i++) 
        {
          Serial.print(addr[i], HEX);
          Serial.print(" ");
        }
      Serial.println();
    }
    else 
    {
      Serial.println("No more addresses.");
      Serial.println();
      ds.reset_search();
      delay(250);
      return;
    }
  }
}

// Prints all status variables to Serial
void info() 
{
  Serial.print(tleft);
  Serial.print(" ");
  Serial.print(tright);
  Serial.print(" ");
  Serial.print(tcenter);
  Serial.print(" ");
  Serial.print(tctrl);
  Serial.print(" ");
  Serial.print(pump_left);
  Serial.print(" ");
  Serial.print(pump_right);
  Serial.print(" ");
  Serial.print(mixer);
  Serial.print(" ");
  Serial.println(heater);
}

void pump_on(int pin) 
{
  digitalWrite(pin, HIGH);
  if (pin == PUMP_LEFT_PIN) pump_left = true;
  else pump_right = true;
}

void pump_off(int pin) 
{
  digitalWrite(pin, LOW);
  if (pin == PUMP_LEFT_PIN) pump_left = false;
  else pump_right = false;
}

void mixer_on() 
{
  digitalWrite(MIXER_PIN, HIGH);
  mixer = true;
}

void mixer_off() 
{
  digitalWrite(MIXER_PIN, LOW);
  mixer = false;
}

void heater_on() 
{
  digitalWrite(HEATER_PIN, HIGH);
  heater = true;
}

void heater_off() 
{
  digitalWrite(HEATER_PIN, LOW);
  heater = false;
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  sensors.setResolution(12);
  timer_left.setTimerMode();
  timer_right.setTimerMode();
  timer_wait.setTimerMode();

  pinMode(PUMP_LEFT_PIN, OUTPUT);
  pinMode(PUMP_RIGHT_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(MIXER_PIN, OUTPUT);

  // Register the worker function with the Modbus server
  MBserver.registerWorker(MY_SERVER, READ_HOLD_REGISTER, &FC03);
  // Register the worker function again for another FC
  MBserver.registerWorker(MY_SERVER, READ_INPUT_REGISTER, &FC03);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  // Connect to WiFi 
  WiFi.begin(ssid, pass);
  delay(200);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  // print local IP address:
  lIP = WiFi.localIP();
  Serial.printf("My IP address: %u.%u.%u.%u\n", lIP[0], lIP[1], lIP[2], lIP[3]);

  // Start the Modbus TCP server:
  // Port number 502, maximum of 4 clients in parallel, 10 seconds timeout
  MBserver.start(502, 4, 10000);
  
  start = true;
}

void loop() 
{
  read_temperature();
  switch (state) 
  {

    case INITIAL:
      if (DEBUG) Serial.println("INITIAL");
      if (start == true) state = INIIAL_HEAT;
      break;

    case INIIAL_HEAT:
      if (DEBUG) Serial.println("INITIAL_HEAT");
      if ((tleft >= tleft_target) and (tright >= tright_target)) 
      {
          pump_on(PUMP_LEFT_PIN);
          pump_on(PUMP_RIGHT_PIN);
          timer_left.start();
          timer_right.start();
          state = FLOW;
      }
      break;

    case FLOW:
      if (DEBUG) Serial.println("FLOW");
      if (timer_left.tick()) 
      {
        pump_off(PUMP_LEFT_PIN);
      }
      if (timer_right.tick()) 
      {
        pump_off(PUMP_RIGHT_PIN);
      }
      if (timer_right.elapsed() and timer_left.elapsed()) 
      {
        heater_on();
        mixer_on();
        state = MAIN_HEAT;
      }
      break;

    case MAIN_HEAT:
      if (DEBUG) Serial.println("MAIN_HEAT");
      if (tcenter >= tcenter_target) 
      {
        mixer_off();
        heater_off();
        timer_wait.start();
        state = WAIT;
      }
      break;

    case WAIT:
      if (DEBUG) Serial.println("WAIT");
      if (timer_wait.tick()) {
        start = false;
        state = INITIAL;
      }
      break;
  }
  info();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <Arduino.h>
// #include <TimerMs.h>


// int PUMP_LEFT_PIN = 32;
// uint32_t time_left = 120000;

// TimerMs timer_left;

// void pump_on(int pin) 
// {
//   digitalWrite(pin, HIGH);
// }

// void pump_off(int pin) 
// {
//   digitalWrite(pin, LOW);
// }


// void setup() 
// {
//   timer_left.setTimerMode();
//   timer_left.setTime(time_left);
//   timer_left.start();
 
//   pinMode(PUMP_LEFT_PIN, OUTPUT);
//   pump_on(PUMP_LEFT_PIN);
// }


// void loop() 
// {
//   if (timer_left.tick()) {
//     pump_off(PUMP_LEFT_PIN);
//   }
// }