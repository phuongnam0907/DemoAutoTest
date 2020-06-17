#ifndef CCARDDETECTION_H
#define CCARDDETECTION_H

#include <QObject>
#include <mlsErrors.h>
#include <mlsCmdParser.h>

class cCardDetection : public QObject
{
    Q_OBJECT

public:
    cCardDetection();
    quint8 cardDetection(quint8 ucAntenna, quint8 ucPollingSequence, quint8 ucTypeANo, quint8 ucTypeBNo, quint8 ucTypeCNo, quint16 usiSystemCode, quint16 usiTimeout, QByteArray* data);
};

#endif // CCARDDETECTION_H
