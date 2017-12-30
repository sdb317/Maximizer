/* XL Direct Ltd. */

#include "stdafx.h"
#include "CDocumentTransform.h"

CodLib::CDocumentTransform::CDocumentTransform()
    {
    pTransform=NULL;
    HRESULT hResult=pTransform.CreateInstance(__uuidof(MSXML::XSLTemplate60));
    if (FAILED(hResult))
        _com_raise_error(hResult);
    }

CodLib::CDocumentTransform::CDocumentTransform(CDocumentTransformPtr pTransform)
    {
    this->pTransform=pTransform;
    }

void CodLib::CDocumentTransform::operator=(CDocumentTransformPtr pTransform)
    {
    this->pTransform=pTransform;
    }

CodLib::CDocumentTransform::~CDocumentTransform()
    {
    pTransform=NULL;
    }

bool CodLib::CDocumentTransform::LoadTransform(_bstr_t& TransformName)
    {
    try
        {
        CodLib::CDocument Document(true); // Needs to be free-threaded type
        if (Document.LoadDocument(TransformName))
            {
            HRESULT hResult;
            hResult=pTransform->putref_stylesheet(Document.pDocument);
            if (FAILED(hResult))
                _com_raise_error(hResult);
            return true;
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

bool CodLib::CDocumentTransform::ApplyTransform(CodLib::CDocument& Document)
    {
    try
        {
        MSXML::IXSLProcessorPtr pProcessor=NULL;
        pProcessor=pTransform->createProcessor();
        if (pProcessor!=NULL)
            {
            pProcessor->input=_variant_t((MSXML::IXMLDOMNode*)Document.pDocument);
            VARIANT_BOOL isSuccessful;
            isSuccessful=pProcessor->transform();
            if (isSuccessful==VARIANT_TRUE)
                {
                isSuccessful=Document.pDocument->loadXML((_bstr_t)pProcessor->output);
                if (isSuccessful==VARIANT_TRUE)
                    return true;
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

bool CodLib::CDocumentTransform::ApplyTransform(CodLib::CDocument& Document, _bstr_t& Result)
    {
    try
        {
        MSXML::IXSLProcessorPtr pProcessor=NULL;
        pProcessor=pTransform->createProcessor();
        if (pProcessor!=NULL)
            {
            pProcessor->input=_variant_t((MSXML::IXMLDOMNode*)Document.pDocument);
            VARIANT_BOOL isSuccessful;
            isSuccessful=pProcessor->transform();
            if (isSuccessful==VARIANT_TRUE)
                {
			    _variant_t VarResult = pProcessor->output;
			    if(VarResult.vt==VT_BSTR)
				    Result=VarResult;
			    else
				    Result=L"";
                return true;
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

/*
#pragma warning(disable:4146)
#import "C:\Program Files\Common Files\system\ado\msado15.dll" no_namespace rename("EOF", "ADOEOF")
#pragma warning(default:4146)
*/
/*
            _variant_t Output;
            Output=pProcessor->output;
            // ::MessageBox(NULL,_bstr_t(Output),_T("Transformed Output"),MB_OK);
            _StreamPtr pStream=NULL; //declare one first
            HRESULT hResult=pStream.CreateInstance(__uuidof(Stream)); //create it after
            if (FAILED(hResult))
                _com_raise_error(hResult);
            _variant_t vtEmpty(DISP_E_PARAMNOTFOUND, VT_ERROR);
            pStream->Open(vtEmpty, adModeUnknown, adOpenStreamUnspecified, L"", L"");
            pStream->Position=0;
            pStream->Type=adTypeText;
            pStream->WriteText((_bstr_t)Output, adWriteLine);
            pStream->SaveToFile(_bstr_t("Test.xml"), adSaveCreateOverWrite);
            pStream->Close();

*/
