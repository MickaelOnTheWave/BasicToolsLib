#include "configurationparseroutput.h"

using namespace std;

ParserOutput ParserOutput::CreateAsOk(const wstring& descriptionFile)
{
   ParserOutput output;
   output.result = true;
   output.errorMessages = {};
   output.outputDescriptionFile = descriptionFile;
   return output;
}

ParserOutput ParserOutput::CreateAsError(const wstring& _errorMessage)
{
   ParserOutput output;
   output.result = false;
   output.errorMessages.push_back(_errorMessage);
   output.outputDescriptionFile = L"";
   return output;
}

bool ParserOutput::GetResult() const
{
   return result;
}

std::vector<wstring> ParserOutput::GetErrorMessages() const
{
   return errorMessages;
}

wstring ParserOutput::GetDescriptionFile() const
{
   return outputDescriptionFile;
}
