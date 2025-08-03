#ifndef CONFIGURATIONPARSEROUTPUT_H
#define CONFIGURATIONPARSEROUTPUT_H

#include <string>
#include <vector>

class ParserOutput
{
public:
   static ParserOutput CreateAsOk(const std::wstring& descriptionFile);
   static ParserOutput CreateAsError(const std::wstring& _errorMessage);

   bool GetResult() const;
   std::vector<std::wstring> GetErrorMessages() const;
   std::wstring GetDescriptionFile() const;

private:
   ParserOutput() = default;

   bool result;
   std::vector<std::wstring> errorMessages;
   std::wstring outputDescriptionFile;
};


#endif // CONFIGURATIONPARSEROUTPUT_H
