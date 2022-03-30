//Period Pants Version 1.0 
//Technology integrated by Jyo(https://jyozspace.in/) on the behalf of Dr. Subhankar Mishra's Lab (School of Computer Science, NISER)
//Technology developed for Amish and Shreya, Grad Students, NIFT Bhubaneswar.

#include<SoftwareSerial.h>

 int coilA = 7;
 int coilB = 6;
 int coilC = 8;
 float sensA;
 float sensB;
 float sensC;
 float sensavg;
 float stddav;
 bool state = 0;
 
//Values for temperature calculation: 
#define RT0 1000   // Ω : resistance of thermistor at t=25
#define B 5000      // K : constant for calculation
#define VCC 5    //Supply voltage
#define R 220  //R=10KΩ

//Variables
float RT, VR, ln, TX, T0, VRT;
float Tempadd;
int rec = 0;
int set;
int i=1;


//initiating a software serial communication for connecting with bluetooth module (hc-05)
SoftwareSerial bluetooth(2,3); //Rx,Tx

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

// turning all coils off.
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
//temperature control system will be initiated as "Deactivated"
  state = 0;
 }

void loop() {

//Serial.print("------------------------------Loop Iteration: ");
//Serial.print(i);
//Serial.println(" ----------------------------------");

//Recieving Bluetooth Data:
if(bluetooth.available() > 0){
  rec = bluetooth.read();
  //Serial.print("Bluetooth Input is : ");
  //Serial.println(rec);
  if (rec == NAN){
   // Serial.println("Value received is not a number - resetting the temperature setting to 0");
    rec = 0;
  }
  if (rec == 111){
   //Serial.print("Activating Temperature Control. Device state set to ");
   state = 1;
   //Serial.println(state);
   bluetooth.print(111.0);
   delay (500);
  }
 if(rec == 101){
   //Serial.print("Dectivating Temperature Control. Device state set to ");
   state = 0;
   //Serial.println(state);
   bluetooth.print(101.0);
   delay (500);
 }
 if (rec < 100){
  set = rec;
 }
}
//-----------------------------

//displaying the operating decision based on app input:
//Serial.print("The set Temp is: ");
//Serial.println(set);
//Serial.print("Control system is set to be: ");
//Serial.println(state);
//------------------------------------------- 

//fetching temperature data from the pad sensors:
sensA= tempread(A0);
sensB= tempread(A1);
sensC= tempread(A2);
//--------------------------------------------

//average temperature (that'll be sent to app)
sensavg = (sensA+sensB+sensC)/3;
//-----------------------------------

//finding the standard daviation of sensor values to detect anomaly in pad status.
stddav = sqrt((pow((sensA-sensavg),2)+ pow((sensB-sensavg),2)+pow((sensC-sensavg),2))/3);
//----------------------------------
  


//displaying sensor values and interpretations:
//Serial.print("Current Sensor A Value is: ");
//Serial.println(sensA);
//Serial.print("Current Sensor B Value is: ");
//Serial.println(sensB);
//Serial.print("Current Sensor C Value is: ");
//Serial.println(sensC);
//Serial.print("Average sensor value = ");
//Serial.println(sensavg);
//Serial.print("Standard daviation of sensor values = ");
//Serial.println(stddav);
//--------------------------------

//error check

//1. MAX temperature setting should not exceed 55
if (set >55.0){
  state = 0;
  //Serial.println("Error: Set temperature too high.");
}

//2. Standard daviation of sensor values should not exceed 2.5
if (stddav >5){
  state = 0;
  //Serial.println("Error: Sensor Values not in range.");
  bluetooth.print(150.0);
  delay(500);
}

//3. Neither of the sensors should report a value heigher then 58 degree celcius
if (sensA > 58.0 || sensB > 58.0 || sensC > 58.0){
  state = 0;
  //Serial.println("Error: One or more sensors are reporting too high temperature.");
  bluetooth.print(151.0);
  delay(500);
}
//4. If bluetooth connection with client breaks, disable the heating pad: This feature is to be added in future iterations.
//------------------------------------------

//If the Temperature control is set to be ON through the app. The program should proceed to control the individual heating coils:
if (state == 1){
  //Serial.println("Temperature control is ON");
  //Controlling Coil A
  if (sensA < (set-1.0)){
   // Serial.println("Turning Heating ON for coil A");
    digitalWrite(coilA, HIGH);  
  }
  if (sensA > (set)){
    //Serial.println("Turning Heating OFF for coil A");
    digitalWrite(coilA, LOW);
  }

  //Controlling Coil B
  if (sensB < (set-1.0)){
   // Serial.println("Turning Heating ON for coil B");
    digitalWrite(coilB, HIGH);  
  }
  if (sensB > (set)){
    //Serial.println("Turning Heating OFF for coil B");
    digitalWrite(coilB, LOW);
  }

  //Controlling Coil C
  if (sensC < (set-1.0)){
    //Serial.println("Turning Heating ON for coil C");
    digitalWrite(coilB, HIGH);  
  }
  if (sensC > (set)){
    //Serial.println("Turning Heating OFF for coil C");
    digitalWrite(coilB, LOW);
  }
}

// if state is not set to be on:
else {
   //Serial.println("Temperature control is OFF"); 
   digitalWrite(coilA, LOW);
   digitalWrite(coilB, LOW);
   digitalWrite(coilC, LOW);
}

// Preperating for sending avg temp every 2.5 second to app.
i ++;
Tempadd = Tempadd + sensavg;
if(i>5){
  Tempadd = Tempadd/5;
  i = 1;
  //Serial.print("Sending temp value to the the app: ");
  bluetooth.print(Tempadd);
  delay(500);
  //Serial.println(Tempadd);
  Tempadd = 0;    
 // Serial.println("=================================Loop Reset=======================================");
}


//--------------------------
delay (500);
}


// function to read the temperature of each sensor. 
float tempread(int pin){
//Reading Temperature Data
  VRT = analogRead(pin);              //Acquisition analog value of VRT
  VRT = (5.00 / 1023.00) * VRT;      //Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R);               //Resistance of RT
  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor
  TX = TX - 273.15;                 //Conversion to Celsius
  return TX;
}
