/* XL Direct Ltd. */

#pragma once

#include "CBstr.h"

#define XLdData _T("<?xml version=\"1.0\"?><XLd:Data xmlns:XLd=\"http://www.xl-direct.com/\"/>")

namespace CodLib
    {
    typedef MSXML::IXMLDOMDocument2Ptr CDocumentPtr; // IXMLDOMDocument2 allows XPath queries.
    typedef struct MSXML::IXMLDOMDocument2 CDocumentType;

    class CNode;

    class CNodeList;

/**
 * Base class functionality for all documents.
 */
    class CDocument
        {
    public:
        CDocument(bool FreeThreaded=false);

        CDocument(CDocumentPtr pDocument);

		void operator=(CDocumentPtr pDocument);

        virtual ~CDocument();

        bool LoadDocument(const _bstr_t& DocumentName);

        bool SaveDocument(const _bstr_t& DocumentName);

        bool FreeThreaded;

        CodLib::CNode FindNode(_bstr_t Path);
        CodLib::CNodeList FindNodes(_bstr_t Path);

        bool IsEmpty() {return pDocument==NULL?true:false;}

        CDocumentPtr pDocument;

    private:
		void SetDefaultProperties();
        };

    }

