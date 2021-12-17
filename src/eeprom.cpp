#include <EEPROM.h>
#include "eeprom.h"

void storePoint( uint8_t pin, uint16_t address )
{
    uint16_t eeAddress = pin * 2 ;

    EEPROM.put( eeAddress, pin ) ;
}

uint16_t  loadPoint( uint8_t pin )
{
    uint16_t address ;
    uint16_t eeAddress = pin * 2 ;

    EEPROM.get( eeAddress, address ) ;

    return address ;
}