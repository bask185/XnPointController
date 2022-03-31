#include <Arduino.h>
#include "io.h"
extern void initIO(void) {
	pinMode(rs485dir, OUTPUT);
	pinMode(Inp1, INPUT_PULLUP);
	pinMode(Inp2, INPUT_PULLUP);
	pinMode(Inp3, INPUT_PULLUP);
	pinMode(Inp4, INPUT_PULLUP);
	pinMode(Inp5, INPUT_PULLUP);
	pinMode(Inp6, INPUT_PULLUP);
	pinMode(Inp7, INPUT_PULLUP);
	pinMode(Inp8, INPUT_PULLUP);
	pinMode(Inp9, INPUT_PULLUP);
	pinMode(Inp10, INPUT_PULLUP);
	pinMode(Inp11, INPUT_PULLUP);
	pinMode(Inp12, INPUT_PULLUP);
	pinMode(Inp13, INPUT_PULLUP);
	pinMode(Inp14, INPUT_PULLUP);
	pinMode(Inp15, INPUT_PULLUP);
	pinMode(Inp16, OUTPUT);
	pinMode(storeSwitch, INPUT_PULLUP);
}