#include "cConvert.h"

cConvert::cConvert(QObject *parent) : QObject(parent)
{

}

QString cConvert::ArraytoString(uint8_t *array, uint16_t Length)
{
    QString returnValue = "";
        QString tmp;
        for (int i = 0; i < Length; i++)
        {
            tmp = QString("%1").arg(array[i], 2, 16, QChar('0'));
            tmp = tmp.toUpper();
            returnValue.append(tmp + " ");
        }
        returnValue = returnValue.trimmed();
        return returnValue;
}

int cConvert::StringToArray(QString Textdata, uint8_t *array, uint16_t arrayMaxLength)
{
    int returnValue = 0;
    QStringList TextData;
    qint8 tempData;
    bool isConverOK;
    qint16 index;
    TextData =  GenerateValidHexArrayString(Textdata).split(" ");
    index = 0;
    if (arrayMaxLength < TextData.length())
    {
        returnValue = -1;
    }
    else
    {
        returnValue = TextData.length();
        foreach (QString data, TextData)
        {
            tempData = (qint8)data.toInt(&isConverOK, 16);
            if (isConverOK)
            {
                array[index++] = tempData;
            }
            else
            {
                returnValue = -1;
                break;
            }
        }

    }
    return returnValue;
}
bool cConvert::StringToNumber(QString TextData, uint8_t *numberOut, uint8_t Base)
{
    bool isConverOK = false;
    TextData = TextData.trimmed();
    *numberOut = TextData.toInt(&isConverOK, Base);
    return isConverOK;
}

bool cConvert::StringArrayToNumber(QString stringarray, uint32_t *numberOut)
{
    bool isConvertOK = true;
        qint8 shiftVal = 0;
        qint32 TempData;
        QStringList Array = stringarray.trimmed().split(" ");
        *numberOut = 0;
        foreach (QString data, Array)
        {
            TempData = data.toInt(&isConvertOK, 16);
            if (isConvertOK)
            {
                *numberOut = *numberOut | (TempData << (shiftVal++*8));
            }
            else
            {
                break;
            }
        }
        return isConvertOK;
}

char *cConvert::QStringToStdString(QString string)
{
    static QByteArray sArray;
    sArray = string.toLatin1();
    return sArray.data();
}

bool cConvert::StringToASCIICode(QString text, unsigned char *array, int MaxArrayLength)
{
    bool returnValue = true;
    static QByteArray asciiCode;
    if (MaxArrayLength < text.length())
        returnValue = false;
    else
    {
        asciiCode = text.toLatin1();
        memcpy(array, asciiCode, asciiCode.length());
    }
    return returnValue;
}

QString cConvert::ASCIICodeToString(uint8_t *array, int ArrayLength)
{
    QByteArray byteArray;
    byteArray = QByteArray::fromRawData((const char *)array, ArrayLength);
    for (int i = 0; i < byteArray.length(); i++)
        if (byteArray.at(i) < 33)
            byteArray[i] = 46;
    QString StringData = QString::fromLatin1(byteArray);
    return StringData;
}

QString cConvert::ASCIICodeToString(const char *array, int ArrayLength)
{
    QByteArray byteArray;
    byteArray = QByteArray::fromRawData(array, ArrayLength);
    for (int i = 0; i < byteArray.length(); i++)
        if (byteArray.at(i) < 33)
            byteArray[i] = 46;
    QString StringData = QString::fromLatin1(byteArray);
    return StringData;
}

bool cConvert::UnknowDataToASCIICode(QString text, unsigned char *array, uint16_t MaxArrayLength)
{
    bool returnval = true;
    int length;
    length = StringToArray(text, array, MaxArrayLength);
    if (length == -1)
        returnval = StringToASCIICode(text, array, MaxArrayLength);
    return returnval;
}

QString cConvert::ArrayToInt(uint8_t *array, uint8_t arrayLength)
{
    uint32_t tmpData = 0;
    memcpy(&tmpData, array, arrayLength);
    return QString::number(tmpData, 10);
}

qint64 cConvert::StringLength(QString StringArray)
{
    QStringList StringList;
    QString tmpString = StringArray.trimmed();
    StringList = tmpString.split(" ");
    return StringList.length();
}
static QString splitHexByte(QString text)
{
    QString tmpString;
    for (int i = 0; i < text.length(); i++)
    {
        if (i%2 != 0)
            tmpString = tmpString + text.at(i) + " ";
        else
            tmpString += text.at(i);
    }
    tmpString = tmpString.trimmed();
    return tmpString;
}

static QString standardizedString(QString Text)
{
    QString tmp;
    tmp = Text.trimmed();
    while (tmp.contains("\n"))
    {
        tmp = tmp.replace("\n", " ");
    }
    while (tmp.contains("\t"))
    {
        tmp = tmp.replace("\t", " ");
    }
    while (tmp.contains("  "))
    {
        tmp = tmp.replace("  ", " ");
    }

    return tmp;
}
QString cConvert::GenerateValidHexArrayString(QString inputTextArray)
{
    QStringList tmpPart;
    QString TmpOut = "";
    inputTextArray = standardizedString(inputTextArray);
    tmpPart = inputTextArray.split(" ");
    foreach (QString txt, tmpPart)
    {
        TmpOut += splitHexByte(txt) + " ";
    }
    TmpOut = TmpOut.trimmed();
    return TmpOut;
}

QString cConvert::GetRFResultCodeString(quint8 result)
{
    QString strtmp = "Unknown Error!";

    switch(result)
    {
    case SUCCESS:                   strtmp = "Command executed successfully";           break;
    case FAIL:                      strtmp = "Command execution failed";                break;
    case INVALID_VALUE:             strtmp = "Invalid value of a command parameter";    break;
    case MULTIPLE_CARD_DETECTED:    strtmp = "Multiple card detected";                  break;
    case NO_CARD_DETECTED:          strtmp = "No card detected or card removed";        break;
    case INITIALIZING_ERROR:        strtmp = "Error initializing the card";             break;
    case OPERATION_ERROR:           strtmp = "Error in card operation (on the card)";   break;
    case CARD_ERROR:                strtmp = "An error occurred on the card";           break;
    case OPERATION_INTERRUPT:       strtmp = "Card operation interrupted";              break;
    case READER_ERROR:              strtmp = "Error occurred in the reader RF receiver";break;
    case CRC_ERROR:                 strtmp = "Card response message CRC check error";   break;
    case NO_RESPONSE:               strtmp = "No response from card";                   break;
    case COMMAND_NOT_SUPPORTED:     strtmp = "Command not supported";                   break;
    case READER_OPERATION_TIMEOUT:  strtmp = "Reader operation timeout";                break;
    case ANTENNA_ID_DUPLICATE:      strtmp = "Antenna ID is duplicated";                break;
    case SYNTAX_ERROR:              strtmp = "Command or Card syntax error, including error in card in CRC";
        break;
    default:
        break;
    }

    strtmp += " (0x" + QString("%1").arg(result, 2, 16, QChar('0')).toUpper() + ")";

    return strtmp;
}
