/*
 * BasicToolsLib
 * Copyright (C) 2025 Guimarães Tecnologia Ltda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <catch2/catch.hpp>

#include "configurationparser.h"
#include "configurationparseroutput.h"
#include "filetools.h"

using namespace std;

//-------------------------------------------------------------------------

namespace  {
   wstring DataFile(const wstring& filename)
   {
      const wstring dataFolder = L"ConfigurationParser/";
      return dataFolder + filename;
   }

   wstring GetDescriptionContent(const ParserOutput& output)
   {
      const wstring fullFilename = DataFile(output.GetDescriptionFile());
      return FileTools::GetTextFileContent(fullFilename);
   }

   void testParseFile(const wstring& filename, const ParserOutput& expectedOutput)
   {
      ConfigurationParser parser;
      vector<wstring> errors;
      const bool result = parser.ParseFile(DataFile(filename), errors);
      const wstring expectedDescription = GetDescriptionContent(expectedOutput);

      REQUIRE(expectedOutput.GetResult() == result);
      REQUIRE(expectedOutput.GetErrorMessages() == errors);
      REQUIRE(expectedDescription == parser.GetFullDescription());
   }

   void testParseFileOk(const wstring& testName)
   {
      const wstring extension = L".txt";
      const wstring inputFile = L"ok_" + testName + extension;
      const wstring expectedOutputFile = L"ok_" + testName + L"_expectedContent" + extension;
      const ParserOutput expectedOutput = ParserOutput::CreateAsOk(expectedOutputFile);
      testParseFile(inputFile, expectedOutput);
   }

   void testParseBufferOk(const wstring& input, const wstring& expectedDescription)
   {
      ConfigurationParser parser;
      vector<wstring> errors;
      const bool result = parser.ParseBuffer(input, errors);

      REQUIRE(result == true);
      REQUIRE(errors.size() == 0);
      REQUIRE(parser.GetFullDescription() == expectedDescription);
   }
}

//-------------------------------------------------------------------------

TEST_CASE("ConfigurationParser - Error - Non existent file")
{
   const wstring message = L"File could not be opened";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseFile(L"nonexistentfile.txt", expectedOutput);
}

TEST_CASE("ConfigurationParser - Error - Missing close brace")
{
   const wstring message = L"Syntax error : missing }";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseFile(L"error_missingCloseBrace.txt", expectedOutput);
}

TEST_CASE("ConfigurationParser - Error - Missing declaration end")
{
   const wstring message = L"Syntax error : Missing end declaration symbol";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseFile(L"error_missingEndDeclaration.txt", expectedOutput);
}

TEST_CASE("ConfigurationParser - Error - No symbol between atoms")
{
   const wstring message = L"Syntax error : two atoms without symbol between them!";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseFile(L"error_missingNoSymbolBetweenAtoms.txt", expectedOutput);
}

TEST_CASE("ConfigurationParser - Ok - Normal")
{
   testParseFileOk(L"normal");
}

TEST_CASE("ConfigurationParser - Ok - With inline creation")
{
   testParseFileOk(L"inlineCreation");
}

TEST_CASE("ConfigurationParser - Ok - With single line comment")
{
   testParseFileOk(L"singleLineComment");
}

TEST_CASE("ConfigurationParser - Ok - With multi line comment")
{
   testParseFileOk(L"multiLineComment");
}

TEST_CASE("ConfigurationParser - Ok - Properties on anonymous objects")
{
   const wstring input = L"property1 = value1;\n"
                         L"anotherProp = 456;\n";
   const wstring expectedDescription = L"Anonymous object :\n"
                                       L"\tName : \n"
                                       L"\t[Property] anotherProp : 456\n"
                                       L"\t[Property] property1 : value1\n";
   testParseBufferOk(input, expectedDescription);
}




