/* XL Direct Ltd. */

#pragma once

namespace CodLib
    {
    class CNode;

    class CNodeList
        {
    public:
        CNodeList(MSXML::IXMLDOMNodeListPtr pNodeList=NULL);

        CNodeList(CodLib::CNodeList& NodeList);

        virtual ~CNodeList();

        CodLib::CNode NextNode();

        __declspec(property(get=GetLength)) long Length;
        long GetLength();

        bool IsEmpty() {return pNodeList==NULL?true:false;}

        MSXML::IXMLDOMNodeListPtr pNodeList;
        };
    }

