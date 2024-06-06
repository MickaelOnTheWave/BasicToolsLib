#ifndef CONFIGURATIONPARSER_H
#define CONFIGURATIONPARSER_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "configurationobject.h"

/// TODO make parser recognize parenteses as numbered properties.
/// TODO Have to way to make parser use " inside parameters.
class ConfigurationParser
{
public:
	ConfigurationParser();
	virtual ~ConfigurationParser();

   void ResetData();

   int GetFormatVersion(void) const;

	/**
	  @return true if file was successfully parse or false if a
	  non-recoverable error occurred while parsing.
	  @param[in]	filePath File to parse
	  @param[out]	errors List of errors that occurred during parsing.
	  */
    bool ParseFile(const std::wstring& filePath, std::vector<std::wstring>& errors);

    bool ParseBuffer(const std::wstring& content, std::vector<std::wstring>& errors);

    std::wstring GetFullDescription() const;

	std::list<ConfigurationObject*> objectList;
	ConfigurationObject* anonymousObject;

protected:
   bool ParseData(const std::wstring& dataChunk, std::vector<std::wstring> &errors);
   bool Parse_AwaitingOpenContext(const std::wstring& dataChunk,
                                  unsigned int& currentPos,
                                  std::vector<std::wstring> &errors);
   bool Parse_AwaitingDeclaration(const std::wstring& dataChunk,
                                  unsigned int& ptr,
                                  std::vector<std::wstring> &errors);

   void JumpWhitespaces(const std::wstring& dataChunk, unsigned int& ptr);
   bool ParseObjectParameters(const std::wstring &dataChunk, unsigned int &ptr, ConfigurationObject* currentObject);

	std::wstring CreateSyntaxError(const wchar_t expected, const wchar_t received);
   bool CreateUnexpectedCharError(const wchar_t receivedChar,
                                  std::vector<std::wstring> &errors);

   bool Parsing_CloseContext(std::vector<std::wstring>& errors);
   bool Parsing_CreateNewObject(const std::wstring& name);

	std::wstring GetAtom(const std::wstring &dataChunk, unsigned int& ptr);
	std::wstring GetSimpleAtom(const std::wstring &dataChunk, unsigned int& ptr);
	std::wstring GetStringAtom(const std::wstring &dataChunk, unsigned int& ptr);

	ConfigurationObject *GetCurrentObject();

	static const unsigned int AWAITING_OPEN_CONTEXT = 0;
	static const unsigned int AWAITING_DECLARATION = 1;
	unsigned int state;
	std::vector<ConfigurationObject*> currentObjectStack;
	bool IsSpecialChar(const wchar_t c) const;
	bool IsWhitespace(const wchar_t character);
	bool IsEndOfLine(const unsigned int ptr, const std::wstring &dataChunk) const;

private:
   std::wstring RemoveSingleLineComments(const std::wstring& content) const;
   std::wstring RemoveMultiLineComments(const std::wstring& content) const;
   std::wstring RemoveComments(const std::wstring& beginMarker,
                               const std::wstring& endMarker,
                               const size_t endMarkerOffset,
                               const std::wstring& content) const;
};

#endif // CONFIGURATIONPARSER_H
