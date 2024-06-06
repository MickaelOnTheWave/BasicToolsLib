#ifndef EMAILACCOUNTDATA_H
#define EMAILACCOUNTDATA_H

#include <string>
#include <vector>

class EmailAccountData
{
public:
   EmailAccountData();
   EmailAccountData(const EmailAccountData& other);

   std::wstring GetAddress() const;
   void SetAddress(const std::wstring& value);

   std::wstring GetPassword() const;
   void SetPassword(const std::wstring& value);

   std::wstring GetSmtpServer() const;
   void SetSmtpServer(const std::wstring& value);

   int GetSmtpPort() const;
   void SetSmtpPort(int value);

   bool GetUseSsl() const;
   void SetUseSsl(bool value);

   bool IsValid() const;

private:
   std::wstring address;
   std::wstring password;
   std::wstring smtpServer;
   bool useSsl;
   int smtpPort;
};

#endif // EMAILDATA_H
