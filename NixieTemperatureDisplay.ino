#include <OneWire.h>
#include <math.h>

//BCD Code snipped from here -  
//http://scuola.arduino.cc/lesson/B7xOJzE/How_to_drive_7_segment_display_with_HCF4511
//
//OneWire Resources and code from here -
// http://www.pjrc.com/teensy/td_libs_OneWire.html
// http://milesburton.com/Dallas_Temperature_Control_Library
//
//In addition to the Aruino Pro Micro (or your Arduino variety of choice) you'll need 3 K155ID1 Nixie driver ICs, 3 Nixie tubes, 1 Neon indicator, 
//a High Voltage power supply and resistors to match your tubes.
//


//Setup OneWire

OneWire  ds(19);  // on pin 19 (a 4.7K resistor is necessary)

int digOne = 0, digTwo = 0, digThree = 0; //digOne - Tens, digTwo - Ones, digThree - Decimal

//Declaration of Arduino pins for layout with Arduino Pro Micro in order to eliminate twisting or crossing of wires between Arduino board and K155ID1 Nixie drivers

//Tens
const int tenA=2;
const int tenD=3;
const int tenB=4;
const int tenC=5;

//Ones
const int oneA=15;
const int oneD=14;
const int oneB=16;
const int oneC=10;

//Decimal
const int decA=6;
const int decD=7;
const int decB=8;
const int decC=9;

//int count = 0;

void setup(void) {
  Serial.begin(9600);

  pinMode(tenA, OUTPUT); //LSB
  pinMode(tenB, OUTPUT);
  pinMode(tenC, OUTPUT);
  pinMode(tenD, OUTPUT); //MSB
  
  pinMode(oneA, OUTPUT); //LSB
  pinMode(oneB, OUTPUT);
  pinMode(oneC, OUTPUT);
  pinMode(oneD, OUTPUT); //MSB
  
  pinMode(decA, OUTPUT); //LSB
  pinMode(decB, OUTPUT);
  pinMode(decC, OUTPUT);
  pinMode(decD, OUTPUT); //MSB

}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
 //     Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
//      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
 //     Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
  //    Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }


  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
//  fahrenheit = celsius * 1.8 + 32.0;
/*  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print("     ");
    Serial.print(celsius,1);
  Serial.print("     ");
 int c1 = (celsius +.05) * 10;
    Serial.print(c1);
  Serial.print(" Celsius");
*/

	double temperature = (celsius + 0.05);

	
	//digOne - Tens
	temperature/=10;
	digOne = temperature;
        to_tenBCD();
	

	//digTwo - Ones
	temperature = (temperature-digOne)*10;
	digTwo = temperature;
        to_oneBCD();
	
	//digThree - Decimal
	temperature = (temperature-digTwo)*10;
	digThree = temperature;
        to_decBCD();        

}

void to_decBCD() //Evaluate and display decimal
{
    if (digThree == 0) //write 0000
    {
      digitalWrite(decA, LOW);
      digitalWrite(decB, LOW);
      digitalWrite(decC, LOW);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 1) //write 0001
    {
      digitalWrite(decA, HIGH);
      digitalWrite(decB, LOW);
      digitalWrite(decC, LOW);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 2) //write 0010
    {
      digitalWrite(decA, LOW);
      digitalWrite(decB, HIGH);
      digitalWrite(decC, LOW);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 3) //write 0011
    {
      digitalWrite(decA, HIGH);
      digitalWrite(decB, HIGH);
      digitalWrite(decC, LOW);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 4) //write 0100
    {
      digitalWrite(decA, LOW);
      digitalWrite(decB, LOW);
      digitalWrite(decC, HIGH);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 5) //write 0101
    {
      digitalWrite(decA, HIGH);
      digitalWrite(decB, LOW);
      digitalWrite(decC, HIGH);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 6) //write 0110
    {
      digitalWrite(decA, LOW);
      digitalWrite(decB, HIGH);
      digitalWrite(decC, HIGH);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 7) //write 0111
    {
      digitalWrite(decA, HIGH);
      digitalWrite(decB, HIGH);
      digitalWrite(decC, HIGH);
      digitalWrite(decD, LOW);
    }
     
    if (digThree == 8) //write 1000
    {
      digitalWrite(decA, LOW);
      digitalWrite(decB, LOW);
      digitalWrite(decC, LOW);
      digitalWrite(decD, HIGH);
    }
     
    if (digThree == 9) //write 1001
    {
      digitalWrite(decA, HIGH);
      digitalWrite(decB, LOW);
      digitalWrite(decC, LOW);
      digitalWrite(decD, HIGH);
    } 
}

void to_oneBCD()  //Evaluate and display Ones
{
    if (digTwo == 0) //write 0000
    {
      digitalWrite(oneA, LOW);
      digitalWrite(oneB, LOW);
      digitalWrite(oneC, LOW);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 1) //write 0001
    {
      digitalWrite(oneA, HIGH);
      digitalWrite(oneB, LOW);
      digitalWrite(oneC, LOW);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 2) //write 0010
    {
      digitalWrite(oneA, LOW);
      digitalWrite(oneB, HIGH);
      digitalWrite(oneC, LOW);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 3) //write 0011
    {
      digitalWrite(oneA, HIGH);
      digitalWrite(oneB, HIGH);
      digitalWrite(oneC, LOW);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 4) //write 0100
    {
      digitalWrite(oneA, LOW);
      digitalWrite(oneB, LOW);
      digitalWrite(oneC, HIGH);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 5) //write 0101
    {
      digitalWrite(oneA, HIGH);
      digitalWrite(oneB, LOW);
      digitalWrite(oneC, HIGH);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 6) //write 0110
    {
      digitalWrite(oneA, LOW);
      digitalWrite(oneB, HIGH);
      digitalWrite(oneC, HIGH);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 7) //write 0111
    {
      digitalWrite(oneA, HIGH);
      digitalWrite(oneB, HIGH);
      digitalWrite(oneC, HIGH);
      digitalWrite(oneD, LOW);
    }
     
    if (digTwo == 8) //write 1000
    {
      digitalWrite(oneA, LOW);
      digitalWrite(oneB, LOW);
      digitalWrite(oneC, LOW);
      digitalWrite(oneD, HIGH);
    }
     
    if (digTwo == 9) //write 1001
    {
      digitalWrite(oneA, HIGH);
      digitalWrite(oneB, LOW);
      digitalWrite(oneC, LOW);
      digitalWrite(oneD, HIGH);
    } 
}



void to_tenBCD()  //Evaluate and display Tens
{
    if (digOne == 0) //write 0000
    {
      digitalWrite(tenA, LOW);
      digitalWrite(tenB, LOW);
      digitalWrite(tenC, LOW);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 1) //write 0001
    {
      digitalWrite(tenA, HIGH);
      digitalWrite(tenB, LOW);
      digitalWrite(tenC, LOW);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 2) //write 0010
    {
      digitalWrite(tenA, LOW);
      digitalWrite(tenB, HIGH);
      digitalWrite(tenC, LOW);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 3) //write 0011
    {
      digitalWrite(tenA, HIGH);
      digitalWrite(tenB, HIGH);
      digitalWrite(tenC, LOW);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 4) //write 0100
    {
      digitalWrite(tenA, LOW);
      digitalWrite(tenB, LOW);
      digitalWrite(tenC, HIGH);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 5) //write 0101
    {
      digitalWrite(tenA, HIGH);
      digitalWrite(tenB, LOW);
      digitalWrite(tenC, HIGH);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 6) //write 0110
    {
      digitalWrite(tenA, LOW);
      digitalWrite(tenB, HIGH);
      digitalWrite(tenC, HIGH);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 7) //write 0111
    {
      digitalWrite(tenA, HIGH);
      digitalWrite(tenB, HIGH);
      digitalWrite(tenC, HIGH);
      digitalWrite(tenD, LOW);
    }
     
    if (digOne == 8) //write 1000
    {
      digitalWrite(tenA, LOW);
      digitalWrite(tenB, LOW);
      digitalWrite(tenC, LOW);
      digitalWrite(tenD, HIGH);
    }
     
    if (digOne == 9) //write 1001
    {
      digitalWrite(tenA, HIGH);
      digitalWrite(tenB, LOW);
      digitalWrite(tenC, LOW);
      digitalWrite(tenD, HIGH);
    } 
}
