#ifndef PARSERSTATEMACHINE_H
#define PARSERSTATEMACHINE_H

#include <string>
#include <vector>

#include "parsingcontext.h"
#include "parserstates.h"

struct ParserInput
{
   const std::wstring content;
};

struct ParserOutput
{
   std::list<ConfigurationObject*> objectList;
   ConfigurationObject* anonymousObject;
   std::vector<std::wstring> errors;
   bool noFatalErrors;
};

class ParserStateMachine
{
public:
   ParserStateMachine(const std::wstring& _content);
   ~ParserStateMachine();

   bool Parse(std::vector<std::wstring> &errors);

private:
   /**
    * @brief Creates and initializes all parser states
    * @return Starting state
    */
   ParserState* CreateStates();
   void ClearStates();

   ParsingContext context;
   ParserState* currentState;
   std::vector<ParserState*> states;
};

#endif // PARSERSTATEMACHINE_H
