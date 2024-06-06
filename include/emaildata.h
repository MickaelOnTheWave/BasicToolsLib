#ifndef EMAILDATA_H
#define EMAILDATA_H

#include <string>
#include <vector>

class EmailData
{
public:
   EmailData();

   std::wstring GetTo() const;
   void SetTo(const std::wstring& value);

   std::wstring GetCc() const;
   void SetCc(const std::wstring& value);

   std::wstring GetBcc() const;
   void SetBcc(const std::wstring& value);

   std::wstring GetSubject() const;
   void SetSubject(const std::wstring& value);

   std::wstring GetBody() const;
   void SetBody(const std::wstring& value);

   bool IsHtml() const;
   void SetIsHtml(const bool value);

private:
   std::wstring to;
   std::wstring cc;
   std::wstring bcc;
   std::wstring subject;
   std::wstring body;
   bool isHtml;
};

#endif // EMAILDATA_H
