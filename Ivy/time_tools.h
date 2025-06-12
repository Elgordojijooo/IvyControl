#ifndef IVY_TIME_TOOLS_H
#define IVY_TIME_TOOLS_H

#include "ivy_nuts_and_bolts.h"

// Helpers!
bool entre_horas(int a, int b) {
  if (a <= b){
    return (a <= timeinfo.tm_hour) && (timeinfo.tm_hour < b);
  }
  return (timeinfo.tm_hour >= a) || (timeinfo.tm_hour < b);
}

#endif