#pragma once
#include <string>
#include <memory>
#include <ctime>
#include <vector>
#include "ipAddress.h"
#include "randomizer.h"
#include "myComputer.h"
#include "addressesStorage.h"

constexpr int nameColWidth = 50;
constexpr int playersColWidth = 7;
constexpr int pingColWidth = 4;
#define borderRow "-----------------------------------------------------------------------\n";

void PrintSpaces(int number)
{
   for (int i = 0; i < number; i++)
      std::cout << ' ';
}

class Server
{
   std::string name = "";
   std::string description = "This is server!";
   std::unique_ptr<IPAddress> ip;
   bool isAbleToConnect = true;
   int port = 0;
   int maxCapacity = 0;
   int curPlayersNumber = 0;
   int ping = 0;
   bool isHostedOnCurComputer = false;

   void RandomizeServerName()
   {
      std::vector<std::string> additionalNamesVec = {"Fun", "Play", "Together", "Cool", "Myaso", "Rasslabuha", "AWP", "DM", "Knifes", "5vs5", "Zombies"};
      name = RandomizeString(additionalNamesVec).substr(0, nameColWidth);
   }

   void ReserveFreeAddress()
   {
      this->port = RandomizeNumber(minPort, maxPort);
      if (!AddressesStorage::IsAddressFree(this->GetIPPort()))
      {
         int i = minPort;
         do 
         {
            this->port = i;
         } while(!AddressesStorage::IsAddressFree(this->GetIPPort()) && i <= maxPort);
      }
      AddressesStorage::TakeAddress(this->GetIPPort(), name);
   }
public:
   Server(int seed)
   {  
      srand((seed + 1) * std::time(0));
      RandomizeServerName();
      ip = std::unique_ptr<IPAddress>(new IPAddress(seed)); 
      ReserveFreeAddress();
      maxCapacity = RandomizeNumber(minServerCapacity, maxServerCapacity);
      curPlayersNumber = RandomizeNumber(0, maxCapacity);
      ping = RandomizeNumber(minPing, maxPing);
   }
   
   Server(const std::string& name, const std::string& description = "This is server!", const std::string& ip = "", int port = 111)
   {
      this->name = name.substr(0, nameColWidth);
      this->description = description;
      this->ip = std::unique_ptr<IPAddress>(new IPAddress(ip));
      this->port = port;
      this->maxCapacity = RandomizeNumber(minServerCapacity, maxServerCapacity);
      this->curPlayersNumber = RandomizeNumber(0, maxCapacity);
      this->ping = RandomizeNumber(minPing, maxPing);
   }

   //Конструктор для сервера запущенного на условном компьютере текушего клиента
   Server(const std::string& name, const std::string& description = "This is server!", int maxCapacity = 0)
   { 
      this->name = name.substr(0, nameColWidth);
      this->description = description;
      this->ip = std::unique_ptr<IPAddress>(new IPAddress(MyComputer::GetIPString()));
      ReserveFreeAddress();
      this->maxCapacity = (maxCapacity > maxServerCapacity) ? (maxServerCapacity + 1) : maxCapacity;
      this->isHostedOnCurComputer = true;
   }

   ~Server()
   {
      AddressesStorage::ReleaseAddress(this->GetIPPort());
   }

   std::string GetName() {return name;}

   std::string GetIP() {return ip->ToString();}   

   int GetPort() {return port;}   
  
   std::string GetIPPort() {return std::string(ip->ToString() + ":" + std::to_string(port));}   

   int GetNumberOfPlayersOnServer() {return curPlayersNumber;}

   int GetServerPlayersCapacity() {return maxCapacity;}
   
   int GetPing() {return ping;}

   void PrintServerInfo()
   {
      std::cout << "Server name: " << name << "\n";
      std::cout << "Description: " << description << "\n";
      std::cout << "IP: "; ip->PrintIPAddress(); std::cout << "\n";
      std::cout << "Port: " << port << "\n";
      std::cout << "Players: " << curPlayersNumber << "/" << maxCapacity << "\n";
      std::cout << "Ping: " << ping << "\n";
   }

   void PrintInRow()
   {
      int freeSpaceSize = (nameColWidth - name.size());
      int leftSpaces = (int)(freeSpaceSize/ 2), rightSpaces = (int)(freeSpaceSize/ 2);
      if (freeSpaceSize % 2 != 0) {
         leftSpaces += 1;
      } 
      std::cout << "| "; PrintSpaces(leftSpaces); std::cout << name; PrintSpaces(rightSpaces);
      std::cout << " | ";  PrintSpaces(playersColWidth - (1 + std::to_string(curPlayersNumber).size() + std::to_string(maxCapacity).size())); std::cout << curPlayersNumber << "/" << maxCapacity ; 
      std::cout << " | "; PrintSpaces(pingColWidth - std::to_string(ping).size()); std::cout << ping << " |\n";
      std::cout << borderRow;
   }

   void PrintConnectionAttemptResult()
   {
      if ((isAbleToConnect || isHostedOnCurComputer) && curPlayersNumber != maxCapacity)
         std::cout << "Connection established succesfully!\n";
      else if (curPlayersNumber == maxCapacity)
         std::cout << "Failed to connect. Server is full!\n";
      else
         std::cout << "Failed to connect due to the server unavailability!\n";
   }

   void UpdateInfoInRuntime()
   {  
      int minPlayersBorder = (curPlayersNumber - deltaPlayers > 0) ? curPlayersNumber - deltaPlayers : 0;
      int maxPlayersBorder = (curPlayersNumber + deltaPlayers < maxCapacity) ? curPlayersNumber + deltaPlayers : maxCapacity;
      int minPingBorder = (ping - deltaPing > 0) ? ping - deltaPing : 0; 
      int maxPingBorder = (ping + deltaPing < maxPing) ? ping + deltaPing : maxPing; 
      curPlayersNumber = RandomizeNumber(minPlayersBorder, maxPlayersBorder - minPlayersBorder);
      if (!isHostedOnCurComputer) {
         ping = RandomizeNumber(minPingBorder, maxPingBorder - minPingBorder);
      }
      isAbleToConnect = RandomizeBool();
   }
};
