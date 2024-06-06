#ifndef PARSERSTATES_H
#define PARSERSTATES_H

#include <string>
#include "parsingcontext.h"

class ParseOutput
{

};

class ParserState
{
public:
   ParserState();

   virtual ParserState* Parse(ParsingContext& _context) = 0;

protected:
   void JumpWhitespaces(const std::wstring& dataChunk, unsigned int& ptr);
   bool IsWhitespace(const wchar_t character);
   bool IsSpecialChar(const wchar_t c) const;
};

class DeclarationState : public ParserState
{
public:
   DeclarationState() = default;

   ParserState* Parse(ParsingContext& _context) override;
};

class AtomNameState : public ParserState
{
public:
   AtomNameState() = default;

   ParserState* Parse(ParsingContext& _context) override;
};

class ObjectContentState : public ParserState
{
public:
   ObjectContentState() = default;
};

class PropertyValueState : public ParserState
{
public:
   PropertyValueState() = default;
};


#endif // PARSERSTATES_H
