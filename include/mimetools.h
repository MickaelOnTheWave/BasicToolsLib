#ifndef MIMETOOLS_H
#define MIMETOOLS_H

#include <string>
#include <vector>

#include "emaildata.h"

class MimeTools
{
public:
    MimeTools();
    virtual ~MimeTools();

    std::string CreateEmailContent(
        const std::string &fromDisplay,
        const std::string &from,
        const EmailData& emailData,
        const std::vector<std::string> &attachmentFiles
    );

    std::string CreateEmailContent(
        const std::string &fromDisplay,
        const std::string &from,
        const EmailData& emailData,
        const std::vector<std::string> &attachmentFiles,
        const std::vector<std::pair<std::string,std::string> > &attachmentBuffers
    );

private:
    void InitializeBoundaryFlag();

    std::string CreateDateString();
    std::string CreateFromString(const std::string &displayName, const std::string &from);
    std::string CreateMessageId();
    std::string CreateMimeMultipartHeader();
    std::string CreateContentHeader(const bool isHtml);
    std::string CreateMimeAttachmentsContent(const std::vector<std::string> &attachments);
    std::string CreateMimeAttachmentsContent(const std::vector<std::pair<std::string,std::string> > &attachments);
    std::string CreateSingleAttachmentContentFromBuffer(const std::string &name, const char *rawContents, unsigned long size);
    std::string CreateSingleAttachmentContentFromFile(const std::string &filename);
    std::string CreateMimeAttachmentHeader(const std::string& attachmentName);
    std::string CreateFileErrorMessage();
    std::string CreateAttachmentSizeErrorMessage(unsigned long size, unsigned long maxsize);
    std::string GetBoundaryLine();
    std::string CreateRandomString(const unsigned int size) const;

    std::string boundaryFlag;
};

#endif // MIMETOOLS_H
