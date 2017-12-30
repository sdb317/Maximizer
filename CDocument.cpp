/* XL Direct Ltd. */

#include "stdafx.h"
#include <string.h>
#include "CDocument.h"
#include "CNode.h"
#include "CNodeList.h"
#include "CBstr.h"

CodLib::CDocument::CDocument(bool FreeThreaded)
    {
    pDocument=NULL;
    this->FreeThreaded=FreeThreaded;
    if (FreeThreaded)
        {
        HRESULT hResult=pDocument.CreateInstance(__uuidof(MSXML::FreeThreadedDOMDocument60));
        if (FAILED(hResult))
            _com_raise_error(hResult);
        }
    else
        {
        HRESULT hResult=pDocument.CreateInstance(__uuidof(MSXML::DOMDocument60));
        if (FAILED(hResult))
            _com_raise_error(hResult);
        }
    SetDefaultProperties();
    }

CodLib::CDocument::CDocument(CDocumentPtr pDocument)
    {
    this->pDocument=pDocument;
    SetDefaultProperties();
    }

void CodLib::CDocument::operator=(CDocumentPtr pDocument)
    {
    this->pDocument=pDocument;
    SetDefaultProperties();
    }

CodLib::CDocument::~CDocument()
    {
    pDocument=NULL;
    }

void CodLib::CDocument::SetDefaultProperties()
    {
    if (pDocument!=NULL)
        {
        pDocument->async=VARIANT_FALSE;
        pDocument->validateOnParse=VARIANT_FALSE;
        pDocument->preserveWhiteSpace=VARIANT_FALSE;
        pDocument->resolveExternals=VARIANT_FALSE;
        pDocument->setProperty(_bstr_t(_T("SelectionLanguage")),_variant_t(_T("XPath")));
        pDocument->setProperty(_bstr_t(_T("NewParser")),VARIANT_TRUE); // Until this is the default
        pDocument->setProperty(_bstr_t(_T("SelectionNamespaces")),_variant_t(_T("xmlns:XLd=\"http://www.xl-direct.com/\"")));
        }
    }

bool CodLib::CDocument::LoadDocument(const _bstr_t& DocumentName)
    {
    try
        {
        VARIANT_BOOL isSuccessful;
        if ((_tcsstr((LPCTSTR)DocumentName,_T("<")))&&(_tcsstr((LPCTSTR)DocumentName,_T(">"))))
            isSuccessful=pDocument->loadXML(DocumentName); // Must be fragment.
        else
            isSuccessful=pDocument->load(_variant_t(DocumentName)); // Must be file.
        if (isSuccessful==VARIANT_TRUE)
            return true;
        else
            {
            MSXML::IXMLDOMParseErrorPtr pParseError=NULL;
            pParseError=pDocument->parseError;
            if (pParseError)
                {
                long filepos=pParseError->filepos;
                long line=pParseError->line;
                long linepos=pParseError->linepos;
                _bstr_t Error=pParseError->reason+L"\n"+pParseError->srcText;
                }
            }
        }
    catch(_com_error & e)
        {
        _bstr_t Error=e.Source()+L"\n"+e.Description();
        }
    catch (...)
        {
        }
    return false;
    }

bool CodLib::CDocument::SaveDocument(const _bstr_t& DocumentName)
    {
    try
        {
        pDocument->save(_variant_t(DocumentName));
        }
    catch(_com_error & e)
        {
        _bstr_t Error=e.Source()+L"\n"+e.Description();
        }
    catch (...)
        {
        }
    return true;
    }

CodLib::CNode CodLib::CDocument::FindNode(_bstr_t Path)
{
    try
        {
        if (this->pDocument!=NULL)
            {
            if (this->pDocument->documentElement!=NULL)
                {
                return CodLib::CNode(this->pDocument->documentElement->selectSingleNode(Path));
                }
            }
        }
    catch(_com_error & e)
        {
        _bstr_t Error=e.Source()+L"\n"+e.Description();
        }
    catch (...)
        {
        }
    return CodLib::CNode(NULL);
}

CodLib::CNodeList CodLib::CDocument::FindNodes(_bstr_t Path)
{
    try
        {
        if (this->pDocument!=NULL)
            {
            if (this->pDocument->documentElement!=NULL)
                {
                return CodLib::CNodeList(this->pDocument->documentElement->selectNodes(Path));
                }
            }
        }
    catch(_com_error & e)
        {
        _bstr_t Error=e.Source()+L"\n"+e.Description();
        }
    catch (...)
        {
        }
    return CodLib::CNodeList(NULL);
}

