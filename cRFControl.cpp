#include "cRFControl.h"

cRFControl::cRFControl()
{

}

void cRFControl::rfReset()
{
    if (rfOff() == 0){
        _sleep(50);
        rfOn();
    }
}

quint8 cRFControl::rfOn()
{
    return TRU_RF_onoff(RF_ON);
}

quint8 cRFControl::rfOff()
{
    return TRU_RF_onoff(RF_OFF);
}
