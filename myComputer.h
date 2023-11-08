#pragma once 
#include <memory>
#include <string>
#include "ipAddress.h"

class MyComputer
{
   static std::unique_ptr<IPAddress> ip;
public:
   MyComputer()
   {
      ip = std::unique_ptr<IPAddress>(new IPAddress(std::time(0)));
   }

   MyComputer(const byte first, const byte second, const byte third, const byte fourth)
   {
      ip = std::unique_ptr<IPAddress>(new IPAddress(first, second, third, fourth));
   }

   MyComputer(std::string str)
   {
      ip = std::unique_ptr<IPAddress>(new IPAddress(str));
   }

   static std::string GetIPString()
   {
      return ip->ToString();
   }
};
