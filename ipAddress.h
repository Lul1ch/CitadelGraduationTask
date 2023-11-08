#pragma once
#include <iostream>
#include <ctime>
#include <bitset>

typedef std::bitset<8> byte;

class IPAddress
{
   byte first = 0;
   byte second = 0;
   byte third = 0;
   byte fourth = 0;

   byte GetRandomByte()
   {
      return rand() % 256;  
   }

   std::string ByteToString(const byte byte) { return std::to_string(byte.to_ulong()); }
public:
   IPAddress(int seed)
   {
      srand((seed + 1) * time(0));
      this->first = GetRandomByte();
      this->second = GetRandomByte();
      this->third = GetRandomByte();
      this->fourth = GetRandomByte();
   }

   IPAddress(const byte first, const byte second, const byte third, const byte fourth)
   {
      this->first = first;
      this->second = second;
      this->third = third;
      this->fourth = fourth;
   }   

   IPAddress(std::string str)
   {
      bool isOk;
      do 
      {
         isOk = true;
         try 
         {
            std::string substr = str.substr(0,str.find('.'));
            this->first = std::stoi(substr);
            str.erase(0,substr.size() + 1);
            
            substr = str.substr(0,str.find('.'));
            this->second = std::stoi(substr);
            str.erase(0,substr.size() + 1);
            
            substr = str.substr(0,str.find('.'));
            this->third = std::stoi(substr);
            str.erase(0,substr.size() + 1);
            
            substr = str.substr(0,str.find('.'));
            this->fourth = std::stoi(substr);
         } 
         catch(const std::exception e)
         {
            std::cout << "Incorrect IP string format. The correct one is 255.255.255.255!\nPlease enter ip again: ";
            std::getline(std::cin, str);
            isOk = false;
         }
      } while (!isOk);
   }

   std::string ToString()
   {
      return std::string(ByteToString(first) + "." + ByteToString(second) + "." + ByteToString(third) + "." + ByteToString(fourth));
   }

   void PrintIPAddress()
   {
      std::cout << ToString();
   }

   byte GetFirst() const { return this->first; }
   byte GetSecond() const { return this->second; }
   byte GetThird() const { return this->third; }
   byte GetFourth() const { return this->fourth; }

   IPAddress& operator=(const IPAddress& element)
   {
      this->first = element.GetFirst();
      this->second = element.GetSecond();
      this->third = element.GetThird();
      this->fourth = element.GetFourth();
      return *this;
   }
};
