#ifndef CRFCONTROLOL_H
#define CRFCONTROLOL_H

#include <QObject>
#include <rfcapirfcontrol.h>

class cRFControl : public QObject
{
    Q_OBJECT

public:
    cRFControl();

    void rfReset();

private:
    quint8 rfOn();
    quint8 rfOff();
};

#endif // CRFCONTROLOL_H
