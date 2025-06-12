#ifndef IVY_RELAY_H
#define IVY_RELAY_H

#include "Arduino.h"
const int N_OUTLETS = 8;

typedef struct Outlet {
  const unsigned int pin;
  unsigned int mode;
  unsigned int current_state;
  unsigned int auto_state;
  unsigned int manual_state;
  const char* label;
};

#define MODE_AUTO   0
#define MODE_MANUAL 1

Outlet outlet[N_OUTLETS] = {
  {17, MODE_AUTO, 0, 0, 0, "Luces x3"},
  {16, MODE_AUTO, 0, 0, 0, "Luces x2"},
  {18, MODE_AUTO, 0, 0, 0, "Ventilador"},
  {23, MODE_AUTO, 0, 0, 0, "Extractor"},
  {19, MODE_AUTO, 0, 0, 0, "Humidifcador"},
  {25, MODE_AUTO, 0, 0, 0, "Toma 6"},
  {21, MODE_AUTO, 0, 0, 0, "Toma 7"},
  {22, MODE_AUTO, 0, 0, 0, "Toma 8"}
};

void ivy_outlet_begin();
void ivy_outlet_update();

void ivy_outlet_begin(){
  for(int i = 0; i < N_OUTLETS; i++){
    pinMode(outlet[i].pin, OUTPUT);
    digitalWrite(outlet[i].pin, 1);
  }
}

void ivy_outlet_update(){
  for(int i = 0; i < N_OUTLETS; i++){
    if (outlet[i].mode == MODE_MANUAL) outlet[i].current_state = outlet[i].manual_state;
    else outlet[i].current_state = outlet[i].auto_state;
    digitalWrite(outlet[i].pin, (outlet[i].current_state != 0) ? 0 : 1);
  } 
}

#endif