#ifndef CONVERT_H
#define CONVERT_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QByteArray>
#include <stdint.h>
#include <resultcode.h>

/*!
 * \brief The Convert class Contain function convert between data type
 */
class cConvert : public QObject
{
    Q_OBJECT
public:
    explicit cConvert(QObject *parent = 0);
    /*!
     * \brief ArraytoString Convert an array to string, Each element of array will separate by a space.
     * \param array Point to an array need to be convert
     * \param Length Length of Array
     * \return QString
     */
    static QString ArraytoString(uint8_t *array, uint16_t Length);
    /*!
     * \brief StringToArray Convert a String to array, Each element of array need to be separeted by a space.Ex: 02 0A 3F
     * \param Textdata Input Text String need to be converted
     * \param array point to array for store output data
     * \param arrayMaxLength Maximum length of array
     * \return true if convert OK, other wise false.
     */
    static int StringToArray(QString Textdata, uint8_t* array, uint16_t arrayMaxLength);
    /*!
     * \brief StringToNumber Convert a String to a number depend on Base 10, 16...
     * \param TextData Number in String Data.
     * \param numberOut point to a variable store data.
     * \param Base 10, 16, 8...
     * \return  true if convert OK, other wise false.
     */
    static bool StringToNumber(QString TextData, uint8_t *numberOut, uint8_t Base);
    /*!
     * \brief StringArrayToNumber Convert a big number larger than 8bits, The number stored in an array
     * \param stringarray String need to be convert. ex 02 03 0A
     * \param numberOut Number after convert.
     * \return true if convert OK, other wise false.
     */
    static bool StringArrayToNumber(QString stringarray, uint32_t *numberOut);
    /*!
     * \brief QStringToStdString Convert QString to char*
     * \param string Input String need to be converted
     * \return a pointer to string
     */
    char *QStringToStdString(QString string);
    /*!
     * \brief StringToASCIICode Convert each charactor in a string to code in ASCII Table.
     * \param text input String
     * \param array point to array stored ASCII code of each charactor
     * \param MaxArrayLength Max length of array
     * \return  true if convert OK, other wise false.
     */
    static bool StringToASCIICode(QString text, uint8_t *array, int MaxArrayLength);

    static QString ASCIICodeToString(uint8_t *array, int ArrayLength);
    /*!
     * \brief ASCIICodeToString Convert ASCII code to a string
     * \param array point to array stored ASCII Code
     * \param ArrayLength Length of array
     * \return A String type QString
     */
    static QString ASCIICodeToString(const char *array, int ArrayLength);
    /*!
     * \brief UnknowDataToASCIICode UnknowData mean charactor not on keyboard or control charactor, This function is used to convert these charactor to ASCII code
     * \param text Input String need to be converted
     * \param array point to array stored data out
     * \param MaxArrayLength Max length of array.
     * \return true if convert OK, other wise false.
     */
    static bool UnknowDataToASCIICode(QString text, uint8_t *array, uint16_t MaxArrayLength);
    /*!
     * \brief ArrayToInt Convert an array to a number (4 bytes)
     * \param array point to array, array must have equal or less then 4 elements.
     * \param arrayLength Array length
     * \return
     */
    static QString ArrayToInt(uint8_t *array, uint8_t arrayLength);
    /*!
     * \brief StringLength Get length of a string
     * \param StringArray A string contains valid data in HEX, each elements separate by space.
     * \return
     */
    static qint64 StringLength(QString StringArray);
    /*!
     * \brief QStringGenerateValidHexArrayString generate a valid hex array in text format with any input text by user.
     * - 01AA0D -> 01 AA 0D
     * - 01 AA 0D -> 01 AA 0D
     * - 01A A 0D -> 01 0A 0A 0D
     * \param inputTextArray Text to be convert
     */
    static QString GenerateValidHexArrayString(QString inputTextArray);

    static QString GetRFResultCodeString(quint8 result);
signals:

public slots:
};

#endif // CONVERT_H
