#ifndef _CDATE_H_D130E080_1B3E_11d2_955A_00805FC3DFB4
#define _CDATE_H_D130E080_1B3E_11d2_955A_00805FC3DFB4

#include <stdio.h>

#include <comdef.h>

#include <tchar.h>

namespace CodLib
{
class CDate
    {
public:
    CDate();
    CDate(DATE Value);
    void operator=(DATE Value);
    operator DATE();
    double* operator&();
private:
    DATE Date;
    };

}

inline CodLib::CDate::CDate() {};

inline CodLib::CDate::CDate(DATE Value):Date(Value) {};

inline void CodLib::CDate::operator=(DATE Value) { Date = Value; };

inline CodLib::CDate::operator DATE() {return Date;};

inline double* CodLib::CDate::operator&() {return &Date;};

#endif // _CDATE_H_D130E080_1B3E_11d2_955A_00805FC3DFB4
