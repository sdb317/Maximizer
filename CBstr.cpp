#include "stdafx.h"
#include "CBstr.h"

void CodLib::CBstr::Format(const TCHAR*pszFormat,...)
    {
    // Doesn't have all the features of CString::Format()
    va_list vl;
    va_start(vl, pszFormat);

    TCHAR*pszTemp=NULL;
    int nBufferSize=0;
    int nRetVal=-1;

    do
        {
        // Increment the buffer 256 characters at a time
        // I'm not sure whether this is robust!
        nBufferSize+=256;
        delete[] pszTemp;
        pszTemp=new TCHAR[nBufferSize];
//        nRetVal=_vstprintf(pszTemp, pszFormat, vl); // The working buffer should grow until it is long enough to format the string. This is not happening since the wrong formatting function is being called.
#ifdef _UNICODE
        nRetVal=_vsnwprintf(pszTemp, nBufferSize - 1, pszFormat, vl);
#else
        nRetVal=_vsnprintf(pszTemp, nBufferSize - 1, pszFormat, vl);
#endif
        }
    while (nRetVal<0);

    *this=pszTemp;
    delete[] pszTemp;

    va_end(vl);
    return;

/*
    va_list argList;
    va_start(argList, pszFormat);
    FormatV(pszFormat, argList);
    va_end(argList);
    return;
*/
    };

#define TCHAR_ARG   TCHAR
#define WCHAR_ARG	WCHAR
#define CHAR_ARG	char

#define DOUBLE_ARG  double

#define FORCE_ANSI	   0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

void CodLib::CBstr::FormatV(LPCTSTR lpszFormat, va_list argList)
    {
    va_list argListSave=argList;

    // make a guess at the maximum length of the resulting string
    int nMaxLen=0;
    for (LPCTSTR lpsz=lpszFormat; *lpsz!='\0'; lpsz=_tcsinc(lpsz))
        {
        // handle '%' character, but watch out for '%%'
        if (*lpsz!='%' ||*(lpsz=_tcsinc(lpsz))=='%')
            {
            nMaxLen+=_tclen(lpsz);
            continue;
            }

        int nItemLen=0;

        // handle '%' character with format
        int nWidth=0;
        for ( ; *lpsz!='\0'; lpsz=_tcsinc(lpsz))
            {
            // check for valid flags
            if (*lpsz=='#') nMaxLen+=2; // for '0x'
            else if (*lpsz=='*') nWidth=va_arg(argList, int);
            else if (*lpsz=='-' ||*lpsz=='+' ||*lpsz=='0' ||*lpsz==' ');
            else // hit non-flag character
                   break;
            }
        // get width and skip it
        if (nWidth==0)
            {
            // width indicated by
            nWidth=_ttoi(lpsz);
            for ( ; *lpsz!='\0' &&_istdigit(*lpsz); lpsz=_tcsinc(lpsz));
            }
        //		ASSERT(nWidth >= 0);

        int nPrecision=0;
        if (*lpsz=='.')
            {
            // skip past '.' separator (width.precision)
            lpsz=_tcsinc(lpsz);

            // get precision and skip it
            if (*lpsz=='*')
                {
                nPrecision=va_arg(argList, int);
                lpsz=_tcsinc(lpsz);
                }
            else
                {
                nPrecision=_ttoi(lpsz);
                for ( ; *lpsz!='\0' &&_istdigit(*lpsz); lpsz=_tcsinc(lpsz));
                }
            //			ASSERT(nPrecision >= 0);
            }

        // should be on type modifier or specifier
        int nModifier=0;
        if (_tcsncmp(lpsz, _T("I64"), 3)==0)
            {
            lpsz+=3;
            nModifier=FORCE_INT64;
            //#if !defined(_X86_) && !defined(_ALPHA_)
            //			// __int64 is only available on X86 and ALPHA platforms
            //			ASSERT(FALSE);
            //#endif
            }
        else
            {
            switch (*lpsz)
                {
                // modifiers that affect size
                case 'h':
                    nModifier=FORCE_ANSI;
                    lpsz=_tcsinc(lpsz);
                    break;
                case 'l':
                    nModifier=FORCE_UNICODE;
                    lpsz=_tcsinc(lpsz);
                    break;

                    // modifiers that do not affect size
                case 'F':
                case 'N':
                case 'L':
                    lpsz=_tcsinc(lpsz);
                    break;
                }
            }

        // now should be on specifier
        switch (*lpsz|nModifier)
            {
            // single characters
            case 'c':
            case 'C':
                nItemLen=2;
                va_arg(argList, TCHAR_ARG);
                break;
            case 'c' |FORCE_ANSI:
            case 'C' |FORCE_ANSI:
                nItemLen=2;
                va_arg(argList, CHAR_ARG);
                break;
            case 'c' |FORCE_UNICODE:
            case 'C' |FORCE_UNICODE:
                nItemLen=2;
                va_arg(argList, WCHAR_ARG);
                break;

                // strings
            case 's':
                    {
                    LPCTSTR pstrNextArg=va_arg(argList, LPCTSTR);
                    if (pstrNextArg==NULL) nItemLen=6; // "(null)"
                    else
                        {
                        nItemLen=lstrlen(pstrNextArg);
                        nItemLen=max(1, nItemLen);
                        }
                    }
                break;

            case 'S':
                    {
#ifndef _UNICODE
                    LPWSTR pstrNextArg=va_arg(argList, LPWSTR);
                    if (pstrNextArg==NULL) nItemLen=6; // "(null)"
                    else
                        {
                        nItemLen=wcslen(pstrNextArg);
                        nItemLen=max(1, nItemLen);
                        }
#else
                    LPCSTR pstrNextArg=va_arg(argList, LPCSTR);
                    if (pstrNextArg==NULL) nItemLen=6; // "(null)"
                    else
                        {
                        nItemLen=lstrlenA(pstrNextArg);
                        nItemLen=max(1, nItemLen);
                        }
#endif
                    }
                break;

            case 's' |FORCE_ANSI:
            case 'S' |FORCE_ANSI:
                    {
                    LPCSTR pstrNextArg=va_arg(argList, LPCSTR);
                    if (pstrNextArg==NULL) nItemLen=6; // "(null)"
                    else
                        {
                        nItemLen=lstrlenA(pstrNextArg);
                        nItemLen=max(1, nItemLen);
                        }
                    }
                break;

            case 's' |FORCE_UNICODE:
            case 'S' |FORCE_UNICODE:
                    {
                    LPWSTR pstrNextArg=va_arg(argList, LPWSTR);
                    if (pstrNextArg==NULL) nItemLen=6; // "(null)"
                    else
                        {
                        nItemLen=wcslen(pstrNextArg);
                        nItemLen=max(1, nItemLen);
                        }
                    }
                break;
            }

        // adjust nItemLen for strings
        if (nItemLen!=0)
            {
            if (nPrecision!=0) nItemLen=min(nItemLen, nPrecision);
            nItemLen=max(nItemLen, nWidth);
            }
        else
            {
            switch (*lpsz)
                {
                // integers
                case 'd':
                case 'i':
                case 'u':
                case 'x':
                case 'X':
                case 'o':
                    if (nModifier&FORCE_INT64) va_arg(argList, __int64);
                    else va_arg(argList, int);
                    nItemLen=32;
                    nItemLen=max(nItemLen, nWidth+nPrecision);
                    break;

                case 'e':
                case 'g':
                case 'G':
                    va_arg(argList, DOUBLE_ARG);
                    nItemLen=128;
                    nItemLen=max(nItemLen, nWidth+nPrecision);
                    break;

                case 'f':
                    va_arg(argList, DOUBLE_ARG);
                    nItemLen=128; // width isn't truncated
                    // 312 == strlen("-1+(309 zeroes).")
                    // 309 zeroes == max precision of a double
                    nItemLen=max(nItemLen, 312+nPrecision);
                    break;

                case 'p':
                    va_arg(argList, void*);
                    nItemLen=32;
                    nItemLen=max(nItemLen, nWidth+nPrecision);
                    break;

                    // no output
                case 'n':
                    va_arg(argList, int*);
                    break;

                default:
                    //				ASSERT(FALSE);	// unknown formatting option
                    return;
                }
            }

        // adjust nMaxLen for output nItemLen
        nMaxLen+=nItemLen;
        }
    TCHAR*pBuffer=new TCHAR[nMaxLen];
    _vstprintf(pBuffer, lpszFormat, argListSave);

    *this=pBuffer;
    if (pBuffer) delete[] pBuffer;

    va_end(argListSave);
    }

/*
int CodLib::CBstr::Replace(const wint_t&chFind, const wint_t&chReplace)
    {
    const wchar_t*pstr=_bstr_t::operator const wchar_t*();
    if (pstr==NULL)
        return 0;
    if (chFind==chReplace)
        return 0;
    // Make a copy of the string to avoid reference counting problems.
    _bstr_t bstrNew(*this, true);
    pstr=(wchar_t*) bstrNew;
    int nCount=0;
    wchar_t*pSearchPos=0;
    while (pSearchPos=wcschr(pSearchPos?pSearchPos:pstr, chFind))
        {
        *pSearchPos=chReplace;
        ++nCount;
        }
    // Assign the new string (avoids refcount and deletes cached char-string)
    *this=bstrNew;
    return nCount;
    }
*/

int CodLib::CBstr::Replace(const _bstr_t&bstrFind, const _bstr_t&bstrReplace)
    {
    const wchar_t*pSrcStr=_bstr_t::operator const wchar_t*();
    if (pSrcStr==NULL)
        return 0;
    int nCount=0;
    int nSearchStrLen=bstrFind.length();
    // Calculate new length
    wchar_t*pSearchPos=0;
	while (pSearchPos=(wchar_t*)wcsstr(pSearchPos ? pSearchPos : pSrcStr, bstrFind))
        {
        pSearchPos+=nSearchStrLen;
        ++nCount;
        }
    // Replace bstrFind substrings with bstrReplace substrings
    if (nCount)
        {
        int nReplaceStrLen=bstrReplace.length();
        int nOrgStrLen=length();
        int nNewStrLength=nOrgStrLen-(nSearchStrLen-nReplaceStrLen)*nCount;
        nCount=0;
        wchar_t*pNewStr=SysAllocStringLen(0, nNewStrLength);
        wchar_t*pSrcPos=(wchar_t*) pSrcStr;
        wchar_t*pDestPos=pNewStr;
        wchar_t*pNextSrcPos=0;
        while (pNextSrcPos=wcsstr(pSrcPos, bstrFind))
            {
            if (pSrcPos!=pNextSrcPos)
                {
                int nCopyLength=pNextSrcPos-pSrcPos;
                wcsncpy(pDestPos, pSrcPos, nCopyLength);
                pDestPos+=nCopyLength;
                pSrcPos=pNextSrcPos;
                }
            wcsncpy(pDestPos, bstrReplace, nReplaceStrLen);
            pDestPos+=nReplaceStrLen;
            pSrcPos+=nSearchStrLen;
            ++nCount;
            }
        wcsncpy(pDestPos, pSrcPos, nOrgStrLen-(pSrcPos-pSrcStr));
        *this=bstr_t(pNewStr, false);
        }
    return nCount;
    }
