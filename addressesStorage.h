#pragma once 
#include <map>
#include <string>

class AddressesStorage
{
   static std::map<std::string, std::string> addresses;
public:
   static void TakeAddress(const std::string address, std::string name)
   {
      AddressesStorage::addresses.emplace(address, name);  
   }

   static void ReleaseAddress(const std::string address)
   {
      addresses.erase(address);
   }

   static bool IsAddressFree(const std::string address)
   {
      return addresses.find(address) == addresses.end();
   }

   static std::string GetServerName(const std::string ipPort)
   {
      auto search = addresses.find(ipPort);
      if (search == addresses.end())
      {
         return "";  
      }
      return search->second;
   }
};
