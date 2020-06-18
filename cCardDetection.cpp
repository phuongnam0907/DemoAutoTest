#include "cCardDetection.h"

cCardDetection::cCardDetection()
{

}

quint8 cCardDetection::cardDetection(quint8 ucAntenna, quint8 PollingSequence, quint8 TypeANo, quint8 TypeBNo, quint8 TypeCNo, quint16 systemCode, quint16 Timeout, QString *data)
{
    quint8 resultCode = FAIL;
    quint16 SystemCode;
    quint8 Status;
    quint8 Card1type;
    quint8 Card2type;
    quint8 Card1Response[128];
    quint8 Card1ResponseLen = 0;
    quint8 Card2Response[128];
    quint8 Card2ResponseLen = 0;

//    if (systemCode.size() == 2) {
        SystemCode = systemCode;
        resultCode = TRU_CardDetectionHw(ucAntenna, PollingSequence, TypeANo, TypeBNo,
                                   TypeCNo, SystemCode, Timeout, &Status, &Card1type, &Card2type,
                                   Card1Response, &Card1ResponseLen, Card2Response, &Card2ResponseLen);
//    }

    if (resultCode == SUCCESS)
    {
        QString mData = "";
        QByteArray res1, res2;

        mData = "Status: " + QString::number(Status) + "<br>";

        if (Status == 0x01 || Status == 0x02) {
            res1.resize(Card1ResponseLen);
            memcpy(res1.data(), &Card1Response, Card1ResponseLen);
            mData += "  Type 1: " + QString::number(Card1type,16).toUpper() + "<br>";
            mData += "  Response 1: " + cConvert::ArraytoString(Card1Response, Card1ResponseLen) + "<br>";
        }

        if (Status == 0x02) {
            res2.resize(Card2ResponseLen);
            memcpy(res2.data(), &Card2Response, Card2ResponseLen);
            mData += "  Type 2: " + QString::number(Card2type,16).toUpper() + "<br>";
            mData += "  Response 2: " + cConvert::ArraytoString(Card2Response, Card2ResponseLen) + "<br>";
        }
        *data = mData;
    }
    return resultCode;
}
