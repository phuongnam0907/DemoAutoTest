#ifndef CCARDDETECTION_H
#define CCARDDETECTION_H

#include <QObject>
#include <mlsErrors.h>
#include <rfcapimiscellaneouscommands.h>
#include "cConvert.h"

class cCardDetection : public QObject
{
    Q_OBJECT

public:
    cCardDetection();
    quint8 cardDetection(quint8 ucAntenna, quint8 PollingSequence, quint8 TypeANo, quint8 TypeBNo, quint8 TypeCNo, quint16 systemCode, quint16 Timeout, QString *data);
};

#endif // CCARDDETECTION_H
