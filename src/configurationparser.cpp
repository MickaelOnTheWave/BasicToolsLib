#include "configurationparser.h"

#include <iostream>
#include <sstream>

#include "filetools.h"
#include "stringtools.h"

using namespace std;

static const wchar_t ASSIGN_SYMBOL				= '=';
static const wchar_t DECLARATION_END_SYMBOL	= ';';
static const wchar_t OPEN_CONTEXT_SYMBOL		= '{';
static const wchar_t CLOSE_CONTEXT_SYMBOL		= '}';
static const wchar_t wstring_SYMBOL				= '"';
static const wchar_t OPEN_PARAM_SYMBOL			= '(';
static const wchar_t CLOSE_PARAM_SYMBOL		= ')';
static const wchar_t PARAM_SEPARATOR_SYMBOL    = ',';

ConfigurationParser::ConfigurationParser()
	: anonymousObject(NULL), state(AWAITING_DECLARATION)
{
}

ConfigurationParser::~ConfigurationParser()
{
    ResetData();
}

int ConfigurationParser::GetFormatVersion() const
{
    return 2;
}

bool ConfigurationParser::ParseFile(const wstring& filePath, vector<wstring> &errors)
{
    wstring fileContents(L"");
    if (FileTools::GetTextFileContent(filePath, fileContents) == false)
    {
        errors.push_back(L"File could not be opened");
        return false;
    }

    return ParseBuffer(fileContents, errors);
}

bool ConfigurationParser::ParseBuffer(const wstring &content, std::vector<wstring> &errors)
{
    currentObjectStack.clear();
    objectList.clear();
    delete anonymousObject;

    wstring filteredContent = RemoveSingleLineComments(content);
    filteredContent = RemoveMultiLineComments(filteredContent);

    vector<wstring> lines;
    StringTools::Tokenize(filteredContent, '\n', lines);

    bool noFatalError = true;
    vector<wstring>::const_iterator itLine = lines.begin();
    for (; itLine != lines.end() && noFatalError; ++itLine)
    {
        bool result = ParseData(*itLine, errors);
        if (result == false)
        {
            currentObjectStack.clear();
            noFatalError = false;
        }
    }

    if (currentObjectStack.size() > 0)
    {
        errors.push_back(L"Syntax error : missing }");
        noFatalError = false;
    }

    if (noFatalError == false)
        ResetData();

    return noFatalError;
}

wstring ConfigurationParser::GetFullDescription() const
{
    if (objectList.empty() && anonymousObject == NULL)
        return wstring(L"");

    wstringstream buffer;
    if (anonymousObject == NULL)
        buffer << "No anonymous object." << endl;
    else
    {
        buffer << "Anonymous object :" << endl;
        buffer << anonymousObject->GetFullDescription(1);
    }

    list<ConfigurationObject*>::const_iterator itObj = objectList.begin();
    for (; itObj!=objectList.end(); itObj++)
    {
        buffer << "Object" << endl;
        buffer << (*itObj)->GetFullDescription(1);
    }
    return buffer.str();
}

bool ConfigurationParser::ParseData(const wstring& dataChunk, vector<wstring>& errors)
{
	unsigned int ptr = 0;
	JumpWhitespaces(dataChunk, ptr);

	if (IsEndOfLine(ptr, dataChunk))
		// Blank line - just ignore it.
      return true;

	if (state == AWAITING_OPEN_CONTEXT)
      return Parse_AwaitingOpenContext(dataChunk, ptr, errors);
	else if (state == AWAITING_DECLARATION)
      return Parse_AwaitingDeclaration(dataChunk, ptr, errors);

   errors.push_back(L"Internal parser error");
   return false;
}

bool ConfigurationParser::Parse_AwaitingOpenContext(const std::wstring& dataChunk,
                                                    unsigned int& currentPos,
                                                    std::vector<std::wstring> &errors)
{
   const wchar_t currentChar = dataChunk[currentPos];
   if (currentChar != OPEN_CONTEXT_SYMBOL)
   {
      errors.push_back(CreateSyntaxError(OPEN_CONTEXT_SYMBOL, currentChar));
      return false;
   }

   currentPos++;
   JumpWhitespaces(dataChunk, currentPos);
   if (IsEndOfLine(currentPos, dataChunk))
   {
      state = AWAITING_DECLARATION;
      return true;
   }
   else
      return CreateUnexpectedCharError(dataChunk[currentPos], errors);
}

bool ConfigurationParser::Parse_AwaitingDeclaration(const std::wstring& dataChunk,
                                                    unsigned int& ptr,
                                                    std::vector<std::wstring> &errors)
{
   wstring atom = GetAtom(dataChunk, ptr);

   JumpWhitespaces(dataChunk, ptr);
   if (atom == L"")
   {
      if (IsEndOfLine(ptr, dataChunk))
      {
         // Blank line - just ignore it.
         return true;
      }
      else if (dataChunk[ptr] == CLOSE_CONTEXT_SYMBOL)
         return Parsing_CloseContext(errors);
      else
         return CreateUnexpectedCharError(dataChunk[ptr], errors);
   }
   else if (IsEndOfLine(ptr, dataChunk))
      return Parsing_CreateNewObject(atom);
   else
   {
      wchar_t symbol = dataChunk[ptr];
      ptr++;
      if (!IsSpecialChar(symbol))
      {
         errors.push_back(L"Syntax error : two atoms without symbol between them!");
         return false;
      }

      if (symbol == DECLARATION_END_SYMBOL)
      {
         GetCurrentObject()->SetProperty(atom, "");
         return true;
      }
      else if (symbol == OPEN_PARAM_SYMBOL)
      {
         ConfigurationObject* newObject = new ConfigurationObject();
         newObject->SetName(atom);

         ParseObjectParameters(dataChunk, ptr, newObject);
         if (dataChunk[ptr] != CLOSE_PARAM_SYMBOL)
         {
            errors.push_back(L"Syntax error : missing ')'");
                  delete newObject;
            return false;
         }
         ++ptr;
         JumpWhitespaces(dataChunk, ptr);
         if (dataChunk[ptr] != DECLARATION_END_SYMBOL)
         {
            errors.push_back(L"Syntax error : missing end decl");
                  delete newObject;
            return false;
         }

         GetCurrentObject()->AddObject(newObject);
         return true;

      }
      else if (symbol == ASSIGN_SYMBOL)
      {
         JumpWhitespaces(dataChunk, ptr);

         if (IsEndOfLine(ptr, dataChunk))
         {
            errors.push_back(L"Syntax error : assign without Rvalue!");
            return false;
         }

         wstring atom2 = GetAtom(dataChunk, ptr);

         JumpWhitespaces(dataChunk, ptr);

         if (IsEndOfLine(ptr, dataChunk))
         {
            errors.push_back(L"Syntax error : Missing end declaration symbol");
            return false;
         }

         if (dataChunk[ptr] != DECLARATION_END_SYMBOL)
         {
            errors.push_back(CreateSyntaxError(DECLARATION_END_SYMBOL, dataChunk[ptr]));
            return false;
         }

         GetCurrentObject()->SetProperty(atom, atom2);
         return true;
      }
      else
      {
         const std::wstring message = L"Syntax error : Unexpected symbol ";
         errors.push_back(message + symbol);
         return false;
      }
   }
}

void ConfigurationParser::ResetData()
{
    delete anonymousObject;
    anonymousObject = NULL;

    list<ConfigurationObject*>::iterator it=objectList.begin();
    for (; it!=objectList.end(); ++it)
        delete *it;
    objectList.clear();
}

ConfigurationObject* ConfigurationParser::GetCurrentObject()
{
	if (currentObjectStack.size() > 0)
		return currentObjectStack.back();
	else
	{
		if (anonymousObject == NULL)
			anonymousObject = new ConfigurationObject();

		return anonymousObject;
	}
}

wstring ConfigurationParser::GetAtom(const wstring& dataChunk, unsigned int &ptr)
{
	bool iswstringAtom = (dataChunk[ptr] == wstring_SYMBOL);
	if (iswstringAtom)
		return GetStringAtom(dataChunk, ++ptr);
	else
		return GetSimpleAtom(dataChunk, ptr);
}

wstring ConfigurationParser::GetSimpleAtom(const wstring& dataChunk, unsigned int &ptr)
{
	unsigned int start = ptr;
	while (ptr < dataChunk.size() && !IsWhitespace(dataChunk[ptr]) && !IsSpecialChar(dataChunk[ptr]))
		ptr++;

	return wstring(dataChunk.substr(start, ptr-start));
}

wstring ConfigurationParser::GetStringAtom(const wstring& dataChunk, unsigned int &ptr)
{
	unsigned int start = ptr;
	while (ptr < dataChunk.size() && dataChunk[ptr] != wstring_SYMBOL)
		ptr++;

	wstring atom = wstring(dataChunk.substr(start, ptr-start));
	if (ptr < dataChunk.size())
		ptr++;
	return atom;
}

void ConfigurationParser::JumpWhitespaces(const wstring &dataChunk, unsigned int &ptr)
{
	const size_t end = dataChunk.size();
	while (ptr < end && IsWhitespace(dataChunk[ptr]))
		ptr++;
}

bool ConfigurationParser::ParseObjectParameters(const wstring &dataChunk, unsigned int &ptr, ConfigurationObject* currentObject)
{
    JumpWhitespaces(dataChunk, ptr);

    int paramCounter = 0;
    while (dataChunk[ptr] != CLOSE_PARAM_SYMBOL)
    {
        JumpWhitespaces(dataChunk, ptr);
        wstringstream paramName;
        paramName << "param" << paramCounter;
        wstring currentAtom = GetAtom(dataChunk, ptr);
        if (currentAtom == L"")
            break;

        currentObject->SetProperty(paramName.str(), currentAtom);
        paramCounter++;
        JumpWhitespaces(dataChunk, ptr);

        if (dataChunk[ptr] == PARAM_SEPARATOR_SYMBOL)
            ++ptr;
        else if (dataChunk[ptr] != CLOSE_PARAM_SYMBOL)
            return false;

    }
    return true;
}

wstring ConfigurationParser::CreateSyntaxError(const wchar_t expected, const wchar_t received)
{
	wstringstream stream;
	stream << "Syntax error : expected '" << expected << "', received '" << received << "'";
	return stream.str();
}

bool ConfigurationParser::CreateUnexpectedCharError(const wchar_t receivedChar,
                                                    std::vector<wstring>& errors)
{
   errors.push_back(CreateSyntaxError('$', receivedChar));
   return false;
}

bool ConfigurationParser::Parsing_CloseContext(std::vector<wstring>& errors)
{
   if (currentObjectStack.size() == 0)
   {
      errors.push_back(L"Error : no context to close!");
      return false;
   }
   currentObjectStack.pop_back();
   return true;
}

bool ConfigurationParser::Parsing_CreateNewObject(const wstring& name)
{
   ConfigurationObject* newObject = new ConfigurationObject();
   if (currentObjectStack.size() == 0)
      objectList.push_back(newObject);
   else
      currentObjectStack.back()->AddObject(newObject);

   currentObjectStack.push_back(newObject);
   newObject->SetName(name);
   state = AWAITING_OPEN_CONTEXT;
   return true;
}

bool ConfigurationParser::IsSpecialChar(const wchar_t c) const
{
	return (	c == ASSIGN_SYMBOL				||
                c == DECLARATION_END_SYMBOL     ||
				c == OPEN_CONTEXT_SYMBOL		||
				c == CLOSE_CONTEXT_SYMBOL		||
				c == OPEN_PARAM_SYMBOL			||
                c == CLOSE_PARAM_SYMBOL         ||
                c == PARAM_SEPARATOR_SYMBOL );
}

bool ConfigurationParser::IsWhitespace(const wchar_t character)
{
	return (character == ' ' || character == '\n' || character == '\r' || character == '\t');
}

bool ConfigurationParser::IsEndOfLine(const unsigned int ptr, const wstring& dataChunk) const
{
   return (ptr >= dataChunk.size());
}

wstring ConfigurationParser::RemoveSingleLineComments(const wstring& content) const
{
   const size_t endMarkerOffset = 0;
   return RemoveComments(L"//", L"\n", 0, content);
}

wstring ConfigurationParser::RemoveMultiLineComments(const wstring& content) const
{
   const wstring endMarker = L"*/";
   const size_t endMarkerOffset = endMarker.size();
   return RemoveComments(L"/*", endMarker, endMarkerOffset, content);
}

wstring ConfigurationParser::RemoveComments(const wstring& beginMarker,
                                            const wstring& endMarker,
                                            const size_t endMarkerOffset,
                                            const wstring& content) const
{
   wstring filteredContent = content;
   size_t currentPos = 0;

   while (currentPos != wstring::npos)
   {
      currentPos = filteredContent.find(beginMarker);
      if (currentPos != wstring::npos)
      {
         size_t endPos = filteredContent.find(endMarker, currentPos);
         filteredContent.erase(currentPos, endPos-currentPos + endMarkerOffset);
      }
   }
   return filteredContent;
}

