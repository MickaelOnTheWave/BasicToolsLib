#include "emaildata.h"

EmailData::EmailData()
{

}

std::wstring EmailData::GetTo() const
{
   return to;
}

void EmailData::SetTo(const std::wstring& value)
{
   to = value;
}

std::wstring EmailData::GetCc() const
{
   return cc;
}

void EmailData::SetCc(const std::wstring& value)
{
   cc = value;
}

std::wstring EmailData::GetBcc() const
{
   return bcc;
}

void EmailData::SetBcc(const std::wstring& value)
{
   bcc = value;
}

std::wstring EmailData::GetSubject() const
{
   return subject;
}

void EmailData::SetSubject(const std::wstring& value)
{
   subject = value;
}

std::wstring EmailData::GetBody() const
{
   return body;
}

void EmailData::SetBody(const std::wstring& value)
{
   body = value;
}

bool EmailData::IsHtml() const
{
   return isHtml;
}

void EmailData::SetIsHtml(const bool value)
{
   isHtml = value;
}

