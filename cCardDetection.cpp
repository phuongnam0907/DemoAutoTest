#include "cCardDetection.h"

cCardDetection::cCardDetection()
{

}

quint8 cCardDetection::cardDetection(quint8 ucAntenna, quint8 ucPollingSequence, quint8 ucTypeANo, quint8 ucTypeBNo, quint8 ucTypeCNo, quint16 usiSystemCode, quint16 usiTimeout, QByteArray *data)
{
    quint8 cmd[11];
    quint8 resultCode = 255;
    mlsErrorCode_t  retval = MLS_ERROR;
    quint8 receiveData[sizeof(serialRspHeaderData_t)];
    quint16 lengthOut;
    quint8 RespondCode[3] = {0x8B, 0x03, 0x00};

    cmd[0] = 0x8B;
    cmd[1] = 0x02;
    cmd[2] = ucAntenna;
    cmd[3] = ucPollingSequence;
    cmd[4] = ucTypeANo;
    cmd[5] = ucTypeBNo;
    cmd[6] = ucTypeCNo;
    cmd[7] = (quint8)usiSystemCode;
    cmd[8] = (quint8)(usiSystemCode >> 8);
    cmd[9] = (quint8)usiTimeout;
    cmd[10] =(quint8)(usiTimeout >> 8);
    retval = mlsSendCmd(cmd, sizeof(cmd), SERIAL_FI_DEVICE_TO_RF_PROCESSOR, receiveData, &lengthOut);
    if (memcmp(receiveData, RespondCode , 3) == 0)
    {
        data->resize(lengthOut);
        memcpy(data->data(), &receiveData, lengthOut);
        resultCode = MLS_SUCCESS;
    }
    return resultCode;
}
