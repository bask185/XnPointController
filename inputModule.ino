#include "src/io.h"
#include "src/date.h"
#include "src/version.h"
#include "src/macros.h"
#include "src/XpressNetMaster.h"
#include "src/debounceClass.h"
#include "src/eeprom.h"



/* THINGS TODO 

 * last pos should be stored in eeprom as well, so inversed directions can be used
 * following The above line it is also needed to process the inverse stat when
   button is pressed.


*/

/************ VARIABLES & CONSTANTS ***********/
const int nInputs = 16 ;

const int inputPin[ nInputs ] =
{
    Inp1,   Inp2,   Inp3,   Inp4, 
    Inp5,   Inp6,   Inp7,   Inp8,
    Inp9,   Inp10,  Inp11,  Inp12,
    Inp13,  Inp14,  Inp15,  Inp16
} ;

Debounce input [] =
{
    Debounce(  Inp1 ) , Debounce(  Inp2 ) , Debounce(  Inp3 ) , Debounce(  Inp4 ) ,
    Debounce(  Inp5 ) , Debounce(  Inp6 ) , Debounce(  Inp7 ) , Debounce(  Inp8 ) ,
    Debounce(  Inp9 ) , Debounce( Inp10 ) , Debounce( Inp11 ) , Debounce( Inp12 ) ,
    Debounce( Inp13 ) , Debounce( Inp14 ) , Debounce( Inp15 ) , Debounce( Inp16 ) ,
} ;

Debounce store( storeSwitch ) ;

#ifndef DEBUG
XpressNetMasterClass Xnet ;
#endif

enum modes
{
    TEACHIN,
    NORMAL,
} mode = NORMAL ;

volatile uint16_t lastAddress = 9 ; // dummy value for testing
uint8_t  lastPos  ;

uint32_t timeStamp ;
uint8_t  newData ;
uint8_t  counter ;
uint8_t  speed ;
uint8_t  sensorState ;
uint8_t  state ;

void debounce()
{
    REPEAT_MS( 50 )
    {
        for (int i = 0; i < nInputs; i++)
        {
            input[i].debounceInputs() ;
        }
        store.debounceInputs() ;

    } END_REPEAT
}

void readSwitches()
{
    for (int pin = 0; pin < nInputs-1 ; pin ++ )
    {
        uint8_t btnState = input[pin].readInput() ;

        if( btnState == FALLING || btnState == RISING )
        {
            if( mode == NORMAL)                                                 // set point
            {
                uint16_t address = loadPoint( pin ) ;                           // fetch address & state from EEPROM
                
                uint8_t state = address >> 15 ;                                  // stuff MSB in state
                address &= 0x03FF ;                                              // remove MSB from address 
                //address -- ;
                
                if( btnState == FALLING ) state ^= 1 ;

                #ifndef DEBUG
                Xnet.SetTrntPos( address, state, 1) ;
                delay(20) ;
                Xnet.SetTrntPos( address, state, 0) ;                               // maybe not needed? should check for this
                #else
                Serial.print("pin = " ) ;               Serial.println( pin ) ;
                Serial.print("address in EEPROM = ");   Serial.println(address) ;
                Serial.print("state = ");               Serial.println(state) ;
                #endif
            }
            else                                                                    // store point
            {   
                uint16_t toStore ;
                if( btnState ==  RISING ) toStore = lastAddress | (0x1 << 15) ;     // set state according to button position
                else                      toStore = lastAddress | (0x0 << 15) ;

                storePoint( pin, toStore ) ;                                         // store in EEPROM


                #ifdef DEBUG
                Serial.print("\nconnecting pin "); Serial.print(pin) ;
                Serial.print(" to address ") ; Serial.println( lastAddress ) ;
                Serial.print("with state ") ; Serial.println( toStore ) ;
                #endif                
            }
        }
    }
}
/************* SETUP *************/
void setup()
{
    initIO() ;
    pinMode( Inp16, OUTPUT ) ;   // blink LED so I can see that arduino has booted
    digitalWrite( Inp16, HIGH ) ;
    delay(100) ;
    digitalWrite( Inp16, LOW ) ;
    delay(100) ;
    digitalWrite( Inp16, HIGH ) ;
    delay(100) ;
    digitalWrite( Inp16, LOW ) ;
    delay(100) ;
    digitalWrite( Inp16, HIGH ) ;
    delay(100) ;
    digitalWrite( Inp16, LOW ) ;

    debounce() ;
    delay( 100 ) ;
    debounce() ;
    delay( 100 ) ;
    debounce() ;
    delay( 100 ) ;
    debounce() ;
    delay( 100 ) ;
    debounce() ;


    #ifdef DEBUG
    // storePoint(  0, 0) ;
    // storePoint(  1, 1) ;
    // storePoint(  2, 2) ;
    // storePoint(  3, 3) ;
    // storePoint(  4, 4) ;
    // storePoint(  5, 5) ;
    // storePoint(  6, 6) ;
    // storePoint(  7, 7) ;
    // storePoint(  8, 8) ;
    // storePoint(  9, 9^0x8000 ) ;
    // storePoint( 10, 10^0x8000 ) ;
    // storePoint( 11, 11^0x8000 ) ;
    // storePoint( 12, 12^0x8000 ) ;
    // storePoint( 13, 13^0x8000 ) ;
    // storePoint( 14, 14^0x8000 ) ;
    // storePoint( 15, 15^0x8000 ) ;
    // storePoint( 16, 16^0x8000 ) ;

    Serial.begin(115200);
    Serial.println("booted");

    for( int i = 0 ; i < 16 ; i ++ )
    {
        uint16_t address = loadPoint( i ) ;    
        Serial.print(i) ; Serial.print( " address raw = ") ; Serial.print(address);    // I often use the DEBUG flag in combination with EEPROM to fetch XpressNet info when live and display it in Debug mode.

        uint16_t state = address >> 15 ;                                  // stuff MSB in state
        address &= 0x3FF ; 

        Serial.print( " address = ") ; Serial.print(address); Serial.print( " state = ") ; Serial.println(state);

    }

    #else
    Xnet.setup( Loco128 , rs485dir ) ;
    #endif
}

volatile bool flash ;


void loop()
{
    REPEAT_MS( 200 )
    {
        if( flash )
        {
            flash = false ;
            digitalWrite( Inp16, HIGH ) ;
        }
        else if( digitalRead( Inp16 ) == HIGH )
        {
            digitalWrite( Inp16, LOW ) ;
        }
    } END_REPEAT

    debounce() ;
    readSwitches() ;

    mode = NORMAL ;
    if( store.readInput() == HIGH ) { mode =  NORMAL ; }
    if( store.readInput() ==  LOW ) { mode = TEACHIN ; }
    
    #ifndef DEBUG
    Xnet.update() ;
    #endif

}

void notifyXNetTrnt( uint16_t Address,  uint8_t data )
{
    if( bitRead( data, 3 ) == 1 )
    {
        flash = true ;

        if( mode == NORMAL ) return ;

        lastAddress  = Address;// & 0x03FF ;

        pinMode(12, INPUT_PULLUP);                                              // PREVENTS THIS ENTIRE FUNCTION FROM BEING OPTIMALIZED AWAY BY THE LINKER
    }
}