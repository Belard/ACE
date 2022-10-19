#include <Arduino.h> 
#define LED1_pin 6
#define LED2_pin 7
#define LED3_pin 8
#define LED4_pin 9
#define LED5_pin 10
#define LED6_pin 11
#define LED7_pin 12

#define S1_pin 2
#define S2_pin 3

typedef struct {
  int state, new_state, pause;
;

  // tes - time entering state
  // tis - time in state
  // tis_pause - time in state (save while in pause)
  unsigned long tes, tis, tis_pause;
} fsm_t;

// Input variables
uint8_t S1, prevS1;
uint8_t S2, prevS2;

uint8_t S1_real, S2_real;

uint8_t S1_gate = 0;
uint8_t S2_gate = 0;

unsigned long tis_pause = 0;

int end_cycle = 0;

// Output variables
uint8_t LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7;

// Our finite state machines
fsm_t fsm1, fsm2, fsm3, fsm4, fsm5, fsm6, fsm7, sw1, sw2;

unsigned long interval, last_cycle;
unsigned long loop_micros;

// Set new state
void set_state(fsm_t& fsm, int new_state)
{
  if (fsm.state != new_state) {  // if the state changed tis is reset
      
        fsm.state = new_state;
        fsm.tes = millis();
        fsm.tis = 0;
      
  }
}

void set_led(fsm_t& fsm, uint8_t* LED)
{
  if (fsm.state == 0) { // Start state
    *LED = 0;
  } else if (fsm.state == 1) { // Led ON state
    *LED = 1;
  } else if (fsm.state == 2) { // Led OFF while counting state
    *LED = 0;
  } else if (fsm.state == 3) { // Led OFF while paused state
    *LED = 0;
  } else if (fsm.state == 4) { // Led OFF while paused state
    *LED = 1;
  }
}

void set_conditions(fsm_t& fsm, uint8_t S1, uint8_t prevS1, uint8_t S2, uint8_t prevS2, unsigned long cur_time, int pos)
{
      // Calculate next state for the first state machine
      if (fsm.state == 0 && (S1 || S2)) {
        fsm.new_state = 1;
      } else if(fsm.state == 1 && S1 && !prevS1) {
        fsm.tes = millis();
      } else if(fsm.state == 1 && S2 && !prevS2) {
        fsm.new_state = 4;
        fsm.tis_pause = fsm.tis;
        fsm.pause = 1;
      } else if (fsm.state == 1 && (fsm.tis + fsm.tis_pause) >= (2000*pos)) {
        fsm.new_state = 2;
        fsm.tis_pause = 0;
        if (pos == 6) end_cycle = 1;
      } else if (fsm.state == 4 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 3;
      } else if (fsm.state == 4 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 1;
      } else if (fsm.state == 3 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 4;
      } else if (fsm.state == 3 && S2 && !prevS2 && fsm.pause){
        fsm.new_state = 1;
       // fsm1.tes = cur_time - fsm1.tis_pause;
        fsm.pause = 0;
      // } else if(fsm1.state == 2 && S2 && !prevS2) {
      //   fsm1.state = 2;
      //   fsm1.pause = 1;
      } else if (fsm.state == 2 && end_cycle) {
        fsm.new_state = 1;
        if (pos == 6) {
          end_cycle = 0;
          }
      } else if (fsm.state == 2 && S1 && !prevS1){
        fsm.new_state = 1;
      }
}

// int check_switch(uint8_t S, uint8_t S_real, fsm_t& fsm) {
//   if (S_real && S) {return 0;}
//   if (!S_real && !S) {return 0;}
//   if (S_real && !S && fsm.tis > 500) {
//     fsm.tes = millis();
//     fsm.tis = 0;
//     return 1;
//   }
//   if (!S_real && S) {return 0;}

//   return 0;
// }

void setup() 
{
  pinMode(LED1_pin, OUTPUT);
  pinMode(LED2_pin, OUTPUT);
  pinMode(LED3_pin, OUTPUT);
  pinMode(LED4_pin, OUTPUT);
  pinMode(LED5_pin, OUTPUT);
  pinMode(LED6_pin, OUTPUT);
  pinMode(LED7_pin, OUTPUT);
  pinMode(S1_pin, INPUT);
  pinMode(S2_pin, INPUT);

  // Start the serial port with 115200 baudrate
  Serial.begin(115200);

  sw1.tes = millis();
  sw1.tis = 0;
  sw1.tes = millis();  
  sw2.tis = 501;

  interval = 10;
  set_state(fsm1, 0);
  set_state(fsm2, 0);  
  set_state(fsm3, 0);  
  set_state(fsm4, 0);  
  set_state(fsm5, 0);  
  set_state(fsm6, 0);    
  set_state(fsm7, 0);  
}

void loop() 
{
    // To measure the time between loop() calls
    //unsigned long last_loop_micros = loop_micros; 
    
    // Do this only every "interval" miliseconds 
    // It helps to clear the switches bounce effect
    unsigned long now = millis();
    if (now - last_cycle > interval) {
      loop_micros = micros();
      last_cycle = now;
      
      // Read the inputs
      // sw1.tis = cur_time - sw1.tes;
      // sw2.tis = cur_time - sw2.tes;
      prevS1 = S1;
      prevS2 = S2;
      // S1_real = !digitalRead(S1_pin);
      // S2_real = !digitalRead(S2_pin);
      // S1 = check_switch(S1, S1_real, sw1);
      // S2 = check_switch(S2, S2_real, sw2);

      S1 = !digitalRead(S1_pin);
      S2 = !digitalRead(S2_pin);

      // FSM processing

      // Update tis for all state machines
      unsigned long cur_time = millis();   // Just one call to millis()
      fsm1.tis = cur_time - fsm1.tes;
      fsm2.tis = cur_time - fsm2.tes;
      fsm3.tis = cur_time - fsm3.tes;
      fsm4.tis = cur_time - fsm4.tes; 
      fsm5.tis = cur_time - fsm5.tes;
      fsm6.tis = cur_time - fsm6.tes;  

      // Calculate next state for the first state machine
      set_conditions(fsm1, S1, prevS1, S2, prevS2, cur_time, 1);
      set_conditions(fsm2, S1, prevS1, S2, prevS2, cur_time, 2);
      set_conditions(fsm3, S1, prevS1, S2, prevS2, cur_time, 3);
      set_conditions(fsm4, S1, prevS1, S2, prevS2, cur_time, 4);
      set_conditions(fsm5, S1, prevS1, S2, prevS2, cur_time, 5);
      set_conditions(fsm6, S1, prevS1, S2, prevS2, cur_time, 6);

      // if (fsm1.state == 0 && (S1 || S2)) {
      //   fsm1.new_state = 1;
      // } else if(fsm1.state == 1 && S1 && !prevS1) {
      //   fsm1.tes = millis();
      // } else if(fsm1.state == 1 && S2 && !prevS2) {
      //   fsm1.tis_pause = fsm1.tis;
      //   fsm1.pause = 1;
      // } else if (fsm1.state == 1 && fsm1.tis > 2000) {
      //   fsm1.new_state = 2;
      // } else if (fsm1.state == 1 && fsm1.tis > 1000 && fsm1.pause) {
      //   fsm1.new_state = 3;
      // } else if (fsm1.state == 1 && S2 && !prevS2 && fsm1.pause) {
      //   fsm1.tes = cur_time - fsm1.tis_pause;
      //   fsm1.pause = 0;
      // } else if (fsm1.state == 3 && fsm1.tis > 1000 && fsm1.pause) {
      //   fsm1.new_state = 1;
      // } else if (fsm1.state == 3 && S2 && !prevS2 && fsm1. pause){
      //   fsm1.state = 1;
      //  // fsm1.tes = cur_time - fsm1.tis_pause;
      //   fsm1.pause = 0;
      // // } else if(fsm1.state == 2 && S2 && !prevS2) {
      // //   fsm1.state = 2;
      // //   fsm1.pause = 1;
      // } else if (fsm1.state == 2 && end_cycle) {
      //   fsm1.new_state = 1;
      // } else if (fsm1.state == 2 && S1 && !prevS1){
      //   fsm1.new_state = 1;
      // }

      // if (fsm2.state == 0 && (S1 || S2)){
      //   fsm2.new_state = 1;
      // } else if(fsm2.state == 1 && S1 && !prevS1) {
      //   fsm2.tes = millis();
      // } else if(fsm2.state == 1 && S2 && !prevS2) {
      //   fsm2.tis_pause = fsm2.tis;
      //   fsm2.pause = 1;
      // } else if (fsm2.state == 1 && fsm2.tis > 4000){
      //   fsm2.new_state = 2;
      // } else if (fsm2.state == 1 && fsm2.tis > 1000 && fsm2.pause) {
      //   fsm2.new_state = 2;
      // } else if (fsm2.state == 1 && S2 && !prevS2 && fsm2.pause){
      //   fsm2.tes = cur_time - fsm2.tis_pause;
      //   fsm2.pause = 0;
      // } else if (fsm2.state == 2 && fsm2.tis > 1000 && fsm2.pause) {
      //   fsm2.new_state = 1;
      // } else if (fsm2.state == 2 && S2 && !prevS2 && fsm2.pause){
      //   fsm2.state = 2;
      //   fsm2.tes = cur_time - fsm2.tis_pause;
      //   fsm2.pause = 0;
      // } else if (fsm2.state == 2 && end_cycle){
      //   fsm2.new_state = 1;
      // } else if (fsm2.state == 2 && S1 && !prevS1){
      //   fsm2.new_state = 1;
      // }

      // if (fsm3.state == 0 && (S1 || S2)){
      //   fsm3.new_state = 1;
      // } else if(fsm3.state == 1 && S1 && !prevS1) {
      //   fsm3.tes = millis();
      // } else if(fsm3.state == 1 && S2 && !prevS2) {
      //   fsm3.tis_pause = fsm3.tis;
      //   fsm3.pause = 1;
      // } else if (fsm3.state == 1 && fsm3.tis > 6000){
      //   fsm3.new_state = 2;
      // } else if (fsm3.state == 1 && fsm3.tis > 1000 && fsm3.pause) {
      //   fsm3.new_state = 2;
      // } else if (fsm3.state == 1 && S2 && !prevS2 && fsm3.pause){
      //   fsm3.tes = cur_time - fsm3.tis_pause;
      //   fsm3.pause = 0;
      // } else if (fsm3.state == 2 && fsm3.tis > 1000 && fsm3.pause) {
      //   fsm3.new_state = 1;
      // } else if (fsm3.state == 2 && S2 && !prevS2 && fsm3.pause){
      //   fsm3.state = 2;
      //   fsm3.tes = cur_time - fsm3.tis_pause;
      //   fsm3.pause = 0;
      // } else if (fsm3.state == 2 && end_cycle){
      //   fsm3.new_state = 1;
      // } else if (fsm3.state == 2 && S1 && !prevS1){
      //   fsm3.new_state = 1;
      // }

      // if (fsm4.state == 0 && (S1 || S2)){
      //   fsm4.new_state = 1;
      // } else if(fsm4.state == 1 && S1 && !prevS1) {
      //   fsm4.tes = millis();
      // } else if(fsm4.state == 1 && S2 && !prevS2) {
      //   fsm4.tis_pause = fsm4.tis;
      //   fsm4.pause = 1;
      // } else if (fsm4.state == 1 && fsm4.tis > 8000){
      //   fsm4.new_state = 2;
      // } else if (fsm4.state == 1 && fsm4.tis > 1000 && fsm4.pause) {
      //   fsm4.new_state = 2;
      // } else if (fsm4.state == 1 && S2 && !prevS2 && fsm4.pause){
      //   fsm4.tes = cur_time - fsm4.tis_pause;
      //   fsm4.pause = 0;
      // } else if (fsm4.state == 2 && fsm4.tis > 1000 && fsm4.pause) {
      //   fsm4.new_state = 1;
      // } else if (fsm4.state == 2 && S2 && !prevS2 && fsm4.pause){
      //   fsm4.state = 2;
      //   fsm4.tes = cur_time - fsm4.tis_pause;
      //   fsm4.pause = 0;
      // } else if (fsm4.state == 2 && end_cycle){
      //   fsm4.new_state = 1;
      // } else if (fsm4.state == 2 && S1 && !prevS1){
      //   fsm4.new_state = 1;
      // }

      // if (fsm5.state == 0 && (S1 || S2)){
      //   fsm5.new_state = 1;
      // } else if(fsm5.state == 1 && S1 && !prevS1) {
      //   fsm5.tes = millis();
      // } else if(fsm5.state == 1 && S2 && !prevS2) {
      //   fsm5.tis_pause = fsm5.tis;
      //   fsm5.pause = 1;
      // } else if (fsm5.state == 1 && fsm5.tis > 10000){
      //   fsm5.new_state = 2;
      // } else if (fsm5.state == 1 && fsm5.tis > 1000 && fsm5.pause) {
      //   fsm5.new_state = 2;
      // } else if (fsm5.state == 1 && S2 && !prevS2 && fsm5.pause){
      //   fsm5.tes = cur_time - fsm5.tis_pause;
      //   fsm5.pause = 0;
      // } else if (fsm5.state == 2 && fsm5.tis > 1000 && fsm5.pause) {
      //   fsm5.new_state = 1;
      // } else if (fsm5.state == 2 && S2 && !prevS2 && fsm5.pause){
      //   fsm5.state = 2;
      //   fsm5.tes = cur_time - fsm5.tis_pause;
      //   fsm5.pause = 0;
      // } else if (fsm5.state == 2 && end_cycle){
      //   fsm5.new_state = 1;
      // } else if (fsm5.state == 2 && S1 && !prevS1){
      //   fsm5.new_state = 1;
      // }

      // if (fsm6.state == 0 && (S1 || S2)){
      //   fsm6.new_state = 1;
      // } else if(fsm6.state == 1 && S1 && !prevS1) {
      //   fsm6.tes = 0;
      // } else if(fsm6.state == 1 && S2 && !prevS2) {
      //   fsm6.tis_pause = fsm6.tis;
      //   fsm6.pause = 1;
      // } else if (fsm6.state == 1 && fsm6.tis > 12000) {
      //   fsm6.new_state = 2;
      //   end_cycle = 1;
      // } else if (fsm6.state == 1 && fsm6.tis > 1000 && fsm6.pause) {
      //   fsm6.new_state = 2;
      // } else if (fsm6.state == 1 && S2 && !prevS2 && fsm6.pause){
      //   fsm6.tes = cur_time - fsm6.tis_pause;
      //   fsm6.pause = 0;
      // } else if (fsm6.state == 2 && fsm6.tis > 1000 && fsm6.pause) {
      //   fsm6.new_state = 1;
      // } else if (fsm6.state == 2 && S2 && !prevS2 && fsm6.pause){
      //   fsm6.state = 2;
      //   fsm6.tes = cur_time - fsm6.tis_pause;
      //   fsm6.pause = 0;
      // } else if (fsm6.state == 2 && end_cycle) {
      //   fsm6.new_state = 1;
      //   end_cycle = 0;
      // } else if (fsm6.state == 2 && S1 && !prevS1){
      //   fsm6.new_state = 1;
      // }

      // Calculate next state for the second state machine
      /*if (fsm2.state == 0 && S2 && !prevS2){
        fsm2.new_state = 1;
      } else if (fsm2.state == 1 && S2 && !prevS2){
        fsm2.new_state = 0;
      }*/

      // Update the states
      set_state(fsm1, fsm1.new_state);
      set_state(fsm2, fsm2.new_state);
      set_state(fsm3, fsm3.new_state);
      set_state(fsm4, fsm4.new_state);
      set_state(fsm5, fsm5.new_state);
      set_state(fsm6, fsm6.new_state);

      // Actions set by the current state of the first state machine
      set_led(fsm1, &LED_1);
      set_led(fsm2, &LED_2);
      set_led(fsm3, &LED_3);
      set_led(fsm4, &LED_4);
      set_led(fsm5, &LED_5);
      set_led(fsm6, &LED_6);

      // A more compact way
      // LED_1 = (fsm1.state == 1);
      // LED_1 = (state == 1)||(state ==2);  if LED1 must be set in states 1 and 2
      
      // Actions set by the current state of the second state machine
      // LED_2 = (fsm2.state == 0);

      // Set the outputs
      digitalWrite(LED1_pin, LED_1);
      digitalWrite(LED2_pin, LED_2);
      digitalWrite(LED3_pin, LED_3);
      digitalWrite(LED4_pin, LED_4);
      digitalWrite(LED5_pin, LED_5);
      digitalWrite(LED6_pin, LED_6);

      // Debug using the serial port
      Serial.print("S1: ");
      Serial.print(S1);

      Serial.print(" S2: ");
      Serial.print(S2);

      Serial.print(" fsm1.state: ");
      Serial.print(fsm1.state);

      Serial.print(" fsm2.state: ");
      Serial.print(fsm2.state);

      Serial.print(" fsm3.state: ");
      Serial.print(fsm3.state);

      Serial.print(" fsm4.state: ");
      Serial.print(fsm4.state);

      Serial.print(" fsm5.state: ");
      Serial.print(fsm5.state);

      Serial.print(" fsm6.state: ");
      Serial.print(fsm6.state);      

      Serial.print(" LED_1: ");
      Serial.print(LED_1);

      Serial.print(" LED_2: ");
      Serial.print(LED_2);

      Serial.print(" LED_3: ");
      Serial.print(LED_3);

      Serial.print(" LED_4: ");
      Serial.print(LED_4);

      Serial.print(" LED_5: ");
      Serial.print(LED_5);

      Serial.print(" LED_6: ");
      Serial.print(LED_6);

      Serial.print(" loop: ");
      Serial.println(micros() - loop_micros);
    }
    
}
