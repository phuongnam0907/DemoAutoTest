#include "cExportClass.h"

cExportClass::cExportClass()
{

}

void cExportClass::setX(float x)
{
    mX = x;
}

void cExportClass::setY(float y)
{
    mY = y;
}

void cExportClass::setZ(float z)
{
    mZ = z;
}

void cExportClass::setResult(bool result)
{
    mResult = result;
}

float cExportClass::getX()
{
    return mX;
}

float cExportClass::getY()
{
    return mY;
}

float cExportClass::getZ()
{
    return mZ;
}

bool cExportClass::getResult()
{
    return mResult;
}