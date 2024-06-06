#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <string>
#include <vector>

class StringTools
{
public:
    static std::wstring Utf8ToUnicode(const std::string& utf8);
    static std::string UnicodeToUtf8(const std::wstring& utf8);

    static std::string SimpleTabs(const int number);

    static std::wstring Tabs(const int number);
    static std::wstring Spaces(const int number);

    static std::wstring EscapedSpaces(const std::wstring& input);
    static std::wstring LeftTrim(const std::wstring& input);

    static void Tokenize(const std::string &input,
                         const char separator,
                         std::vector<std::string> &tokenList);

    static void Tokenize(const std::wstring &input,
                         const wchar_t separator,
                         std::vector<std::wstring> &tokenList);

     static void Tokenize(const std::wstring& input,
                          const std::wstring& separator,
                          std::vector<std::wstring> &tokenList);

     static unsigned int ToInt(const std::wstring& value);

#ifdef USE_OLD_COMPILER
     static long ToLongLong(const std::wstring& value);
#else
     static long long ToLongLong(const std::wstring& value);
#endif

     static std::wstring FormatTime(const time_t time);

private:

#ifndef USE_OLD_COMPILER
    static std::wstring RawUtf8ToUnicode(const std::string& utf8);
#endif

    static std::wstring SynologyUtf8ToUnicode(const std::string& utf8);

#ifdef _WIN32
    static std::wstring WindowsUtf8ToUnicode(const std::string& utf8);
#endif

    static std::wstring GetCharRepetition(const char character, const int number);

};

#endif // STRINGTOOLS_H
