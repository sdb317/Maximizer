/* XL Direct Ltd. */

#pragma once

#include "CBstr.h"
#include "CDocument.h"

namespace CodLib
    {
    typedef MSXML::IXSLTemplatePtr CDocumentTransformPtr; // IXMLDOMDocument2 allows XPath queries.
    typedef struct MSXML::IXSLTemplate CDocumentTransformType;

/**
 * A transform that changes one document rendition into another. Typically this will be used to transform ADO-style recordset documents into the schema understood by the business objects.
 */
    class CDocumentTransform
        {
    public:
        CDocumentTransform();

        CDocumentTransform(CDocumentTransformPtr pTransform);

		void operator=(CDocumentTransformPtr pTransform);

        virtual ~CDocumentTransform();

        bool LoadTransform(_bstr_t& TransformName);

        bool ApplyTransform(CDocument& Document);

        bool ApplyTransform(CDocument&Document, _bstr_t& Result);

        CDocumentTransformPtr pTransform;

        };
    }

