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

XpressNetMasterClass Xnet ;

enum modes
{
    TEACHIN,
    NORMAL,
} mode = NORMAL ;

uint16_t lastAddress ;


/************* SETUP *************/
void setup()
{
    initIO() ;

    Xnet.setup( Loco28, rs485dir ) ;    
}

/********** FUNCTIONS *************/

void notifyXNetTrnt( uint16_t address, uint8_t Pos )				            // ONLY WORKS IF OTHER DEVICES DOES SOMETHIMG
{
    //address ++ ;                                                              // NEEDED?
    Pos &= 0b11 ;
    if( Pos >= 2) Pos -= 2 ; // convers 1-2 into 0-1

    lastAddress  = address & 0x07FF ;                                           // NOTE it maybe that this function is called twice for every set point
    lastAddress |= Pos << 15 ;                                                  // in that case more code is needed to compensate
}

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
    for (int i = 0; i < nInputs; i++)
    {
        uint8_t btnState = input[i].readInput() ;

        if( btnState == FALLING || btnState == RISING)
        {
            if( mode == NORMAL)                                                 // set point
            {
                uint16_t address = loadPoint( i ) ;
                
                uint8_t state = address > 15 ;
                address &= 0x07FF ;
                
                if( btnState == FALLING ) state ^= 1 ;

                Xnet.SetTrntPos( address, state, 1) ;
                // delay(20) ;
                // Xnet.SetTrntPos( address, state, 0) ;                        // maybe not needed? should check for this
            }
            else                                                                // store point
            {   
                if( btnState == FALLING ) lastAddress ^= 0x8000 ;                  // flip the state bit if the switch is falling.
                storePoint( i, lastAddress ) ;
            }
        }
    }
}


void loop()
{
    debounce() ;
    readSwitches() ;

    if( store.readInput() == HIGH ) mode =  NORMAL ;
    else                            mode = TEACHIN ;
    

    Xnet.update() ;
}