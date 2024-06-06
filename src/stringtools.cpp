#include "stringtools.h"

#include <locale>
#include <sstream>
#ifdef _WIN32
   #include <Windows.h>
#endif
#ifndef USE_OLD_COMPILER
   #include <codecvt>
#endif

using namespace std;

wstring StringTools::Utf8ToUnicode(const string& utf8)
{
#ifdef _WIN32
   return WindowsUtf8ToUnicode(utf8);
#elif USE_OLD_COMPILER
   return SynologyUtf8ToUnicode(utf8);
#else
   return RawUtf8ToUnicode(utf8);
#endif
}

string StringTools::UnicodeToUtf8(const wstring& value)
{
#ifdef USE_OLD_COMPILER
   return std::string(value.begin(), value.end());
#else
   std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
   return myconv.to_bytes(value);
#endif
}

string StringTools::SimpleTabs(const int number)
{
   string result;
   for (int i=0; i<number; ++i)
       result += '\t';
   return result;
}

wstring StringTools::Tabs(const int number)
{
    return GetCharRepetition('\t', number);
}

wstring StringTools::Spaces(const int number)
{
    return GetCharRepetition(' ', number);
}

wstring StringTools::EscapedSpaces(const wstring& input)
{
   wstring output(input);

   size_t currentPos = 0;
   while (true)
   {
      size_t foundPos = output.find(L' ', currentPos);
      if (foundPos != wstring::npos)
      {
         output.replace(foundPos, 1, L"\\ ");
         currentPos = foundPos + 2;
      }
      else
         break;
   }

   return output;
}

wstring StringTools::LeftTrim(const wstring &input)
{
    size_t pos = 0;
    for (; pos < input.size(); ++pos)
    {
        if (input[pos] != ' ' && input[pos] != '\t')
            break;
    }

    return (pos < input.size()) ? input.substr(pos) : input;
}

unsigned int StringTools::ToInt(const wstring& value)
{
#ifdef _WIN32
   return _wtoi(value.c_str());
#else
   wchar_t* end;
   const int base = 10;
   return wcstol(value.c_str(), &end, base);
#endif
}

#ifdef USE_OLD_COMPILER
long StringTools::ToLongLong(const wstring& value)
#else
long long StringTools::ToLongLong(const wstring& value)
#endif
{
#ifdef _WIN32
   return _wtol(value.c_str());
#else
   wchar_t* end;
   const int base = 10;
   return wcstoll(value.c_str(), &end, base);
#endif
}

void StringTools::Tokenize(const std::string &input,
                           const char separator,
                           vector<string> &tokenList)
{
   tokenList.clear();
   istringstream is(input);
   string token;
   while (getline(is, token, separator))
      tokenList.push_back(token);
}

void StringTools::Tokenize(const wstring &input,
                           const wchar_t separator,
                           vector<wstring> &tokenList)
{
   tokenList.clear();
   wistringstream is(input);
   wstring token;
   while (getline(is, token, separator))
      tokenList.push_back(token);
}

void StringTools::Tokenize(const wstring& input,
                           const wstring& separator,
                           vector<wstring> &tokenList)
{
    tokenList.clear();

    size_t currentPos = 0;
    while (1)
    {
        size_t nextPos = input.find(separator, currentPos);
        if (nextPos != wstring::npos)
        {
            ++nextPos;
            tokenList.push_back(input.substr(currentPos, nextPos-currentPos));
            currentPos = nextPos + separator.size()-1;
        }
        else
        {
            tokenList.push_back(input.substr(currentPos));
            break;
        }
    }
}

wstring StringTools::FormatTime(const time_t time)
{
   wstringstream ss;
   int seconds = 0;
   int minutes = 0;
   int hours = 0;
   if (time < 60)
      seconds = static_cast<int>(time);
   else
   {
      minutes = (int)(time/60);
      seconds = static_cast<int>(time - minutes*60);
      if (minutes > 60)
      {
         hours = (int)(minutes/60);
         minutes = minutes - hours*60;
      }
   }

   if (hours > 0)
      ss << hours << "h";
   if (minutes > 0)
   {
      if (ss.str() != L"")
         ss << " ";
      ss << minutes << "m";
   }

   if (ss.str() != L"")
      ss << " ";
   ss << seconds << "s";

    return ss.str();
}

// TODO : replace this with C++11 code. And test it.
#ifndef USE_OLD_COMPILER
std::wstring StringTools::RawUtf8ToUnicode(const std::string& utf8)
{
   std::vector<unsigned long> unicode;
   size_t i = 0;
   while (i < utf8.size())
   {
      unsigned long uni;
      size_t todo;
      unsigned char ch = utf8[i++];
      if (ch <= 0x7F)
      {
         uni = ch;
         todo = 0;
      }
      else if (ch <= 0xBF)
      {
         return L"";
      }
      else if (ch <= 0xDF)
      {
         uni = ch & 0x1F;
         todo = 1;
      }
      else if (ch <= 0xEF)
      {
         uni = ch & 0x0F;
         todo = 2;
      }
      else if (ch <= 0xF7)
      {
         uni = ch & 0x07;
         todo = 3;
      }
      else
      {
         throw std::logic_error("not a UTF-8 string");
      }
      for (size_t j = 0; j < todo; ++j)
      {
         if (i == utf8.size())
            throw std::logic_error("not a UTF-8 string");
         unsigned char ch = utf8[i++];
         if (ch < 0x80 || ch > 0xBF)
            throw std::logic_error("not a UTF-8 string");
         uni <<= 6;
         uni += ch & 0x3F;
      }
      if (uni >= 0xD800 && uni <= 0xDFFF)
         throw std::logic_error("not a UTF-8 string");
      if (uni > 0x10FFFF)
         throw std::logic_error("not a UTF-8 string");
      unicode.push_back(uni);
   }
   std::wstring utf16;
   for (size_t i = 0; i < unicode.size(); ++i)
   {
      unsigned long uni = unicode[i];
      if (uni <= 0xFFFF)
      {
         utf16 += (wchar_t)uni;
      }
      else
      {
         uni -= 0x10000;
         utf16 += (wchar_t)((uni >> 10) + 0xD800);
         utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
      }
   }
   return utf16;
}
#endif

std::wstring StringTools::SynologyUtf8ToUnicode(const std::string& utf8)
{
   std::vector<unsigned long> unicode;
   size_t i = 0;
   while (i < utf8.size())
   {
      unsigned long uni;
      size_t todo;
      unsigned char ch = utf8[i++];
      if (ch <= 0x7F)
      {
         uni = ch;
         todo = 0;
      }
      else if (ch <= 0xBF)
      {
         return L"";
      }
      else if (ch <= 0xDF)
      {
         uni = ch & 0x1F;
         todo = 1;
      }
      else if (ch <= 0xEF)
      {
         uni = ch & 0x0F;
         todo = 2;
      }
      else if (ch <= 0xF7)
      {
         uni = ch & 0x07;
         todo = 3;
      }
      else
      {
         return L"";
      }
      for (size_t j = 0; j < todo; ++j)
      {
         if (i == utf8.size())
            return L"";
         unsigned char ch = utf8[i++];
         if (ch < 0x80 || ch > 0xBF)
            return L"";
         uni <<= 6;
         uni += ch & 0x3F;
      }
      if (uni >= 0xD800 && uni <= 0xDFFF)
         return L"";
      if (uni > 0x10FFFF)
         return L"";
      unicode.push_back(uni);
   }
   std::wstring utf16;
   for (size_t i = 0; i < unicode.size(); ++i)
   {
      unsigned long uni = unicode[i];
      if (uni <= 0xFFFF)
      {
         utf16 += (wchar_t)uni;
      }
      else
      {
         uni -= 0x10000;
         utf16 += (wchar_t)((uni >> 10) + 0xD800);
         utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
      }
   }
   return utf16;
}

#ifdef _WIN32
std::wstring StringTools::WindowsUtf8ToUnicode(const std::string& utf8)
{
//   std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//   return converter.from_bytes(utf8);

   if (utf8.empty())
      return wstring();

   const size_t charsNeeded = ::MultiByteToWideChar(
      CP_UTF8, 0, utf8.data(),
      static_cast<int>(utf8.size()), NULL, 0
   );
   if (charsNeeded == 0)
      return L"";

   vector<wchar_t> buffer(charsNeeded);
   const int charsConverted = ::MultiByteToWideChar(
      CP_UTF8, 0, utf8.data(),
      static_cast<int>(utf8.size()), &buffer[0], 
      static_cast<int>(buffer.size())
   );
   if (charsConverted == 0)
      return L"";

   return wstring(&buffer[0], charsConverted);
}
#endif

wstring StringTools::GetCharRepetition(const char character, const int number)
{
    wstring result;
    for (int i=0; i<number; ++i)
        result += character;
    return result;
}
