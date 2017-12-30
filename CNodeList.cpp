/* XL Direct Ltd. */

#include "stdafx.h"
#include "CNodeList.h"
#include "CNode.h"
#include "CBstr.h"

CodLib::CNodeList::CNodeList(MSXML::IXMLDOMNodeListPtr pNodeList)
    {
    this->pNodeList=pNodeList;
    }

CodLib::CNodeList::CNodeList(CodLib::CNodeList& NodeList)
    {
    this->pNodeList=NodeList.pNodeList;
    }

CodLib::CNodeList::~CNodeList()
    {
    this->pNodeList=NULL;
    }

long CodLib::CNodeList::GetLength() 
{
    try
        {
        if (this->pNodeList!=NULL)
            {
            return pNodeList->length;
            }
        }
    catch(_com_error & e)
        {
        _bstr_t Error=e.Source()+L"\n"+e.Description();
        }
    catch (...)
        {
        }
    return 0L;
}

CodLib::CNode CodLib::CNodeList::NextNode() 
{
    try
        {
        if (this->pNodeList!=NULL)
            {
            return CodLib::CNode(pNodeList->nextNode());
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

