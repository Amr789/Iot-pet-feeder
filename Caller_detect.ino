#include <Arduino.h>
#include <arduinoFFT.h>


#define SAMPLES 128              // Reduced the number of samples
#define SAMPLING_FREQUENCY 3000  // Hz, must be less than 10000 due to ADC


#define FREQ_1_HIGH 1209  // Hz
#define FREQ_1_LOW 697    // Hz


#define FREQ_2_HIGH 1336  // Hz
#define FREQ_2_LOW 697    // Hz


#define FREQ_3_HIGH 1477  // Hz
#define FREQ_3_LOW 697    // Hz


#define FREQ_4_HIGH 1209  // Hz
#define FREQ_4_LOW 770    // Hz


#define FREQ_5_HIGH 1336  // Hz
#define FREQ_5_LOW 770    // Hz


#define FREQ_6_HIGH 1477  // Hz
#define FREQ_6_LOW 770    // Hz


#define FREQ_7_HIGH 1209  // Hz
#define FREQ_7_LOW 852    // Hz


#define FREQ_8_HIGH 1336  // Hz
#define FREQ_8_LOW 852    // Hz


#define FREQ_9_HIGH 1477  // Hz
#define FREQ_9_LOW 852    // Hz


#define FREQ_0_HIGH 1336  // Hz
#define FREQ_0_LOW 941    // Hz


#define FREQ_A_HIGH 1209  // Hz 
#define FREQ_A_LOW 941    // Hz


#define FREQ_B_HIGH 1477  // Hz
#define FREQ_B_LOW 941    // Hz


#define THRESHOLD 20  // Adjust this threshold as needed


#define segmentA 6
#define segmentB 5
#define segmentC 2
#define segmentD 3
#define segmentE 4
#define segmentF 7
#define segmentG 8


ArduinoFFT<float> fft = ArduinoFFT<float>();


unsigned int sampling_period_us;
unsigned long microseconds;


struct Peak {
  float frequency;
  float magnitude;
};


struct PeakPair {
  Peak dominant;
  Peak secondDominant;
};


PeakPair majorPeaks(float* magnitudes, unsigned int samples, float samplingFrequency) {
  Peak dominantPeak = { 0, 0 };
  Peak secondDominantPeak = { 0, 0 };


  for (int i = 1; i < samples / 2; i++) {
    float magnitude = magnitudes[i];
    float frequency = i * (samplingFrequency / samples);


    if (magnitude > dominantPeak.magnitude) {
      secondDominantPeak = dominantPeak;
      dominantPeak = { frequency, magnitude };
    } else if (magnitude > secondDominantPeak.magnitude) {
      secondDominantPeak = { frequency, magnitude };
    }
  }


  PeakPair result = { dominantPeak, secondDominantPeak };
  return result;
}


void setup() {
  Serial.begin(115200);


  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));


  fft.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);


  // Set the pin modes for the seven-segment display
  pinMode(segmentA, OUTPUT);
  pinMode(segmentB, OUTPUT);
  pinMode(segmentC, OUTPUT);
  pinMode(segmentD, OUTPUT);
  pinMode(segmentE, OUTPUT);
  pinMode(segmentF, OUTPUT);
  pinMode(segmentG, OUTPUT);
}


void loop() {
  float vReal[SAMPLES];
  float vImag[SAMPLES];


  microseconds = micros();


  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = analogRead(A1);
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us)
      ;  // Wait until sampling period ends
    microseconds += sampling_period_us;
  }


  fft.compute(vReal, vImag, SAMPLES, FFT_FORWARD);


  float magnitudes[SAMPLES];
  for (int i = 0; i < SAMPLES; i++) {
    magnitudes[i] = sqrt(vReal[i] * vReal[i] + vImag[i] * vImag[i]);
  }


  PeakPair peaks = majorPeaks(magnitudes, SAMPLES, SAMPLING_FREQUENCY);
 
  // Map dominant frequency to corresponding DTMF tone
  char number1 = mapToDTMF(peaks.dominant.frequency, peaks.secondDominant.frequency);


  Serial.print("Detected Frequencies - Dominant: ");
  Serial.print(peaks.dominant.frequency);
  Serial.print(" Hz, Second Dominant: ");
  Serial.print(peaks.secondDominant.frequency);
  Serial.println(" Hz");
 
  if (number1 != '\0') {
    Serial.print("Dominant tone: ");
    Serial.println(number1);
    displayDigit(number1);


    // Display the detected number on the seven-segment display
    displayDigit(number1 - '0');
  }
 
  delay(1000); // Delay to slow down serial output
}


char mapToDTMF(float frequency1, float frequency2) {
  if (frequency1 >= FREQ_1_LOW - THRESHOLD && frequency1 <= FREQ_1_LOW + THRESHOLD && frequency2 >= FREQ_1_HIGH - THRESHOLD && frequency2 <= FREQ_1_HIGH + THRESHOLD) return '1';
  if (frequency1 >= FREQ_2_LOW - THRESHOLD && frequency1 <= FREQ_2_LOW + THRESHOLD && frequency2 >= FREQ_2_HIGH - THRESHOLD && frequency2 <= FREQ_2_HIGH + THRESHOLD) return '2';
  if (frequency1 >= FREQ_3_LOW - THRESHOLD && frequency1 <= FREQ_3_LOW + THRESHOLD && frequency2 >= FREQ_3_HIGH - THRESHOLD && frequency2 <= FREQ_3_HIGH + THRESHOLD) return '3';
  if (frequency1 >= FREQ_4_LOW - THRESHOLD && frequency1 <= FREQ_4_LOW + THRESHOLD && frequency2 >= FREQ_4_HIGH - THRESHOLD && frequency2 <= FREQ_4_HIGH + THRESHOLD) return '4';
  if (frequency1 >= FREQ_5_LOW - THRESHOLD && frequency1 <= FREQ_5_LOW + THRESHOLD && frequency2 >= FREQ_5_HIGH - THRESHOLD && frequency2 <= FREQ_5_HIGH + THRESHOLD) return '5';
  if (frequency1 >= FREQ_6_LOW - THRESHOLD && frequency1 <= FREQ_6_LOW + THRESHOLD && frequency2 >= FREQ_6_HIGH - THRESHOLD && frequency2 <= FREQ_6_HIGH + THRESHOLD) return '6';
  if (frequency1 >= FREQ_7_LOW - THRESHOLD && frequency1 <= FREQ_7_LOW + THRESHOLD && frequency2 >= FREQ_7_HIGH - THRESHOLD && frequency2 <= FREQ_7_HIGH + THRESHOLD) return '7';
  if (frequency1 >= FREQ_8_LOW - THRESHOLD && frequency1 <= FREQ_8_LOW + THRESHOLD && frequency2 >= FREQ_8_HIGH - THRESHOLD && frequency2 <= FREQ_8_HIGH + THRESHOLD) return '8';
  if (frequency1 >= FREQ_9_LOW - THRESHOLD && frequency1 <= FREQ_9_LOW + THRESHOLD && frequency2 >= FREQ_9_HIGH - THRESHOLD && frequency2 <= FREQ_9_HIGH + THRESHOLD) return '9';
  if (frequency1 >= FREQ_0_LOW - THRESHOLD && frequency1 <= FREQ_0_LOW + THRESHOLD && frequency2 >= FREQ_0_HIGH - THRESHOLD && frequency2 <= FREQ_0_HIGH + THRESHOLD) return '0';
  if (frequency1 >= FREQ_A_LOW - THRESHOLD && frequency1 <= FREQ_A_LOW + THRESHOLD && frequency2 >= FREQ_A_HIGH - THRESHOLD && frequency2 <= FREQ_A_HIGH + THRESHOLD) return '*';
  if (frequency1 >= FREQ_B_LOW - THRESHOLD && frequency1 <= FREQ_B_LOW + THRESHOLD && frequency2 >= FREQ_B_HIGH - THRESHOLD && frequency2 <= FREQ_B_HIGH + THRESHOLD) return '#';


  // No match found
  return '\0';
}


void displayDigit(int digit) {
  // Define patterns for each digit
  switch (digit) {
    case 0:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, HIGH);
      digitalWrite(segmentE, HIGH);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, LOW);
      break;
    case 1:
      digitalWrite(segmentA, LOW);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, LOW);
      digitalWrite(segmentE, LOW);
      digitalWrite(segmentF, LOW);
      digitalWrite(segmentG, LOW);
      break;
    case 2:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, LOW);
      digitalWrite(segmentD, HIGH);
      digitalWrite(segmentE, HIGH);
      digitalWrite(segmentF, LOW);
      digitalWrite(segmentG, HIGH);
      break;
    case 3:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, HIGH);
      digitalWrite(segmentE, LOW);
      digitalWrite(segmentF, LOW);
      digitalWrite(segmentG, HIGH);
      break;
    case 4:
      digitalWrite(segmentA, LOW);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, LOW);
      digitalWrite(segmentE, LOW);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, HIGH);
      break;
    case 5:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, LOW);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, HIGH);
      digitalWrite(segmentE, LOW);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, HIGH);
      break;
    case 6:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, LOW);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, HIGH);
      digitalWrite(segmentE, HIGH);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, HIGH);
      break;
    case 7:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, LOW);
      digitalWrite(segmentE, LOW);
      digitalWrite(segmentF, LOW);
      digitalWrite(segmentG, LOW);
      break;
    case 8:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, HIGH);
      digitalWrite(segmentE, HIGH);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, HIGH);
      break;
    case 9:
      digitalWrite(segmentA, HIGH);
      digitalWrite(segmentB, HIGH);
      digitalWrite(segmentC, HIGH);
      digitalWrite(segmentD, LOW);
      digitalWrite(segmentE, LOW);
      digitalWrite(segmentF, HIGH);
      digitalWrite(segmentG, HIGH);
      break;
  }
}