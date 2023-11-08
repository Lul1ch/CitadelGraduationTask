#pragma once
#include <unordered_map>
#include <memory>
#include "server.h"

template<class T>
class Database
{
protected:
   std::unordered_map<std::string, std::shared_ptr<T>> dataMap;
public:
   Database(int numberOfElements = 0)
   {
      for (int i = 0; i < numberOfElements; i++)
      {
         T* elem = new T(i);
         AddData(elem->GetName(), elem);  
      }  
   }
   virtual void AddData(const std::string key, T* value)
   {
      dataMap.insert(std::make_pair(key, std::shared_ptr<T>(value)));
   }
   
   virtual void EraseData(const std::string& key)
   {
      auto numberOfRemovedElements = dataMap.erase(key);
      if (numberOfRemovedElements > 0)
         std::cout << "Element erased succesfully!\n";
      else
         std::cout << "Key not found!\n";
   }  

   std::shared_ptr<T> FindData(const std::string& key, bool printSearchResult)
   {
      auto search = dataMap.find(key);
      if (search == dataMap.end())
      {
         if (printSearchResult)
            std::cout << "Server not found!\n";
         return nullptr;
      }
      return search->second;
   }

   std::string GetServerIPPort(const std::string& key)
   {
      auto search = FindData(key, false);
      if (search != nullptr)
         return search->GetIPPort();
      return "Server not found!";
   }

   void UpdateDataInRuntime();
   
   void PrintDatabase();

   void PrintTopics();
};

class ServerDatabase : public Database<Server>
{
   std::unordered_map<std::string, std::string> IPToNameMatchingMap;
public:
   ServerDatabase(int numberOfElements)
   {
      for (int i = 0; i < numberOfElements; i++)
      {
         Server* elem = new Server(i);
         AddData(elem->GetName(), elem);  
      }  
   }

   void AddData(const std::string key, Server* value) override
   {
      IPToNameMatchingMap.insert(std::make_pair(value->GetIP(), value->GetName()));
      dataMap.insert(std::make_pair(key, std::shared_ptr<Server>(value)));
   }
 
   void EraseData(const std::string& key) override
   {
      auto search = dataMap.find(key);
      if (search != dataMap.end())
      {
         IPToNameMatchingMap.erase(search->second->GetIP());
         dataMap.erase(search->first);
         std::cout << "Element erased succesfully!\n";
      } else
         std::cout << "Key not found!\n";
   }
  
   std::string GetNameByIP(const std::string& ip)
   {
      return IPToNameMatchingMap[ip];
   }
   
   void PrintTopics()
   {
      std::string nameTopic = "Servers";
      std::string playersTopic = "Players";
      std::string pingTopic = "Ping";
   
      std::cout << "| "; PrintSpaces((nameColWidth - nameTopic.size())/2 + 1); std::cout << nameTopic; PrintSpaces((nameColWidth - nameTopic.size())/2);
      std::cout << " | "; PrintSpaces(playersColWidth - playersTopic.size()); std::cout << playersTopic;
      std::cout << " | "; PrintSpaces(pingColWidth - pingTopic.size()); std::cout << pingTopic << " |\n";
      std::cout << borderRow;
   }

   void PrintDatabase() 
   {
      PrintTopics();
      for(auto& kv: dataMap)
      {
         kv.second->PrintInRow();
      }
   }

   void UpdateDataInRuntime()
   {
      for(auto& kv: dataMap)
      {
         kv.second->UpdateInfoInRuntime();
      }
   }
};
