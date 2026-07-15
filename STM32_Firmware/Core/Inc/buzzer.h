#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f4xx_hal.h"

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Set(uint8_t state);

#endif
