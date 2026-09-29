#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
// ---------------------------------------------------------------------
// Live bay state
// ---------------------------------------------------------------------
extern String bayStatus;
extern float voltage, current, power, energyWh, temperature, humidity;

extern unsigned long sessionStartMs;

// ---------------------------------------------------------------------
// Timing / debounce bookkeeping
// ---------------------------------------------------------------------
extern float predictedArrivalProb;
extern int predictedDurationMin;
extern int lastHourOfDay;

extern String loadDecision;
extern int throttleLevel;
extern bool manualOverrideActive;

extern float predictionThreshold;
extern int peakTariffStartHr;
extern int peakTariffEndHr;
extern bool overloadActive;
extern int overloadCurrentA;
extern int maxStationLoadW;

#endif
