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

int interval_counter = 1;

int blink_mode = 0;
int blink_mode_counter = 0;

int finish_mode = 0;

int flicker_counter = 0;

int timer = 0;
int timer_led_7 = 0;

int led_7_counter = 0;

// Output variables
uint8_t LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7;

// Our finite state machines
fsm_t fsm, sw1, sw2;

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
  sw2.tis = 0;

  interval = 10;
  set_state(fsm, 0);
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

      switch (interval_counter) {
        case 0:
          timer_led_7 = 1000;
          timer = 1000;
          break;
        case 1:
          timer_led_7 = 2000;
          timer = 2000;
          break;
        case 2:
          timer_led_7 = 4000;
          timer = 4000;
          break;
        case 3:
          timer_led_7 = 8000;
          timer = 8000;
          break;
        default:
          timer_led_7 = 2000;
          timer = 2000;
          break;
      }

      // FSM processing

      // Update tis for all state machines
      unsigned long cur_time = millis();   // Just one call to millis()
      fsm.tis = cur_time - fsm.tes;

//Mudança de Estados

      // Estado Inicial
      if (fsm.state == 0 && (S1 || S2)) {
        fsm.new_state = 1;
      
      // Reset
      } else if((fsm.state == 1 || fsm.state == 2|| fsm.state == 3|| fsm.state == 4|| fsm.state == 5|| fsm.state == 6) && S1 && fsm.tis < 3000) {
        fsm.new_state = 1;
        //Serial.print("\n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n ");

      // Ativar Pausa
      } else if((fsm.state == 1 || fsm.state == 61) && S2 && !prevS2) {
        fsm.new_state = 51;
        fsm.tis_pause = fsm.tis;
        fsm.pause = 1;
      } else if((fsm.state == 2 || fsm.state == 62) && S2 && !prevS2) {
        fsm.new_state = 52;
        fsm.tis_pause = fsm.tis;
        fsm.pause = 1;
      } else if((fsm.state == 3 || fsm.state == 63) && S2 && !prevS2) {
        fsm.new_state = 53;
        fsm.tis_pause = fsm.tis;
        fsm.pause = 1;
      } else if((fsm.state == 4 || fsm.state == 64) && S2 && !prevS2) {
        fsm.new_state = 54;
        fsm.tis_pause = fsm.tis;
        fsm.pause = 1;
      } else if((fsm.state == 5 || fsm.state == 65) && S2 && !prevS2) {
        fsm.new_state = 55;
        fsm.tis_pause = fsm.tis;
        fsm.pause = 1;
      } else if((fsm.state == 6 || fsm.state == 66) && S2 && !prevS2) {
        fsm.new_state = 56;
        fsm.tis_pause = fsm.tis;
        fsm.pause = 1;

      //Double Click 
      }else if (fsm.state == 52 && S2 && !prevS2 && fsm.pause && fsm.tis < 500) {
        fsm.pause = 0;
        fsm.new_state = 1;
      }else if (fsm.state == 53 && S2 && !prevS2 && fsm.pause && fsm.tis < 500) {
        fsm.pause = 0;
        fsm.new_state = 2;
      }else if (fsm.state == 54 && S2 && !prevS2 && fsm.pause && fsm.tis < 500) {
        fsm.pause = 0;
        fsm.new_state = 3;
      }else if (fsm.state == 55 && S2 && !prevS2 && fsm.pause && fsm.tis < 500) {
        fsm.pause = 0;
        fsm.new_state = 4;
      }else if (fsm.state == 56 && S2 && !prevS2 && fsm.pause && fsm.tis < 500) {
        fsm.pause = 0;
        fsm.new_state = 5;

      //Config Mode
      } else if(fsm.state == 1 && S1 && prevS1 && fsm.tis >= 3000) {
        fsm.new_state = 31; 
      } else if(fsm.state == 31 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 32;
      } else if(fsm.state == 32 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 33;
      } else if(fsm.state == 33 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 31;
      } else if(fsm.state == 41 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 42;
      } else if(fsm.state == 42 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 43;
      } else if(fsm.state == 43 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 41;
      } else if(fsm.state == 311 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 32;
      } else if(fsm.state == 321 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 33;
      } else if(fsm.state == 331 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 31;
      } else if(fsm.state == 411 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 42;
      } else if(fsm.state == 421 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 43;
      } else if(fsm.state == 431 && S1 && !prevS1) {
        led_7_counter = 0;        
        fsm.new_state = 41;
      } else if(( fsm.state == 43 || fsm.state == 42 || fsm.state == 41 || fsm.state == 33 || fsm.state == 32 || fsm.state == 31 || fsm.state == 431 || fsm.state == 421 || fsm.state == 411 || fsm.state == 331 || fsm.state == 321 || fsm.state == 311) && S1 && prevS1 && fsm.tis >= 3000) {
        fsm.new_state = 1;

      //Config mode
      } else if((fsm.state == 31 || fsm.state == 41 || fsm.state == 311 || fsm.state == 411) && S2 && !prevS2) {
        interval_counter = interval_counter + 1;
        if (interval_counter > 4) interval_counter = 0;
      } else if((fsm.state == 32 || fsm.state == 42 || fsm.state == 321 || fsm.state == 421) && S2 && !prevS2) {
        blink_mode++;
        if (blink_mode >= 3) blink_mode = 0;
      } else if((fsm.state == 33 || fsm.state == 43 || fsm.state == 331 || fsm.state == 431) && S2 && !prevS2) {
        finish_mode = !finish_mode;

      //Config Mode (aceso)
      } else if (fsm.state == 31 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter <= (timer_led_7/1000))  {
        fsm.new_state = 41;
        led_7_counter++;
      } else if (fsm.state == 32 && fsm.tis > 1000 && !(S1 && prevS1))   {
        fsm.new_state = 42;
      } else if (fsm.state == 33 && fsm.tis > 100 && !(S1 && prevS1) && finish_mode && flicker_counter > 10)   {
        fsm.new_state = 431;
        flicker_counter = 0;
      } else if (fsm.state == 33 && fsm.tis > 100 && !(S1 && prevS1) && finish_mode)   {
        fsm.new_state = 331;
        flicker_counter ++;
      } else if (fsm.state == 31 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter > (timer_led_7/1000))  {
        fsm.new_state = 411;
        led_7_counter = 0;
      } else if (fsm.state == 32 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 421;
      } else if (fsm.state == 33 && fsm.tis > 1000 && !(S1 && prevS1) && !finish_mode)  {
        fsm.new_state = 431;

      //Config Mode (apagado)
      } else if (fsm.state == 41 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter <= (timer_led_7/1000)) {
        fsm.new_state = 31;
        led_7_counter++;
      } else if (fsm.state == 42 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 32;
      } else if (fsm.state == 43 && fsm.tis > 100 && !(S1 && prevS1) && !finish_mode && flicker_counter > 10)  {
        fsm.new_state = 331;
        flicker_counter = 0;
      } else if (fsm.state == 43 && fsm.tis > 100 && !(S1 && prevS1) && finish_mode)  {
        fsm.new_state = 431;
        flicker_counter ++;
      } else if (fsm.state == 41 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter > (timer_led_7/1000))  {
        fsm.new_state = 311;
        led_7_counter = 0;
      } else if (fsm.state == 42 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 321;
      } else if (fsm.state == 43 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 331;

      //Config Mode (aceso) LED_7_ON
      } else if (fsm.state == 311 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter <= (timer_led_7/1000)) {
        fsm.new_state = 411;
        led_7_counter++;
      } else if (fsm.state == 321 && fsm.tis > 1000 && !(S1 && prevS1)) {
        fsm.new_state = 421;
      } else if (fsm.state == 331 && fsm.tis > 100 && !(S1 && prevS1) && finish_mode && flicker_counter > 10)  {
        fsm.new_state = 43;
        flicker_counter = 0;
      } else if (fsm.state == 331 && fsm.tis > 100 && !(S1 && prevS1) && finish_mode)  {
        fsm.new_state = 33;
        flicker_counter ++;
      } else if (fsm.state == 331 && fsm.tis > 1000 && !(S1 && prevS1) && !finish_mode) {
        fsm.new_state = 431;
      } else if (fsm.state == 311 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter > (timer_led_7/1000))  {
        fsm.new_state = 41;
        led_7_counter = 0;
      } else if (fsm.state == 321 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 42;
      } else if (fsm.state == 331 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 43;

      //Config Mode (apagado) LED_7_ON
      } else if (fsm.state == 411 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter <= (timer_led_7/1000)) {
        fsm.new_state = 311;
        led_7_counter++;
      } else if (fsm.state == 421 && fsm.tis > 1000 && !(S1 && prevS1)) {
        fsm.new_state = 321;
      } else if (fsm.state == 431 && fsm.tis > 100 && !(S1 && prevS1) && finish_mode && flicker_counter > 10)  {
        fsm.new_state = 33;
        flicker_counter = 0;
      } else if (fsm.state == 431 && fsm.tis > 100 && !(S1 && prevS1) && finish_mode)  {
        fsm.new_state = 43;
        flicker_counter ++;
      } else if (fsm.state == 431 && fsm.tis > 1000 && !(S1 && prevS1) && !finish_mode) {
        fsm.new_state = 331;
      } else if (fsm.state == 411 && fsm.tis > 1000 && !(S1 && prevS1) && led_7_counter > (timer_led_7/1000))  {
        fsm.new_state = 31;
        led_7_counter = 0;
      } else if (fsm.state == 421 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 32;
      } else if (fsm.state == 431 && fsm.tis > 1000 && !(S1 && prevS1))  {
        fsm.new_state = 33;

      // Passar ao próximo estado (blinkmode == 0)
      } else if (fsm.state == 1 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 0) {
        fsm.new_state = 2;
        fsm.tis_pause = 0;
      } else if (fsm.state == 2 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 0) {
        fsm.new_state = 3;
        fsm.tis_pause = 0;
      } else if (fsm.state == 3 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 0) {
        fsm.new_state = 4;
        fsm.tis_pause = 0;
      } else if (fsm.state == 4 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 0) {
        fsm.new_state = 5;
        fsm.tis_pause = 0;
      } else if (fsm.state == 5 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 0) {
        fsm.new_state = 6;
        fsm.tis_pause = 0;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 0 && !finish_mode) {
        fsm.new_state = 7;
        fsm.tis_pause = 0;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 0 && finish_mode) {
        fsm.new_state = 71;
        fsm.tis_pause = 0;
      } else if (fsm.state == 7 && (fsm.tis + fsm.tis_pause) >= 500 && blink_mode == 0) {
        fsm.new_state = 1;
        fsm.tis_pause = 0;
      } else if (fsm.state == 71 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 0) {
        fsm.new_state = 72;
        fsm.tis_pause = 0;
      } else if (fsm.state == 72 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 0) {
        fsm.new_state = 1;
        fsm.tis_pause = 0;     
      
      // Passar ao próximo estado (blinkmode == 1)

      } else if (fsm.state == 1 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter >= 9) {
        fsm.new_state = 2;
        fsm.tis_pause = 0;
        blink_mode_counter = 0;
      } else if (fsm.state == 1 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter > 0) {
        fsm.new_state = 61;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 1 && (fsm.tis + fsm.tis_pause) >= timer/2 && blink_mode == 1 && blink_mode_counter == 0) {
        fsm.new_state = 61;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 2 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter >= 9) {
        fsm.new_state = 3;
        fsm.tis_pause = 0;
        blink_mode_counter = 0;
      } else if (fsm.state == 2 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter > 0) {
        fsm.new_state = 62;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 2 && (fsm.tis + fsm.tis_pause) >= timer/2 && blink_mode == 1 && blink_mode_counter == 0) {
        fsm.new_state = 62;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 3 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter >= 9) {
        fsm.new_state = 4;
        fsm.tis_pause = 0;
        blink_mode_counter = 0;
      } else if (fsm.state == 3 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter > 0) {
        fsm.new_state = 63;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 3 && (fsm.tis + fsm.tis_pause) >= timer/2 && blink_mode == 1 && blink_mode_counter == 0) {
        fsm.new_state = 63;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 4 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter >= 9) {
        fsm.new_state = 5;
        fsm.tis_pause = 0;
        blink_mode_counter = 0;
      } else if (fsm.state == 4 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter > 0) {
        fsm.new_state = 64;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 4 && (fsm.tis + fsm.tis_pause) >= timer/2 && blink_mode == 1 && blink_mode_counter == 0) {
        fsm.new_state = 64;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 5 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter >= 9) {
        fsm.new_state = 6;
        fsm.tis_pause = 0;
        blink_mode_counter = 0;
      } else if (fsm.state == 5 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter > 0) {
        fsm.new_state = 65;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 5 && (fsm.tis + fsm.tis_pause) >= timer/2 && blink_mode == 1 && blink_mode_counter == 0) {
        fsm.new_state = 65;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= timer/2 && blink_mode == 1 && blink_mode_counter == 0) {
        fsm.new_state = 66;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter >= 9 && !finish_mode) {
        fsm.new_state = 7; //ver aqui
        fsm.tis_pause = 0;
        blink_mode_counter = 0;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter >= 9 && finish_mode) {
        fsm.new_state = 71; //ver aqui
        fsm.tis_pause = 0;
        blink_mode_counter = 0;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1 && blink_mode_counter > 0) {
        fsm.new_state = 66;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 7 && (fsm.tis + fsm.tis_pause) >= 500 && blink_mode == 1) {
        fsm.new_state = 1;
        fsm.tis_pause = 0;
      } else if (fsm.state == 71 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 72;
        fsm.tis_pause = 0;
      } else if (fsm.state == 72 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 1;
        fsm.tis_pause = 0;

      // Passar ao próximo estado (blinkmode == 2)
      } else if (fsm.state == 1 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 2) {
        fsm.new_state = 2;
        fsm.tis_pause = 0;
      } else if (fsm.state == 2 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 2) {
        fsm.new_state = 3;
        fsm.tis_pause = 0;
      } else if (fsm.state == 3 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 2) {
        fsm.new_state = 4;
        fsm.tis_pause = 0;
      } else if (fsm.state == 4 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 2) {
        fsm.new_state = 5;
        fsm.tis_pause = 0;
      } else if (fsm.state == 5 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 2) {
        fsm.new_state = 6;
        fsm.tis_pause = 0;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 2 && !finish_mode) {
        fsm.new_state = 7;
        fsm.tis_pause = 0;
      } else if (fsm.state == 6 && (fsm.tis + fsm.tis_pause) >= timer && blink_mode == 2 && finish_mode) {
        fsm.new_state = 71;
        fsm.tis_pause = 0;
      } else if (fsm.state == 7 && (fsm.tis + fsm.tis_pause) >= 500 && blink_mode == 2) {
        fsm.new_state = 1;
        fsm.tis_pause = 0;
      } else if (fsm.state == 71 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 2) {
        fsm.new_state = 72;
        fsm.tis_pause = 0;
      } else if (fsm.state == 72 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 2) {
        fsm.new_state = 1;
        fsm.tis_pause = 0;         

      //Piscar em Pausa vindo do doubleclick (Aceso)
      } else if (fsm.state == 51 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 21;
      } else if (fsm.state == 52 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 22;
      } else if (fsm.state == 53 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 23;
      } else if (fsm.state == 54 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 24;
      } else if (fsm.state == 55 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 25;
      } else if (fsm.state == 56 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 26;

      //Piscar em Pausa (Aceso)
      } else if (fsm.state == 11 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 21;
      } else if (fsm.state == 12 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 22;
      } else if (fsm.state == 13 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 23;
      } else if (fsm.state == 14 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 24;
      } else if (fsm.state == 15 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 25;
      } else if (fsm.state == 16 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 26;
        
      //Piscar em Pausa (Apagado)
      } else if (fsm.state == 21 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 11;
      } else if (fsm.state == 22 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 12;
      } else if (fsm.state == 23 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 13;
      } else if (fsm.state == 24 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 14;
      } else if (fsm.state == 25 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 15;
      } else if (fsm.state == 26 && fsm.tis > 1000 && fsm.pause) {
        fsm.new_state = 16;

      //Sair de Pausa (Aceso)
      }else if (fsm.state == 11 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 1;
      }else if (fsm.state == 12 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 2;
      }else if (fsm.state == 13 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 3;
      }else if (fsm.state == 14 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 4;
      }else if (fsm.state == 15 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 5;
      }else if (fsm.state == 16 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 6;

      //Sair de Pausa (Apagado)
     }else if (fsm.state == 21 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 1;
      }else if (fsm.state == 22 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 2;
      }else if (fsm.state == 23 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 3;
      }else if (fsm.state == 24 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 4;
      }else if (fsm.state == 25 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 5;
      }else if (fsm.state == 26 && S2 && !prevS2 && fsm.pause) {
        fsm.pause = 0;
        fsm.new_state = 6;

      // Piscar passar ao próximo estado (blinkmode == 1)
      } else if (fsm.state == 61 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 1;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 62 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 2;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 63 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 3;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 64 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 4;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 65 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 5;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      } else if (fsm.state == 66 && (fsm.tis + fsm.tis_pause) >= 100 && blink_mode == 1) {
        fsm.new_state = 6;
        fsm.tis_pause = 0;
        blink_mode_counter ++;
      }

  set_state(fsm, fsm.new_state);

//LED's em cada Estado

    //Estado Inicial
    if (fsm.state == 0) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;

    //Contagem decrescente
    } else if (fsm.state == 1) { 
      LED_1 = 1, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 2) { 
      LED_1 = 0, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 3) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 4) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 5) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 6) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 1, LED_7 = 0;
    
    //Fim do Ciclo
    } else if (fsm.state == 7) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 1;
    } else if (fsm.state == 71) { 
      LED_1 = 1, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 72) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;

    //Piscar (Aceso)
    } else if (fsm.state == 11) { 
      LED_1 = 1, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 12) { 
      LED_1 = 0, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 13) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 14) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 15) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 16) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 1, LED_7 = 0;

    //Piscar (Apagado)
    } else if (fsm.state == 21) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 22) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 23) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 24) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 25) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 26) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;

    //Configuração (Aceso)
    } else if (fsm.state == 31) { 
      LED_1 = 1, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 32) { 
      LED_1 = 0, LED_2 = 1, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 33) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 1, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;

    //Configuração (Aceso) LED_7 ligado
    } else if (fsm.state == 311) { 
      LED_1 = 1, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 1;
    } else if (fsm.state == 321) { 
      LED_1 = 0, LED_2 = 1, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 1;
    } else if (fsm.state == 331) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 1, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 1;

    //Configuração (Apagado)
    } else if (fsm.state == 41) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 42) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;
    } else if (fsm.state == 43) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;

    //Configuração (Apagado) LED_7 ligado
    } else if (fsm.state == 411) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 1;
    } else if (fsm.state == 421) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 1;
    } else if (fsm.state == 431) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 1;

    //Piscar (Aceso)
    } else if (fsm.state == 51) { 
      LED_1 = 1, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 52) { 
      LED_1 = 0, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 53) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 54) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 55) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 56) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 1, LED_7 = 0;

    //Piscar (Desligado) blink_mode==1
    } else if (fsm.state == 61) { 
      LED_1 = 0, LED_2 = 1, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 62) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 1, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 63) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 1, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 64) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 1, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 65) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 1, LED_7 = 0;
    } else if (fsm.state == 66) { 
      LED_1 = 0, LED_2 = 0, LED_3 = 0, LED_4 = 0, LED_5 = 0, LED_6 = 0, LED_7 = 0;

    }

      // Set the outputs
      digitalWrite(LED1_pin, LED_1);
      digitalWrite(LED2_pin, LED_2);
      digitalWrite(LED3_pin, LED_3);
      digitalWrite(LED4_pin, LED_4);
      digitalWrite(LED5_pin, LED_5);
      digitalWrite(LED6_pin, LED_6);
      digitalWrite(LED7_pin, LED_7);

      // Debug using the serial port
      Serial.print("S1: ");
      Serial.print(S1);

      Serial.print(" S2: ");
      Serial.print(S2);

      Serial.print(" blink: ");
      Serial.print(blink_mode);

      Serial.print(" fsm.state: ");
      Serial.print(fsm.state);    

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

      Serial.print(" LED_7: ");
      Serial.print(LED_7);

      Serial.print(" loop: ");
      Serial.println(micros() - loop_micros);
    }
    
}
