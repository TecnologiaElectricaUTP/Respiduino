/*
Sketch:Program for computing The respiration rate of Climateric fruits, this sketch reads the cozir wide range 
by Hector F. Jimenez Saldarriaga.
Feedback: hfjimenez@utp.edu.co at Technological University of Pereira 
Correlationship between C02,Temperature,Humidity instantaneous or real time.
Keywords:  respiration rate, modified atmosphere packaging,Predective
Ex: This code was taking in count an application note saw in the arduino forum and cgs for more info
http://forum.arduino.cc/index.php?PHPSESSID=8tjhrjjqm5g2cidpgllti25uv6&topic=91467.30.
*/

#include <SoftwareSerial.h>  //Serial Modifications
#include <SPI.h>             // Spi Communication
#include <DACMCP4821.h>      //DAC 4821 Library.
#include <cozir.h>           //adapte from the sensor ambient cozir, wich has differents features.

//     ******** MCP4821 Pinout ********
//     ********************************
//     SDI Pin to MOSI Pin 11 Arduino UNO
//     SCK Pin to SCK Pin 13 Arduino UNO
/* Configure CS,SHUTDOWN and LDAC Pins */

DACMCP4821 dac(2,7,4);
int outputAnalog;
byte ganancia;

SoftwareSerial nss(2,3);     //This Pins must be for Uno , other arduino version works with (11,12);//TX,RX and (50,51)//TX,RX it works like a charm but take care.

COZIR czr(nss);
int sensorPin1 = 7;
//Free I0 Serial : 2(tx,rx),6Pwm ALL,Analog 6 de A0 a A5,Digital : 8,13,4. Total Free Pin  :17
//Power Supply: reset,vin,5v.
//Warning  if you're going to use the jack, The board can operate on an external supply of 6 to 20 volts
long result = 0;
unsigned long time1=0;
unsigned long time2=0;

long readingsPer=20.0;
long readDelay=200.0;

#define RH1(time) ((.1667*time)-67)


//Vars For the Relative Humidity.
//int sensorPin = 7;              // 220 or 1k resistor connected to this pin
//float capacitance = 0;      Second Function for the humidity thing.
//float avgCapacitance = 0;
//long unsigned startTime;
//long unsigned elapsedTime;
// resistance in MegOhms
//float resistance=3.0;
//float fudgeFactor=1.1;
//float RHslope=0.34;  //Cambio de Capacitancia por cada centigrado 
//float RHconstant=155.0;
//int humidity;

/*Calibration and usage of cozir.h library.
Here is a step by step
1.uncomment czr.SetOperatingMode(CZR_POLLING);
Upload Sketch
2.comment czr.SetOperatingMode(CZR_POLLING); and uncomment czr.CalibrateFreshAir();
Upload Sketch
3.comment czr.CalibrateFreshAir();
Upload sketch
and if you want to change digital filter do the same, 
as the settings are kept on the sensor you never need to do these again unless you want to. I have found if i uncomment them all the sensor plays up and gives bad values until 
i comment out and upload the sketch again, i dont know why this is, it would have been such a better sensor if it were polling by default.*/

/*---------------------
Humidity Sensors:

*/
void setup()
{
  dac.initDAC();      //Initialization of the DAC.
  Serial.begin(9600); 
  delay(5000);  //He begin after 5 seconds.
  //czr.SetOperatingMode(CZR_POLLING);
  //czr.SetOperatingMode(CZR_STREAMING);
  //czr.CalibrateFreshAir(); //This will be set by default.
  //czr.SetDigiFilter(64);
}

void loop()
{
  delay(4000);
  float t = czr.Celsius();  //This is for Ambient Cozir.
  float f = czr.Fahrenheit();  //This is for ambient cozir.  
  float h = czr.Humidity();
  int c = czr.CO2();
  int digi = czr.GetDigiFilter();
  
  Serial.print("CO2 ppm : ");Serial.println(c);
//  Serial.print("Digital Filter : ");Serial.println(digi);  que tipo de filtro usar ?
  Serial.print("\n");
  long H=RH1(evalHumid(readingsPer, sensorPin1));
  Serial.print("Humidity : ");Serial.println(H); 
  delay(1000);
  
  
    byte msb = dac.writeDAC(outputAnalog,2);
  }
  
//Relative Humidity
/*  RCtime
 *   Duplicates the functionality of the Basic Stamp's RCtime
 *   Allows digital pins to be used to read resistive analog sensors
 *   One advantage of this technique is that is can be used to read very wide ranging inputs.
 *   (The equivalent of 16 or 18 bit A/D) */
 
/*  int RCtime4RH(int sensPin)
{  
   int avg = 12; 
   avgCapacitance=0;
   for(byte k=0;k<avg;k++)
   {
   long result = 0;
   pinMode(sensPin, OUTPUT);       // make pin OUTPUT
   digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
   delay(1);                       // wait a  ms to make sure cap is discharged

   pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
   digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
   startTime=micros();
   while(digitalRead(sensPin)){    // wait for pin to go low
   }
   elapsedTime=micros()-startTime;
   capacitance=(fudgeFactor*(elapsedTime/resistance));
   avgCapacitance=avgCapacitance+capacitance;
   delay(10);
   }
   avgCapacitance=avgCapacitance/(float)avg;
   humidity=(int)((avgCapacitance-RHconstant)/RHslope);
   return humidity;    
} */  


long evalHumid(long samples, int sPin){
  long avgtime=0; 
  for (int i=0;i<samples;i++){
    RCtime(sPin);
    avgtime+= decayTime(3);
  }
  avgtime=avgtime/samples; 
  return(avgtime);

}

long RCtime(int sensPin){
  long result = 0;
  pinMode(sensPin, OUTPUT);       // make pin OUTPUT
  digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
  delay(1);                       // wait a  ms to make sure cap is discharged

  pinMode(sensPin, INPUT);  // turn pin into an input and time till pin goes low
  digitalWrite(sensPin, LOW);// turn pullups off - or it won't work
  decayTime(1);  
  while(digitalRead(sensPin)){    // wait for pin to go low
    //result++;
  }
  decayTime(2);
  delay(readDelay); 
  //return result;                   // report results   
}

long decayTime(int input){
  if (input==1){
    time1=micros();
  }
  if (input==2 ){
    time2=micros(); 
  }
  if (input==3){
    return (time2-time1);
  }
}
