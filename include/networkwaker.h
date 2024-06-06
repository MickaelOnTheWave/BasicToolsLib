#ifndef NETWORKWAKER_H
#define NETWORKWAKER_H

#include <string>

class NetworkWaker
{
public:
   static bool Wake(const std::wstring& machineMac,
                    const std::wstring& broadcastAddress);

private:
   static std::string CreatePayload(const std::wstring& machineMac);

   static void FillFirstSixBytesPart(std::string& payload);
   static void FillMacAddressPart(const std::string& stringMac, std::string& payload);
   static std::string ConvertMacToRaw(const std::string& stringMac);
};

#endif // NETWORKWAKER_H
