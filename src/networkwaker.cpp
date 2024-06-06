#include "networkwaker.h"

#include <cstdlib>
#include <vector>
#include "stringtools.h"

#ifdef __linux__
    #include <arpa/inet.h>
    #include <sys/socket.h>
#elif _WIN32
   #define _WINSOCK_DEPRECATED_NO_WARNINGS
   #include <winsock2.h>
#endif

using namespace std;

bool NetworkWaker::Wake(const wstring& machineMac, const wstring& broadcastAddress)
{
   int socketHandle = static_cast<int>(socket(AF_INET, SOCK_DGRAM, 0));
   struct sockaddr_in udpClient, udpServer;
   int broadcast = 1;

   /** you need to set this so you can broadcast **/
   const int sockOptionResult = setsockopt(
                                   socketHandle, SOL_SOCKET, SO_BROADCAST,
#ifdef __linux__
                                   &broadcast,
#elif _WIN32
                                   (char*)&broadcast,
#endif
                                   sizeof broadcast);
   if (sockOptionResult == -1) {
      return false;
   }
   udpClient.sin_family = AF_INET;
   udpClient.sin_addr.s_addr = INADDR_ANY;
   udpClient.sin_port = 0;

   bind(socketHandle, (struct sockaddr*)&udpClient, sizeof(udpClient));

   /** …make the packet as shown above **/

   /** set server end point (the broadcast addres)**/
   udpServer.sin_family = AF_INET;
   udpServer.sin_addr.s_addr = inet_addr(StringTools::UnicodeToUtf8(broadcastAddress).c_str());
   udpServer.sin_port = htons(9);

   /** send the packet **/
   const string payload = CreatePayload(machineMac);
   const int payloadSize = static_cast<int>(sizeof(unsigned char) * payload.length());
   sendto(socketHandle, payload.c_str(), payloadSize, 0, (struct sockaddr*)&udpServer, sizeof(udpServer));
   return true;
}

string NetworkWaker::CreatePayload(const wstring& machineMac)
{
   string payload;
   payload.reserve(102);

   FillFirstSixBytesPart(payload);
   FillMacAddressPart(StringTools::UnicodeToUtf8(machineMac), payload);
   return payload;
}

void NetworkWaker::FillFirstSixBytesPart(string& payload)
{
   const unsigned char byte255 = 0xFF;
   /** first 6 bytes of 255 **/
   for(int i = 0; i < 6; i++)
      payload.push_back(byte255);
}

void NetworkWaker::FillMacAddressPart(const string& stringMac, string& payload)
{
   const string rawMac = ConvertMacToRaw(stringMac);
   /** append it 16 times to packet **/
   for(int i = 1; i <= 16; i++)
      payload += rawMac;
}

string NetworkWaker::ConvertMacToRaw(const string& stringMac)
{
   string rawMac;
   rawMac.reserve(6);

   vector<string> macComponents;
   StringTools::Tokenize(stringMac, ':', macComponents);

   vector<string>::const_iterator it = macComponents.begin();
   for (; it != macComponents.end(); ++it)
   {
      const unsigned char value = static_cast<unsigned char>(strtol(it->c_str(),NULL,16));
      rawMac.push_back(value);
   }

   return rawMac;
}
