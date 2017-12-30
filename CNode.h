/* XL Direct Ltd. */

#pragma once

namespace CodLib
    {
    class CNodeList;

    class CNode
        {
    public:
        CNode(MSXML::IXMLDOMNodePtr pNode=NULL);

        CNode(CodLib::CNode& Node);

        virtual ~CNode();

        bool Get(double* Value);
        bool Get(long* Value);
        bool Get(_bstr_t* Value);
        bool Set(double Value);
        bool Set(long Value);
        bool Set(_bstr_t Value);

        operator double();
        operator long();
        operator _bstr_t();
        void operator=(double Value);
		void operator=(long Value);
		void operator=(_bstr_t Value);

        CodLib::CNode FindNode(_bstr_t Path);
        CodLib::CNodeList FindNodes(_bstr_t Path);

        bool IsEmpty() {return pNode==NULL?true:false;}

        MSXML::IXMLDOMNodePtr pNode;
        };
    }

