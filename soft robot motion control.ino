// Authors: Sichuan Huang, Junliang Tao
///////////////////INPUT FOR THE OPENNER FUNCTION//////////////////////////////
long ON1 = 0, ON2 = 0, ON3 = 150, ON4 = 150;
long PM1 = 0, PM2 = 0, PM3 = 150, PM4 = 150;
long OFF1 = 0, OFF2 = 0, OFF3 = 150, OFF4 = 150;

long Delay1 = 0;                      // Delay time for Ch1 in millis
long Delay2 = 0;                      // Delay time for Ch2 in millis
long Delay3 = 0;                      // Delay time for Ch3 in millis
long Delay4 = OFF4;                      // Delay time for Ch4 in millis

long INTERVAL = 1000/240;             // Sampling interval in micros; sampling frequency = 240kHZ
unsigned long lastMicros = 0;         // will store last time for serial print

/////////////////////////DERIVED PARAMETERS////////////////////////////////////
long DELTA1 = OFF1-Delay1;            // Initial off time for Ch1 in millis
long DELTA2 = OFF2-Delay2;            // Initial off time for Ch2 in millis
long DELTA3 = OFF3-Delay3;            // Initial off time for Ch3 in millis
long DELTA4 = OFF4-Delay4;            // Initial off time for Ch4 in millis

////////////////////////CHANNEL PIN ASSIGNMENT/////////////////////////////////
int ChPin1 = 6, ChPin2 = 7, ChPin3 = 8, ChPin4 = 9;
int MdPin1 = 2, MdPin2 = 3, MdPin3 = 4, MdPin4 = 5;
int PsPin1 = A8, PsPin2 = A9, PsPin3 = A10, PsPin4 = A11;

/////////////////////////// DEFINE AN OPENNER CLASS /////////////////////////////
class Openner{
  public:
		int ChPin;                        // the number of the Channel pin
		int MdPin;                        // the number of the Middle pin
		int PsPin;                        // the analog pin number for pressure reading of the channel
		long OnTime;                      // milliseconds of on-time
		long PmTime;                      // milliseconds of pressure-maintain time
		long OffTime;                     // milliseconds of off-time
		long DeltaTime;                   // milliseconds of initial off time
    int mode;                         // control mode of channel Pin
    unsigned long previousMillis;     // will store last time Ch was updated
    unsigned long currentMillis;      // get the real time of test
    
    Openner(int aChPin, int aMdPin, int aPsPin, long aOnTime, long aPmTime, long aOffTime, long aDeltaTime){
    		ChPin = aChPin;
    		MdPin = aMdPin;
    		PsPin = aPsPin;
    		OnTime = aOnTime;
    		PmTime = aPmTime;
    		OffTime = aOffTime;
    		DeltaTime = aDeltaTime;
    }

    void Initiate(){
    		pinMode(ChPin, OUTPUT);
    		pinMode(MdPin, OUTPUT); 
        // set up the default or initial state of the valves
        digitalWrite(MdPin, HIGH);  // 2-way valve is open
    		digitalWrite(ChPin, LOW);   // 3-way valve is close
    		
    		previousMillis = 0;
    		mode = 3;                   // deflation state
    }
    
    int Get_Mode(){
    		currentMillis = millis() + DeltaTime;
    		if ((mode == 1)&&(currentMillis - previousMillis >= OnTime)&&(OnTime > 0)){
      			previousMillis = currentMillis;
      			mode = 2;
    		}
    		if ((mode == 2)&&(currentMillis - previousMillis >= PmTime)&&(PmTime > 0)){
      			previousMillis = currentMillis;
      			mode = 3;
    		}
    		if ((mode == 3)&&(currentMillis - previousMillis >= OffTime)&&(OffTime > 0)){
      			previousMillis = currentMillis;
      			mode = 1;
    		}
       if ((OnTime == 0)&&(PmTime == 0)&&(OffTime == 0)){
            mode = 3;
       }
    		return mode;
    }
    
    void Motion_Update(int mode){
    		switch(mode){
    			case 1:   // for inflation
              digitalWrite(MdPin, HIGH);  // 2-way valve is open
      				digitalWrite(ChPin, HIGH);  // 3-way valve is open
      				break;
    			case 2:   // for inner pressure maintaining
              digitalWrite(MdPin, LOW);   // 2-way valve is closed
      				digitalWrite(ChPin, LOW);   // 3-way valve is open
      				break;
    			case 3:   // for deflation
              digitalWrite(MdPin, HIGH);  // 2-way valve is open
      				digitalWrite(ChPin, LOW);   // 3-way valve is closed
      				break;
    			default:  // no air goes into the robot
              digitalWrite(MdPin, HIGH);  // 2-way valve is open
      				digitalWrite(ChPin, LOW);   // 3-way valve is closed
      				break;
          }
    }

    float Get_Pressure(){
    		float pressure = (analogRead(PsPin)/1024.0 - 0.1)*100.0/0.8;
    		return pressure;
    }
};

Openner Ch1(ChPin1, MdPin1, PsPin1, ON1, PM1, OFF1, DELTA1);
Openner Ch2(ChPin2, MdPin2, PsPin2, ON2, PM2, OFF2, DELTA2);
Openner Ch3(ChPin3, MdPin3, PsPin3, ON3, PM3, OFF3, DELTA3);
Openner Ch4(ChPin4, MdPin4, PsPin4, ON4, PM4, OFF4, DELTA4);

void setup(){
	Serial.begin(9600);
//  Ch1.Initiate();
//  Ch2.Initiate();
	Ch3.Initiate();
  Ch4.Initiate();
}

void loop(){
//  Ch1.Motion_Update(Ch1.Get_Mode());
//  Ch2.Motion_Update(Ch2.Get_Mode());
	Ch3.Motion_Update(Ch3.Get_Mode());
  Ch4.Motion_Update(Ch4.Get_Mode());
  
  if (micros()-lastMicros > INTERVAL){
		lastMicros = micros();
//    Serial.print(Ch1.Get_Pressure());
//    Serial.print(",");
//    Serial.print(Ch2.Get_Pressure());
//    Serial.print(",");
		Serial.print(Ch3.Get_Pressure());
    Serial.print(",");
    Serial.println(Ch4.Get_Pressure());
	}
}
