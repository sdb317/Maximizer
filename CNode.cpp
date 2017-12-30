/* XL Direct Ltd. */

#include "stdafx.h"
#include "CNode.h"
#include "CNodeList.h"
#include "CBstr.h"

CodLib::CNode::CNode(MSXML::IXMLDOMNodePtr pNode)
    {
    this->pNode=pNode;
    }

CodLib::CNode::CNode(CodLib::CNode& Node)
    {
    this->pNode=Node.pNode;
    }

CodLib::CNode::~CNode()
    {
    this->pNode=NULL;
    }

bool CodLib::CNode::Get(double* Value)
{
    try
        {
        if (this->pNode!=NULL)
            {
            if (Value!=NULL)
                {
                (*Value)=0;
                (*Value)=(double)CodLib::CBstr(this->pNode->text);
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

bool CodLib::CNode::Get(long* Value)
{
    try
        {
        if (this->pNode!=NULL)
            {
            if (Value!=NULL)
                {
                (*Value)=0;
                (*Value)=(long)CodLib::CBstr(this->pNode->text);
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

bool CodLib::CNode::Get(_bstr_t* Value)
{
    try
        {
        if (this->pNode!=NULL)
            {
            if (Value!=NULL)
                {
                (*Value)=_bstr_t();
                (*Value)=(_bstr_t)CodLib::CBstr(this->pNode->text);
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

bool CodLib::CNode::Set(double Value)
{
    try
        {
        if (this->pNode!=NULL)
            {
            this->pNode->text=(_bstr_t)CodLib::CBstr((double)Value);
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

bool CodLib::CNode::Set(long Value)
{
    try
        {
        if (this->pNode!=NULL)
            {
            this->pNode->text=(_bstr_t)CodLib::CBstr((long)Value);
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

bool CodLib::CNode::Set(_bstr_t Value)
{
    try
        {
        if (this->pNode!=NULL)
            {
            this->pNode->text=(_bstr_t)CodLib::CBstr((_bstr_t)Value);
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

CodLib::CNode::operator double() 
{
    double Value=0;
    if (Get(&Value))
        return Value;
    else
        return 0;
}

CodLib::CNode::operator long() 
{
    long Value=0L;
    if (Get(&Value))
        return Value;
    else
        return 0L;
}

CodLib::CNode::operator _bstr_t() 
{
    _bstr_t Value;
    if (Get(&Value))
        return Value;
    else
        return _bstr_t();
}

void CodLib::CNode::operator=(double Value) 
{
    Set(Value);
}

void CodLib::CNode::operator=(long Value)
{
    Set(Value);
}

void CodLib::CNode::operator=(_bstr_t Value)
{
    Set(Value);
}

CodLib::CNode CodLib::CNode::FindNode(_bstr_t Path)
{
    try
        {
        if (this->pNode!=NULL)
            {
            return CodLib::CNode(this->pNode->selectSingleNode(Path));
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

CodLib::CNodeList CodLib::CNode::FindNodes(_bstr_t Path)
{
    try
        {
        if (this->pNode!=NULL)
            {
            return CodLib::CNodeList(this->pNode->selectNodes(Path));
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

