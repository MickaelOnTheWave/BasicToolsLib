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
#include "stringtools.h"

using namespace std;

//-------------------------------------------------------------------------

namespace  {
   const string standardDataDescription = "Anonymous object :\n\
\tName : \n\
\t[Property] ReportType : console|html\n\
\t[Property] SendReportByEmail : true\n\
\t[Property] ShutdownOnFinish : true\n\
\t[Property] email : mickael.dacruz@gmail.com\n\
Object\n\
\tName : Me\n\
\t[Property] Name : Cristiana Araujo\n\
\t[Property] UseSSL : true\n\
Object\n\
\tName : Client\n\
\t[Property] Name : Desktop computer\n\
\t[Property] broadcast : 192.168.1.255\n\
\t[Property] sshuser : remotebackup\n\
\tObject\n\
\t\tName : JobList\n\
\t\tObject\n\
\t\t\tName : Wake\n\
\t\tObject\n\
\t\t\tName : Backup\n\
\t\tObject\n\
\t\t\tName : ClamAv\n\
\t\tObject\n\
\t\t\tName : ChangeScreenSaver\n\
\t\tObject\n\
\t\t\tName : Shutdown\n";

   void testParseFile(const wstring& filename, const ParserOutput& expectedOutput)
   {
      ConfigurationParser parser;
      vector<wstring> errors;
      const bool result = parser.ParseFile(filename, errors);

      REQUIRE(expectedOutput.GetResult() == result);
      REQUIRE(expectedOutput.GetErrorMessages() == errors);
      REQUIRE(parser.GetFullDescription() == L"");
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

   void testParseBufferOk(const string& input, const string& expectedDescription)
   {
      const wstring wideInput = StringTools::Utf8ToUnicode(input);
      const wstring wideDescription = StringTools::Utf8ToUnicode(expectedDescription);
      testParseBufferOk(wideInput, wideDescription);
   }

   void testParseBufferError(const string& input, const ParserOutput& expectedOutput)
   {
      const wstring wideInput = StringTools::Utf8ToUnicode(input);
      ConfigurationParser parser;
      vector<wstring> errors;
      const bool result = parser.ParseBuffer(wideInput, errors);

      REQUIRE(expectedOutput.GetResult() == result);
      REQUIRE(expectedOutput.GetErrorMessages() == errors);
      REQUIRE(parser.GetFullDescription() == L"");
   }
}

//-------------------------------------------------------------------------

TEST_CASE("ConfigurationParser - Error - Non existent file")
{
   const wstring inputBuffer = L"";
   const wstring message = L"File could not be opened";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseFile(L"nonexistentfile.txt", expectedOutput);
}

TEST_CASE("ConfigurationParser - Error - Missing close brace")
{
   const string inputBuffer = R"(
   Me
   {
      Name = "Cristiana Araujo";
      UseSSL = true;

      Client
      {
         Name = "Desktop computer";
         broadcast = "192.168.1.255";
         sshuser = "remotebackup";
         JobList
         {
            Wake();
            Backup();
            ClamAv();
            ChangeScreenSaver();
            Shutdown();
         }
      }

      email = "mickael.dacruz@gmail.com";
      ReportType = "console|html";
      SendReportByEmail = true;
      ShutdownOnFinish = true;)";
   const wstring message = L"Syntax error : missing }";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseBufferError(inputBuffer, expectedOutput);
}

TEST_CASE("ConfigurationParser - Error - Missing declaration end")
{
   const string inputBuffer = R"(
   Me
   {
    Name = "Cristiana Araujo"
    UseSSL = true;
   })";
   const wstring message = L"Syntax error : Missing end declaration symbol";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseBufferError(inputBuffer, expectedOutput);
}

TEST_CASE("ConfigurationParser - Error - No symbol between atoms")
{
   const string inputBuffer = R"(
      This is a text file, nothing
      to do with a configuration file.
      Hahaha.)";
   const wstring message = L"Syntax error : two atoms without symbol between them!";
   const ParserOutput expectedOutput = ParserOutput::CreateAsError(message);
   testParseBufferError(inputBuffer, expectedOutput);
}

TEST_CASE("ConfigurationParser - Ok - Normal")
{
   const string inputBuffer = R"(
   Me
   {
       Name = "Cristiana Araujo";
       UseSSL = true;
   }

   Client
   {
       Name = "Desktop computer";
       broadcast = "192.168.1.255";
       sshuser = "remotebackup";
       JobList
       {
           Wake();
           Backup();
           ClamAv();
           ChangeScreenSaver();
           Shutdown();
       }
   }

   email = "mickael.dacruz@gmail.com";
   ReportType = "console|html";
   SendReportByEmail = true;
   ShutdownOnFinish = true;
   )";

   testParseBufferOk(inputBuffer, standardDataDescription);
}

TEST_CASE("ConfigurationParser - Ok - With inline creation")
{
   const string inputBuffer = R"(
   Client
   {
       JobList
       {
         ConsoleJob("ls -l", true, false);
         ConsoleJob
         {
            command = "ls -l";
         }
       }
   }
   )";
   const string expectedDescription = "No anonymous object.\n\
Object\n\
\tName : Client\n\
\tObject\n\
\t\tName : JobList\n\
\t\tObject\n\
\t\t\tName : ConsoleJob\n\
\t\t\t[Property] param0 : ls -l\n\
\t\t\t[Property] param1 : true\n\
\t\t\t[Property] param2 : false\n\
\t\tObject\n\
\t\t\tName : ConsoleJob\n\
\t\t\t[Property] command : ls -l\n";

   testParseBufferOk(inputBuffer, expectedDescription);
}

TEST_CASE("ConfigurationParser - Ok - With single line comment")
{
   const string inputBuffer = R"(Me
{
    Name = "Cristiana Araujo";
    UseSSL = true;
}

// Comment on main

Client
{
    // Comment inside object
    Name = "Desktop computer";
    broadcast = "192.168.1.255";// Comment on data line
    sshuser = "remotebackup";
    JobList
    {
        // Comment inside subovject
        Wake();
        Backup();
        ClamAv();
        ChangeScreenSaver();
        Shutdown();
    }
}

email = "mickael.dacruz@gmail.com";
ReportType = "console|html";
SendReportByEmail = true;
ShutdownOnFinish = true;
   )";

   testParseBufferOk(inputBuffer, standardDataDescription);
}

TEST_CASE("ConfigurationParser - Ok - With multi line comment")
{
   const string inputBuffer = R"(Me
{
    Name = "Cristiana Araujo";
    UseSSL = true;

/*

 Comment in object block
 across multiple lines
*/

}

/*

 Comment in main block
 across multiple lines
*/

Client
{
    Name = "Desktop computer";
    broadcast = "192.168.1.255";
    sshuser = "remotebackup";
    JobList
    {
/*

 Comment in sub object block
 across multiple lines
*/

        Wake();
        Backup();
        ClamAv();
        ChangeScreenSaver();
        Shutdown(/*  Comenting inside params */);
    }
}

email = "mickael.dacruz@gmail.com";
ReportType = "console|html";
SendReportByEmail = true;
ShutdownOnFinish = true;
   )";

   testParseBufferOk(inputBuffer, standardDataDescription);
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




