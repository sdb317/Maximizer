////////////////////////////////////////////////////////
//	
//	CBstr : BSTR wrapper class with manipulation routines
//
//	Gary Olliffe : Parallax Solutions Ltd
//
////////////////////////////////////////////////////////
//
// The public interface to this class was
// "inspired by"/"poached from" the NoMFC::CString example
// www.worldofatl.com.  As a result it fairly closely mimics the
// MFC CString interface -  the methods not implemented have been left
// in the class definition but are commented out.
// Additional constructors to mimic _bstr_t have been added, these include a
// _variant_t constructor and a BSTR constructor with copy/attach flag
//
// If you have a string that will require manipulation then use this
// otherwise stick to the basic _bstr_t class.
//
// The reason I have written this _bstr_t based class rather than use the
// basic_string implementation is that the MS STL requires a support dll
// to be shipped to support the allocators, and not everyone want to use STL
//
// This is by no means intended as an optimised implementation of this class,
// most or the routines use the standard "c" string manipulation routines
//
// Users looking at the implementation should also note that the _bstr_t base class
// has two internal representations (one ANSI, one UNICODE) methods on this
// class that modify the string (eg. MakeUpper, SetAt, etc...) modify both internal
// strings to maintain consistency.
//
// Users should also note that where appropriate the class makes use of the TCHAR
// typedef, giving UNICODE compatibility.
//
/////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>

#include <comdef.h>

#include <tchar.h>

#include "CDate.h"

namespace CodLib
{
class CBstr : public _bstr_t
    {
    // These methods groupings and names have been taken from
    // NoMFC::CString from www.worldofatl.com
public:
    // Constructors
    CBstr();
    CBstr(const CBstr&strInput);
    CBstr(const _bstr_t&strInput);
    CBstr(const TCHAR*strInput);
    // _bstr_t mimic constructors
    CBstr(const _variant_t&varinput);
    CBstr(BSTR strinput, bool bcopy);
    CBstr(BSTR strinput);
    CBstr(const long Value);
    CBstr(const double Value);
    CBstr(CDate Value);

    //      CBstr(TCHAR ch, int nRepeat = 1);

    //string population
    bool LoadString(HINSTANCE hInstance, UINT uID);

    // The string as an array
    //      int GetLength() const;
    bool IsEmpty() const;
    void Empty();
    TCHAR GetAt(int nIndex) const;
    void SetAt(int nIndex, TCHAR ch);

    // Comparison
    int Compare(const TCHAR*psz); //const;
    int Compare(const CBstr&str) const;
    int CompareNoCase(const TCHAR*psz) const;
    int CompareNoCase(const CBstr&str) const;
    int Collate(const TCHAR*psz) const;
    int Collate(const CBstr&str) const;

    // Extraction
    //      CBstr Mid(int nFirst) const;
    CBstr Mid(int nFirst, int nCount) const;
    CBstr Left(int nCount) const;
    CBstr Right(int nCount) const;
    CBstr SpanIncluding(const TCHAR*pszCharSet) const;
    CBstr SpanExcluding(const TCHAR*pszCharSet) const;

    // Other Conversions
    void MakeUpper();
    void MakeLower();
    void MakeReverse();
    void TrimLeft();
    void TrimRight();
    void Format(const TCHAR*pszFormat,...);
    void FormatV(LPCTSTR lpszFormat, va_list argList);
//    int Replace(const wint_t& chFind, const wint_t& chReplace); // Can this use LPCTSTR?
    int Replace(const _bstr_t& bstrFind, const _bstr_t& bstrReplace);
    // Searching
    int Find(const TCHAR&ch) const;
    int Find(const TCHAR*psz) const;
    int ReverseFind(const TCHAR&ch) const;
    int FindOneOf(const TCHAR*pszCharSet) const;

    // Operators
    operator const _bstr_t() const;
    operator const TCHAR*() const;
    operator const long() const;
    operator const double() const;
    operator CDate() const;
    TCHAR operator[] (int nIndex) const;

    // Workarounds
    static BSTR ConvertStringToBSTR(const char* pSrc);
    static char* ConvertBSTRToString(BSTR pSrc);
    };
}

///////////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////////

inline CodLib::CBstr::CBstr():_bstr_t() { }

inline CodLib::CBstr::CBstr(const CodLib::CBstr&strInput):_bstr_t(strInput) { }

inline CodLib::CBstr::CBstr(const _bstr_t&strInput):_bstr_t(strInput) { }

inline CodLib::CBstr::CBstr(const TCHAR*strInput):_bstr_t(strInput) { }

inline CodLib::CBstr::CBstr(const variant_t&varinput):_bstr_t(varinput) { }

inline CodLib::CBstr::CBstr(const BSTR strInput, bool bcopy):_bstr_t(strInput, bcopy) { }

inline CodLib::CBstr::CBstr(const BSTR strInput):_bstr_t(strInput) { }

////////////////////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////////////////////

// New Loadstring method
inline bool CodLib::CBstr::LoadString(HINSTANCE hInstance, UINT uID)
    {
    bool bRetval=false;
    TCHAR lpBuffer[256];
    if (0!=::LoadString(hInstance, uID, lpBuffer, 256))
        {
        bRetval=true;
        // assign value to this class
        *this=lpBuffer;
        }
    return bRetval;
    }

inline bool CodLib::CBstr::IsEmpty() const
    {
    return _bstr_t::length()==0;
    };

inline void CodLib::CBstr::Empty()
    {
    _bstr_t::operator=(_T(""));
    return;
    };

inline TCHAR CodLib::CBstr::GetAt(int nIndex) const
    {
    unsigned int len=_bstr_t::length();
    if (len>0&&len>static_cast<unsigned int>(nIndex))
        {
        const TCHAR*tmp=_bstr_t::operator const TCHAR*();
        return tmp[nIndex];
        }
    return L'\0';
    };

inline void CodLib::CBstr::SetAt(int nIndex, TCHAR ch)
    {
    unsigned int len=_bstr_t::length();
    if (len>0&&len>static_cast<unsigned int>(nIndex))
        {
        // We need to modify both internal representations in the
        // base class
        char*tmp=_bstr_t::operator char*();
        wchar_t*tmpw=_bstr_t::operator wchar_t*();

#if defined(UNICODE) && defined(_UNICODE)
        // This is a UNICODE build
        wchar_t wch=ch;
        char mbch;
        WideCharToMultiByte(CP_ACP, 0, &ch, 1, &mbch, 1, NULL, NULL);
#else
        // This is an ANSI build
        wchar_t wch;
        char mbch=ch;
        MultiByteToWideChar(CP_ACP, 0, &ch, 1, &wch, 1);
#endif

        tmp[nIndex]=mbch;
        tmpw[nIndex]=wch;
        }
    return;
    };

///////////////////////////////////////////////////////////////////////////
// Comparison
///////////////////////////////////////////////////////////////////////////

inline int CodLib::CBstr::Compare(LPCTSTR psz) //const
    {
    const TCHAR*tmp=_bstr_t::operator TCHAR*();
    return _tcscmp(tmp, psz);
    };

inline int CodLib::CBstr::Compare(const CodLib::CBstr&str) const
    {
    const TCHAR*tmpthis=_bstr_t::operator const TCHAR*();
    const TCHAR*tmpOther=static_cast<const TCHAR*>(str);

    return _tcscmp(tmpthis, tmpOther);
    };

inline int CodLib::CBstr::CompareNoCase(const TCHAR*psz) const
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
    return _tcsicmp(tmp, psz);
    };

inline int CodLib::CBstr::CompareNoCase(const CodLib::CBstr&str) const
    {
    const TCHAR*tmpthis=_bstr_t::operator const TCHAR*();
    const TCHAR*tmpOther=static_cast<const TCHAR*>(str);

    return _tcsicmp(tmpthis, tmpOther);
    };

inline int CodLib::CBstr::Collate(const TCHAR*psz) const
    {
    const TCHAR*tmpthis=_bstr_t::operator const TCHAR*();

    return _tcscoll(tmpthis, psz);
    };

inline int CodLib::CBstr::Collate(const CodLib::CBstr&str) const
    {
    const TCHAR*tmpthis=_bstr_t::operator const TCHAR*();
    const TCHAR*tmpOther=static_cast<const TCHAR*>(str);

    return _tcscoll(tmpthis, tmpOther);
    };

///////////////////////////////////////////////////////////////////////////
// Extraction
///////////////////////////////////////////////////////////////////////////

/*   inline CodLib::CBstr<T> CodLib::CBstr::Mid(int nFirst) const
   {
      const TCHAR* tmp = _bstr_t::operator TCHAR*();
      return _tcsnstr(nFirst);
   };
*/

inline CodLib::CBstr CodLib::CBstr::Mid(int nFirst, int nCount) const
    {
    TCHAR*tmp=_bstr_t::operator TCHAR*();
    int len=_bstr_t::length()+1;
    TCHAR*result=new TCHAR[len];

    memset(result, '\0', len*sizeof(TCHAR));
    _tcsncpy(result, &tmp[nFirst], nCount);

    CodLib::CBstr cbRes=result;
    delete result;

    return cbRes;
    };

inline CodLib::CBstr CodLib::CBstr::Left(int nCount) const
    {
    return Mid(0, nCount);
    };

inline CodLib::CBstr CodLib::CBstr::Right(int nCount) const
    {
    return Mid(length()-nCount, nCount);
    };

inline CodLib::CBstr CodLib::CBstr::SpanIncluding(const TCHAR*pszCharSet) const
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
    int strlen=_tcsspn(tmp, pszCharSet);

    return Mid(0, strlen);
    };

inline CodLib::CBstr CodLib::CBstr::SpanExcluding(const TCHAR*pszCharSet) const
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
    int pos=_tcscspn(tmp, pszCharSet);

    return Mid(0, pos);
    };

///////////////////////////////////////////////////////////////////////////
// Other Conversions
///////////////////////////////////////////////////////////////////////////

inline void CodLib::CBstr::MakeUpper()
    {
    // We need to modify both internal representations in the
    // base class
    char*tmp=_bstr_t::operator char*();
    wchar_t*tmpw=_bstr_t::operator wchar_t*();
    strupr(tmp);
    _wcsupr(tmpw);
    return;
    };

inline void CodLib::CBstr::MakeLower()
    {
    // We need to modify both internal representations in the
    // base class
    char*tmp=_bstr_t::operator char*();
    wchar_t*tmpw=_bstr_t::operator wchar_t*();
    strlwr(tmp);
    _wcslwr(tmpw);
    return;
    };

inline void CodLib::CBstr::MakeReverse()
    {
    // We need to modify both internal representations in the
    // base class
    char*tmp=_bstr_t::operator char*();
    wchar_t*tmpw=_bstr_t::operator wchar_t*();
    strrev(tmp);
    _wcsrev(tmpw);

    return;
    };

inline void CodLib::CBstr::TrimLeft()
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
    TCHAR*trimstr=_tcsspnp(tmp, _T(" \t"));

    *this=trimstr;
    return;
    };

inline void CodLib::CBstr::TrimRight()
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
    TCHAR*trimstr=_tcsspnp(tmp, _T(" \t"));
    int pos=_bstr_t::length()-1;
    while (trimstr[pos]==' ')
        {
        trimstr[pos--]='\0';
        }

    *this=trimstr;
    return;
    };

///////////////////////////////////////////////////////////////////////////
// Searching
///////////////////////////////////////////////////////////////////////////

inline int CodLib::CBstr::Find(const TCHAR&ch) const
    {
    // npos == -1 in this implementation, but it might not always be, so this is
    // a bit dodgy.
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
	TCHAR*pdest=(_TCHAR*)_tcschr(tmp, ch);
    return ((pdest==NULL)?NULL:(pdest-tmp));
    };

inline int CodLib::CBstr::Find(const TCHAR*psz) const
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
	TCHAR*pdest=(_TCHAR*)_tcsstr(tmp, psz);
    return ((pdest==NULL)?NULL:(pdest-tmp));
    };

inline int CodLib::CBstr::ReverseFind(const TCHAR&ch) const
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
	TCHAR*pdest=(_TCHAR*)_tcsrchr(tmp, ch);
    return ((pdest==NULL)?NULL:(pdest-tmp));
    };

inline int CodLib::CBstr::FindOneOf(const TCHAR*psz) const
    {
    const TCHAR*tmp=_bstr_t::operator const TCHAR*();
    int pos=_tcscspn(tmp, psz);
    if (pos==static_cast<long>(_bstr_t::length()))
        {
        pos=-1;
        }
    return pos;
    };

///////////////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////////////
inline CodLib::CBstr::operator const _bstr_t() const { return (_bstr_t)(*this); };

inline CodLib::CBstr::operator const TCHAR*() const { return _bstr_t::operator const TCHAR*(); };

inline TCHAR CodLib::CBstr::operator[] (int nIndex) const { return GetAt(nIndex); };

///////////////////////////////////////////////////////////////////////////
// Comparison Operators
///////////////////////////////////////////////////////////////////////////
inline bool operator==(const CodLib::CBstr&s1, const CodLib::CBstr&s2) { return s1.Compare(s2)==0; }

inline bool operator==(const CodLib::CBstr&s1, const TCHAR*s2) { return s1.Compare(s2)==0; }

inline bool operator==(const TCHAR*s1, const CodLib::CBstr&s2) { return s2.Compare(s1)==0; }

inline bool operator!=(const CodLib::CBstr&s1, const CodLib::CBstr&s2) { return s1.Compare(s2)!=0; }

inline bool operator!=(const CodLib::CBstr&s1, const TCHAR*s2) { return s1.Compare(s2)!=0; }

inline bool operator!=(const TCHAR*s1, const CodLib::CBstr&s2) { return s2.Compare(s1)!=0; }

inline bool operator<(const CodLib::CBstr&s1, const CodLib::CBstr&s2) { return s1.Compare(s2)<0; }

inline bool operator<(const CodLib::CBstr&s1, const TCHAR*s2) { return s1.Compare(s2)<0; }

inline bool operator<(const TCHAR*s1, const CodLib::CBstr&s2) { return s2.Compare(s1)>0; }

inline bool operator>(const CodLib::CBstr&s1, const CodLib::CBstr&s2) { return s1.Compare(s2)>0; }

inline bool operator>(const CodLib::CBstr&s1, const TCHAR*s2) { return s1.Compare(s2)>0; }

inline bool operator>(const TCHAR*s1, const CodLib::CBstr&s2) { return s2.Compare(s1)<0; }

inline bool operator<=(const CodLib::CBstr&s1, const CodLib::CBstr&s2) { return s1.Compare(s2)<=0; }

inline bool operator<=(const CodLib::CBstr&s1, const TCHAR*s2) { return s1.Compare(s2)<=0; }

inline bool operator<=(const TCHAR*s1, const CodLib::CBstr&s2) { return s2.Compare(s1)>=0; }

inline bool operator>=(const CodLib::CBstr&s1, const CodLib::CBstr&s2) { return s1.Compare(s2)>=0; }

inline bool operator>=(const CodLib::CBstr&s1, const TCHAR*s2) { return s1.Compare(s2)>=0; }

inline bool operator>=(const TCHAR*s1, const CodLib::CBstr&s2) { return s2.Compare(s1)<=0; }

///////////////////////////////////////////////////////////////////////////
// Unusual Conversions
///////////////////////////////////////////////////////////////////////////

inline CodLib::CBstr::CBstr(const long Value)
    {
    BSTR str=NULL;
    VarBstrFromI4(Value,0,0,&str);
    (*this)=CBstr(str,true);;
    };

inline CodLib::CBstr::CBstr(const double Value)
    {
    BSTR str=NULL;
    VarBstrFromR8(Value,0,0,&str);
    (*this)=CBstr(str,true);;
    };

inline CodLib::CBstr::CBstr(CodLib::CDate Value)
    {
    BSTR str=NULL;
    VarBstrFromDate((DATE)Value,0,0,&str);
    (*this)=CBstr(str,true);;
    };

inline CodLib::CBstr::operator const long() const 
    {
    long Value=0;
    VarI4FromStr((*this),0,0,&Value);
    return Value;
    };

inline CodLib::CBstr::operator const double() const 
    {
    double Value=0;
    VarR8FromStr((*this),0,0,&Value);
    return Value;
    };

inline CodLib::CBstr::operator CodLib::CDate() const 
    {
    DATE Value=0;
    VarDateFromStr((*this),0,0,&Value);
    return CodLib::CDate(Value);
    };


///////////////////////////////////////////////////////////////////////////
// Workarounds for 'stack overflow' bugs. See: http://www.codeproject.com/KB/bugs/bug_in__bstr_t_amp__varia.aspx
///////////////////////////////////////////////////////////////////////////

//------------------------//

// Convert char * to BSTR //

//------------------------//

inline BSTR CodLib::CBstr::ConvertStringToBSTR(const char* pSrc)
{
    if (!pSrc) return NULL;
    DWORD cwch;
    BSTR wsOut(NULL);
    if (cwch = ::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0)) // get size minus NULL terminator
        {
        cwch--;
        wsOut = ::SysAllocStringLen(NULL, cwch);
        if (wsOut)
            {
            if (!::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, wsOut, cwch))
                {
                if (ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
                    return wsOut;
                ::SysFreeString(wsOut);// must clean up
                wsOut = NULL;
                }
            }
        }
    return wsOut;
}

//------------------------//

// Convert BSTR to char * //

//------------------------//

inline char* CodLib::CBstr::ConvertBSTRToString(BSTR pSrc)
{
    if (!pSrc) return NULL;
    DWORD cb,cwch = ::SysStringLen(pSrc); // convert even embeded NULL
    char *szOut = NULL;
    if (cb = ::WideCharToMultiByte(CP_ACP, 0, pSrc, cwch + 1, NULL, 0, 0, 0))
        {
        szOut = new char[cb];
        if (szOut)
            {
            szOut[cb - 1]  = '\0';
            if (!::WideCharToMultiByte(CP_ACP, 0, pSrc, cwch + 1, szOut, cb, 0, 0))
                {
                delete []szOut;// clean up if failed;
                szOut = NULL;
                }
            }
        }
    return szOut;
}

