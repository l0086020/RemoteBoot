#ifndef __MAIN_H
#define __MAIN_H

#include <Arduino.h>
#include <Blinker.h>
#include <Ticker.h>

typedef enum
{
    PowerSwitch     = 0x00,
    PowerForceOFF   = 0x01,
    PowerReset      = 0x02
} PowerStateTypeDef;

#endif
