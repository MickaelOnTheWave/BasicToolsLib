#ifndef BASE64_H
#define BASE64_H

#include <string>

class Base64
{
public:

	static std::string Encode(const char* dataBlock, const size_t blockSize);

	/**
	  @warning This method should be used exclusively for text data.
	  If used for binary data, the data will stop as soon as a '00' character
	  will be found (considered as EndOfString character).
	  */
	static std::string Encode(const std::string& textData);
};

#endif // BASE64_H
