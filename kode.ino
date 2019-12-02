// Eksempel kode for oppsett av timer-interrupt for en konstant samplingsfrekvens
#include <TimerOne.h>

//Globale variaber
volatile int sample;      // Holder siste sample
volatile float prevSample;
bool newSample;           //Støttevaribel for å sjekke om ny sample er tatt
double lowpassnow;
double lowpasslast;
double highpassnow;
double highpasslast;
double sumLow;
double sumHigh;
double averigeLow;
double averigeHigh;

int count;
int countmax;
float LP_a =  0.54;
float HP_a = -0.54;


int b_t;
int u_t;

void takeSample(void){
  sample = analogRead(0); // Sampler på A0
  newSample = true;
}


void lowpass(){
  lowpassnow = (sample-prevSample) + LP_a*lowpasslast;
  lowpasslast = lowpassnow;  
}

void highpass(){
  highpassnow = (sample-prevSample) + HP_a*highpasslast;
  highpasslast = highpassnow;  
}


void setup() {
  // Oppsett av timer interrupt
  Timer1.initialize(500); // 500 mikrosekund mellom hver sample -> gir F_s = 2kHz
  // Argumentet i "attachInterrupt" bestemmer hvilken funskjon som er interrupt handler 
  Timer1.attachInterrupt(takeSample); 
  
  Serial.begin(9600);

  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(0, INPUT);


  lowpassnow=0;
  lowpasslast=0;
  highpassnow=0;
  highpasslast=0;
  sumLow = 0;
  sumHigh = 0;
  averigeHigh = 0;
  averigeLow = 0;
  count = 0;
  countmax = 20;
}


void loop() {
  
  if(newSample){
    lowpass();
    highpass();

    sumLow += abs(lowpassnow);
    sumHigh += abs(highpassnow);

    count++;

    //etter 20 samples
    if (count == countmax) {
      //regner ut gjennomsnittet
      averigeLow = sumLow / countmax ;
      averigeHigh = sumHigh / countmax;
      
      if (averigeHigh < 20 && averigeLow < 20) { //sjekker om u(t) skal være høy eller lav
        digitalWrite(7, LOW);
      } else {
        digitalWrite(7, HIGH); //sjekker om b(t) skal være høy  eller lav
        if (averigeHigh > averigeLow) {
          digitalWrite(8, HIGH);

        } else if (averigeLow > averigeHigh) {
          digitalWrite(8, LOW);

        }

      }
      
      //nullstiller verdienene
      sumLow = 0;
      sumHigh = 0;
      
      averigeLow = 0;
      averigeHigh = 0;
      
      lowpassnow = 0;
      highpassnow = 0;
      
      count = 0;
    }
    prevSample = sample;
    newSample = false;
  }    
}
