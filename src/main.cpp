#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimerMs.h>


bool DEBUG = true;


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
float tleft_target = 29;
float tright_target = 29;
float tcenter_target = 32;
float time_left = 5000;
float time_right = 10000;
float time_wait = 5000;

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
OneWire  ds(THERMAL_SENSOR);
DallasTemperature sensors(&ds);
byte addr_tleft[8] = {0x28, 0xff, 0xed, 0x7, 0xd6, 0x1, 0x3c, 0xe9};
byte addr_tright[8] = {0x28, 0x78, 0xbf, 0x7, 0xd6, 0x1, 0x3c, 0x90};
byte addr_tcenter[8] = {0x28, 0x3d, 0x30, 0x7, 0xd6, 0x1, 0x3c, 0xaa};
byte addr_tctrl[8] = {0x28, 0xd1, 0x82, 0x7, 0xd6, 0x1, 0x3c, 0x32};
byte addr[8];


void setup() 
{
  Serial.begin(9600);
  sensors.setResolution(12);
  timer_left.setTimerMode();
  timer_right.setTimerMode();
  timer_wait.setTimerMode();
  pinMode(PUMP_LEFT_PIN, OUTPUT);
  pinMode(PUMP_RIGHT_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(MIXER_PIN, OUTPUT);
  start = true;
}


// Debug function for 1-wire adresses exploring
void search_addrs() 
{
  while (true) {
    if ( ds.search(addr)) {
      Serial.println("ADDR:");
        for (int i=0; i < 8; i++) {
          Serial.print(addr[i], HEX);
          Serial.print(" ");
        }
      Serial.println();
    }
    else {
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

// Reads temperatures from 1-wire bus
void read_temperature() 
{
  sensors.requestTemperatures();
  tleft = sensors.getTempC(addr_tleft);
  tright = sensors.getTempC(addr_tright);
  tcenter = sensors.getTempC(addr_tcenter);
  tctrl = sensors.getTempC(addr_tctrl);
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