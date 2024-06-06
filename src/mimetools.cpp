#include "mimetools.h"

#include <algorithm>
#include <time.h>

#include "base64.h"
#include "bytestring.h"
#include "filetools.h"
#include "stringtools.h"

using namespace std;

const int boundaryFlagSize = 60;
const string newLine = string("\n");

#ifdef _WIN32
string WindowsCreateDateString()
{
    time_t rawtime;
    struct tm * timeinfo = NULL;
    time ( &rawtime );
    localtime_s (timeinfo, &rawtime);
    string timeString("Date: ");

    const size_t bufferSize = 128;
    char buffer[bufferSize];
    asctime_s(buffer, bufferSize, timeinfo);
    timeString += string(buffer);
    return timeString;
}
#else
string LinuxCreateDateString()
{
   time_t rawtime;
   struct tm* timeinfo;
   time ( &rawtime );
   timeinfo = localtime ( &rawtime );
   string timeString("Date: ");
   timeString += string(asctime(timeinfo));
   return timeString;
}

#endif

MimeTools::MimeTools() : boundaryFlag("")
{
    srand(static_cast<unsigned int>(time(NULL)));
}

MimeTools::~MimeTools()
{
}

string MimeTools::CreateEmailContent(
        const string &fromDisplay,
        const string &from,
        const EmailData& emailData,
        const vector<string>& attachmentFiles)
{
    vector<pair<string,string> > emptyBuffers;
    return CreateEmailContent(
      fromDisplay, from, emailData,
      attachmentFiles, emptyBuffers
    );
}

string MimeTools::CreateEmailContent(
        const string &fromDisplay,
        const string &from,
        const EmailData& emailData,
        const vector<string>& attachmentFiles,
        const vector<pair<string,string> > &attachmentBuffers)
{
   const string to = StringTools::UnicodeToUtf8(emailData.GetTo());
   const string cc = StringTools::UnicodeToUtf8(emailData.GetCc());
   const string bcc = StringTools::UnicodeToUtf8(emailData.GetBcc());
   const string subject = StringTools::UnicodeToUtf8(emailData.GetSubject());
   const string body = StringTools::UnicodeToUtf8(emailData.GetBody());

    string contents;
    contents += CreateDateString().c_str();
    contents += string("To: ") + to + "\n";
    contents += string("Cc: ") + cc + "\n";
    contents += string("Bcc: ") + bcc + "\n";
    contents += CreateFromString(fromDisplay, from).c_str();
    contents += string("Subject: ") + subject + "\n";

    contents += CreateMessageId();
    if (attachmentFiles.size() > 0 || attachmentBuffers.size() > 0)
        contents += CreateMimeMultipartHeader();
    contents += CreateContentHeader(emailData.IsHtml());

    contents += body + "\n";

    if (attachmentFiles.size() > 0)
        contents += CreateMimeAttachmentsContent(attachmentFiles);
    if (attachmentBuffers.size() > 0)
        contents += CreateMimeAttachmentsContent(attachmentBuffers);

    return contents;
}

void MimeTools::InitializeBoundaryFlag()
{
    boundaryFlag = CreateRandomString(boundaryFlagSize);
}

string MimeTools::CreateDateString()
{
#ifdef _WIN32
   return WindowsCreateDateString();
#else
   return LinuxCreateDateString();
#endif
}

string MimeTools::CreateFromString(const std::string& displayName, const std::string& from)
{
    string fromString("From: ");
    fromString += from;
    if (displayName != "")
        fromString += " (" + displayName + ")";
    fromString += "\n";
    return fromString;
}

string MimeTools::CreateMessageId()
{
    string messageId("Message-ID: <");
    messageId += CreateRandomString(20);
    messageId += "@taskagent.com";
    messageId += ">\n";
    return messageId;
}

string MimeTools::CreateMimeMultipartHeader()
{
    InitializeBoundaryFlag();

    string contents;
    contents += string("Content-Type: multipart/mixed;\n");
    contents += string(" boundary=\"" + boundaryFlag + "\"\n");
    contents += "\n";
    contents += "This is a multi-part message in MIME format.\n";
    contents += GetBoundaryLine();
    return contents;
}

string MimeTools::CreateContentHeader(const bool isHtml)
{
    string contents;
    contents += "Content-Type: text/";
    contents += (isHtml) ? "html" : "plain";
    contents += "; charset=ISO-8859-1; format=flowed\n";
    contents += "Content-Transfer-Encoding: 7bit\n";
    contents += "\n";
    return contents;
}

string MimeTools::CreateMimeAttachmentsContent(const std::vector<string> &attachments)
{
    string contents;
    vector<string>::const_iterator it=attachments.begin();
    for (; it!=attachments.end(); ++it)
        contents += CreateSingleAttachmentContentFromFile(*it);

    contents += GetBoundaryLine();
    return contents;
}

string MimeTools::CreateMimeAttachmentsContent(const vector<pair<string,string> > &attachments)
{
    string contents;
    vector<pair<string,string> >::const_iterator it=attachments.begin();
    for (; it!=attachments.end(); ++it)
    {
        const char* content = it->second.c_str();
        const unsigned long size = static_cast<unsigned long>(it->second.size());
        contents += CreateSingleAttachmentContentFromBuffer(it->first, content, size);
    }

    contents += newLine + GetBoundaryLine();
    return contents;
}

string MimeTools::CreateSingleAttachmentContentFromBuffer(const string &name, const char* rawContents, unsigned long size)
{
    string contents = CreateMimeAttachmentHeader(name);
    contents += Base64::Encode(rawContents, size);
    return contents;
}

string MimeTools::CreateSingleAttachmentContentFromFile(const string &filename)
{
    const unsigned long SINGLE_ATTACHMENT_MAX_SIZE = 1500000;
    string contents = CreateMimeAttachmentHeader(FileTools::GetFilenameFromUnixPath(filename));

    if (FileTools::FileExists(filename))
    {
        unsigned long fileSize;
        char* rawAttachmentContents = FileTools::GetBinaryFileContent(filename, fileSize);
        if (fileSize < SINGLE_ATTACHMENT_MAX_SIZE)
            contents += Base64::Encode(rawAttachmentContents, fileSize);
        else
        {
            contents += CreateAttachmentSizeErrorMessage(fileSize, SINGLE_ATTACHMENT_MAX_SIZE);
            contents += Base64::Encode(rawAttachmentContents, SINGLE_ATTACHMENT_MAX_SIZE);
        }
    }
    else
        contents += CreateFileErrorMessage();

    return contents;
}

string MimeTools::CreateMimeAttachmentHeader(const string &attachmentName)
{
    string contents;
    contents += newLine + GetBoundaryLine();
    contents += string("Content-Type: application/octet-stream;\n");
    contents += string(" name=\"") + attachmentName + "\"\n";
    contents += string("Content-Transfer-Encoding: base64\n");
    contents += string("Content-Disposition: attachment;\n");
    contents += string(" filename=\"") + attachmentName + "\"\n";
    contents += newLine;
    return contents;
}

string MimeTools::CreateFileErrorMessage()
{
    const string errorMessage("*** Error while reading original file content ***");
    string contents;
    contents += Base64::Encode(errorMessage.c_str(), errorMessage.size());
    contents += "\n";
    contents += GetBoundaryLine();
    return contents;
}

string MimeTools::CreateAttachmentSizeErrorMessage(unsigned long size, unsigned long maxsize)
{
    string contents;
    contents += Base64::Encode("Error : Attachment exceeded limit.\n");
    string buffer("Attachment size : ");
    buffer += ByteString<string>::Format(size) + " - Maximum size allowed : ";
    buffer += ByteString<string>::Format(maxsize) + "\n";
    contents += Base64::Encode(buffer);
    contents += Base64::Encode("Here is the beginning of the file :\n");
    return contents;
}

string MimeTools::GetBoundaryLine()
{
    return string("--") + boundaryFlag + newLine;
}

string MimeTools::CreateRandomString(const unsigned int size) const
{
    string randomString;
    for (unsigned int i=0; i<size; ++i)
    {
        int randomValue = (rand() % 10);
        randomString += char(randomValue + 48);
    }
    return randomString;
}

