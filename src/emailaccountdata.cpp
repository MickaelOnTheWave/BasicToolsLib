#include "emailaccountdata.h"

EmailAccountData::EmailAccountData() :
   address(L""), password(L""), smtpServer(L""),
   useSsl(false), smtpPort(-1)
{
}

EmailAccountData::EmailAccountData(const EmailAccountData& other) :
   address(other.address),
   password(other.password),
   smtpServer(other.smtpServer),
   useSsl(other.useSsl),
   smtpPort(other.smtpPort)
{
}

std::wstring EmailAccountData::GetAddress() const
{
   return address;
}

void EmailAccountData::SetAddress(const std::wstring& value)
{
   address = value;
}

std::wstring EmailAccountData::GetPassword() const
{
   return password;
}

void EmailAccountData::SetPassword(const std::wstring& value)
{
   password = value;
}

std::wstring EmailAccountData::GetSmtpServer() const
{
   return smtpServer;
}

void EmailAccountData::SetSmtpServer(const std::wstring& value)
{
   smtpServer = value;
}

int EmailAccountData::GetSmtpPort() const
{
   return smtpPort;
}

void EmailAccountData::SetSmtpPort(int value)
{
   smtpPort = value;
}

bool EmailAccountData::GetUseSsl() const
{
   return useSsl;
}

void EmailAccountData::SetUseSsl(bool value)
{
   useSsl = value;
}

bool EmailAccountData::IsValid() const
{
   if (address == L"")
       return false;
   else if (password == L"")
       return false;
   else if (smtpPort == -1)
       return false;
   else if (smtpServer == L"")
       return false;
   else
       return true;
}
