#include <MIDI.h> // Include MIDI library

MIDI_CREATE_DEFAULT_INSTANCE(); // Create MIDI instance

const int transposePin = A0; // Analog pin for transposition value
const int quantizePin = A1; // Analog pin for quantization strength value
const int minNote = 48; // Minimum note value (C2)
const int maxNote = 65; // Maximum note value (F3)

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI); // Start MIDI communication
  pinMode(transposePin, INPUT); // Set transpose pin as input
  pinMode(quantizePin, INPUT); // Set quantize pin as input
}

void loop() {
  int transposeValue = analogRead(transposePin); // Read transposition value from analog pin
  int quantizeValue = analogRead(quantizePin); // Read quantization strength value from analog pin
  
  transposeValue = map(transposeValue, 0, 1023, -12, 12); // Map transposition value to semitones range (-12 to +12)
  
  quantizeValue = map(quantizeValue, 0, 1023, -6, +6); // Map quantization strength value to semitones range (-6 to +6)
  
  if (MIDI.read()) { // If a MIDI message is received
    byte type = MIDI.getType(); // Get message type
    byte channel = MIDI.getChannel(); // Get message channel
    byte data1 = MIDI.getData1(); // Get first data byte (note or controller number)
    byte data2 = MIDI.getData2(); // Get second data byte (velocity or controller value)

    if (type == midi::NoteOn || type == midi::NoteOff) { // If message is note on or note off
      
      data1 += transposeValue; // Add transposition value to note number
      
      if (quantizeValue > 0) { 
        data1 += quantizeValue - (data1 % quantizeValue); 
        /* If quantization strength is positive,
           round up note number to nearest multiple of quantization strength */
      }
      
      if (quantizeValue < 0) {
        data1 -= abs(quantizeValue) - (data1 % abs(quantizeValue)); 
        /* If quantization strength is negative,
           round down note number to nearest multiple of absolute value of quantization strength */
      }
      
      data1 = constrain(data1, minNote, maxNote); 
      /* Constrain note number to specified range */
      
      MIDI.send(type, data1, data2, channel);
      /* Send modified message */
    }
    
    else {
      MIDI.send(type, data1, data2, channel);
      /* Send unmodified message */
    }
    
  }
}
