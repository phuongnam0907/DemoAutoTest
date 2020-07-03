#ifndef CEXPORTCLASS_H
#define CEXPORTCLASS_H


class cExportClass
{
public:
    cExportClass();
    ~cExportClass();

    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void setResult(bool result);
    
    float getX();
    float getY();
    float getZ();
    bool getResult();

private:
    float mX;
    float mY;
    float mZ;
    bool mResult;
};

#endif // CEXPORTCLASS_H