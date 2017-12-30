/* XL Direct Ltd. */

#pragma once

#include "objbase.h"

/* Must put the following in each stdafx.h
#define _WIN32_WINNT 0x400
#define _WIN32_DCOM
*/

namespace CodLib
    {
    class CCoInitialise // Ensures that COM is initialised before any smart pointers thereby avoiding the dreaded "Exception in KERNEL32.DLL : 0xE06D7363"
        {
public:
        enum ThreadingModels {ApartmentThreaded,MultiThreaded};
        CCoInitialise(enum ThreadingModels ThreadingModel=CCoInitialise::ApartmentThreaded) 
            {
            if (ThreadingModel==CCoInitialise::ApartmentThreaded)
                ::CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
            else
                ::CoInitializeEx(NULL,COINIT_MULTITHREADED);
            }
        ~CCoInitialise() {::CoUninitialize();}
        };
    }

