#pragma once 
#include "server.h"
#include "database.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

template<class T>
class Command
{
protected:
std::vector<std::string> arguments;
public:
   virtual void Execute(T inputStruct){}
   
   virtual std::string GetName() {return "/command";}
};

namespace serversList
{
   void ParseStringIntoWords(std::vector<std::string>& vec,const std::string str)
   {
      std::string word;
      for (int i = 0; i < str.size();i++)
      {
         if (str[i] == ' ' && !word.empty()) 
         {
            vec.push_back(word);
            word.clear();
         } else if (str[i] != ' ') {
            word += str[i];
         }
      }
      if (!word.empty()) { vec.push_back(word);}
   }
   
   bool IsAnyArguments(const std::string str)
   {
      int startIndex = str.find(" ");
      for (int i = startIndex; i < str.size(); i++)
      {
         if (str[i] != ' ')
            return true;
      }
      return false;
   }
     
   void IsNoArgumentsNeeded(bool& condition,const std::string& commandName)
   {
      std::vector<std::string> noArgumentsCommands = {"/print", "/exit", "/add"};
      for (int i = 0; i < noArgumentsCommands.size(); i++)
      {
         if (commandName == noArgumentsCommands[i])
            condition = true;
      }
   }
   
   class PrintList : public Command<std::string>
   {
      std::unique_ptr<ServerDatabase> db;
   public:
      PrintList(const std::shared_ptr<ServerDatabase>& db)
      {
         this->db = std::unique_ptr<ServerDatabase>(db.get());  
      }
 
      void Execute (const std::string inputStruct) override
      {
         db->PrintDatabase();
      }
   
      std::string GetName() override
      {
         return "/print";
      }
   };
   
   class AddElement : public Command<std::string>
   {
      std::unique_ptr<ServerDatabase> db;
    
      int GetCorrectIntInput(const std::string preInputStr)
      {
         std::string input;
         int returnValue;
         bool isCorrectInput = true;
         do {
            isCorrectInput = true;
            std::cout << preInputStr; std::getline(std::cin, input);
            try { returnValue = std::stoi(input); }
            catch(std::exception e) { std::cout << "Incorrect input!\n"; isCorrectInput = false; }
         } while (!isCorrectInput);
         return returnValue;
      }
  
      void GetInput(Server*& srv)
      {
         std::string flag = "", name = "", description = "";
         std::cout << "Enter flag: "; std::getline(std::cin, flag);
         
         bool goNext = false;
         while (!goNext) {
            std::cout << "Enter server name: "; std::getline(std::cin, name);
            if (name == "")
               std::cout << "Server name can not be blank!\n";
            else if (db->FindData(name, false) != nullptr)
               std::cout << "Server with this name already exists!\n";
            else
               goNext = true;
         }

         std::cout << "Enter server description: "; std::getline(std::cin, description);
         
         if (flag == "-host")
         {
            int maxPlayers = GetCorrectIntInput("Enter server's players capacity: ");
            srv = new Server(name, description, maxPlayers);
         } else if (flag == "")
         {
            std::string ip;
            std::cout << "Enter server ip: "; std::getline(std::cin, ip);
            int port = GetCorrectIntInput("Enter server's port: ");   
            srv = new Server(name, description, ip, port); 
         } else {
            std::cout << "Unknown flag!\n";
         }
      }
   public:
      AddElement(const std::shared_ptr<ServerDatabase>& db)
      {
         this->db = std::unique_ptr<ServerDatabase>(db.get());  
      }
 
      void Execute (const std::string inputStruct) override
      {
         Server* srv = nullptr;
         GetInput(srv);
         if (srv != nullptr)
         {
            db->AddData(srv->GetName(), srv);
            std::cout << "Element succesfully added!\n";
         } else {
            std::cout << "Failed to creater server!\n";
         }
      }
   
      std::string GetName() override
      {
         return "/add";
      }
   };
   
   class FindElement : public Command<std::string>
   {
      std::unique_ptr<ServerDatabase> db;
      void AnalyzeInput(std::string& key, const std::string& inputStr)
      {
         ParseStringIntoWords(arguments, inputStr);
         key = arguments[1];
         if (arguments[1] == "-ip" && arguments.size() > 2)
         {
            key = db->GetNameByIP(arguments[2]);
         } else if (arguments[1] == "-ip"){
            std::cout << "Not enough arguments!\n";
            return;
         }
         arguments.clear();
      }
public:
      FindElement(const std::shared_ptr<ServerDatabase>& db)
      {
         this->db = std::unique_ptr<ServerDatabase>(db.get());  
      }
 
      void Execute (const std::string inputStruct) override
      {
         std::string key = "";
         AnalyzeInput(key, inputStruct);

         auto search = db->FindData(key, true);
         if (search != nullptr)
         {
            db->PrintTopics();
            search->PrintInRow();
         }
      }
   
      std::string GetName() override
      {
         return "/find";
      }
   };
   
   class EraseElement : public Command<std::string>
   {
      std::unique_ptr<ServerDatabase> db;
   public:
      EraseElement(const std::shared_ptr<ServerDatabase>& db)
      {
         this->db = std::unique_ptr<ServerDatabase>(db.get());  
      }
 
      void Execute (const std::string inputStruct) override
      {
         ParseStringIntoWords(arguments, inputStruct);
         db->EraseData(arguments[1]);
         arguments.clear();
      }
   
      std::string GetName() override
      {
         return "/erase";
      }
   };

   class GetIPPort : public Command<std::string>
   {
      std::unique_ptr<ServerDatabase> db;
   public:
      GetIPPort(const std::shared_ptr<ServerDatabase>& db)
      {
         this->db = std::unique_ptr<ServerDatabase>(db.get());  
      }
 
      void Execute (const std::string inputStruct) override
      { 
         ParseStringIntoWords(arguments, inputStruct);
         std::cout << db->GetServerIPPort(arguments[1]) << "\n";
         arguments.clear();
      }   
      
      std::string GetName() override
      {
         return "/getip:port";
      }
   };

   class Info : public Command<std::string>
   {
      std::unique_ptr<ServerDatabase> db;
   public:
      Info(const std::shared_ptr<ServerDatabase>& db)
      {
         this->db = std::unique_ptr<ServerDatabase>(db.get());  
      }
 
      void Execute (const std::string inputStruct) override
      { 
         ParseStringIntoWords(arguments, inputStruct);
         
         auto search = db->FindData(arguments[1], true);
         if (search != nullptr)
            search->PrintServerInfo();
         arguments.clear();
      }   
      
      std::string GetName() override
      {
         return "/info";
      }
   };

   class Connect : public Command<std::string>
   {
      std::unique_ptr<ServerDatabase> db;
   public:
      Connect(std::shared_ptr<ServerDatabase>& db)
      {
         this->db = std::unique_ptr<ServerDatabase>(db.get());
      }
   
      void Execute (const std::string inputStruct) override
      {         
         ParseStringIntoWords(arguments, inputStruct);
      
         std::string key = AddressesStorage::GetServerName(arguments[1]);
         auto search = db->FindData(key, true);
         if (search != nullptr)
            search->PrintConnectionAttemptResult();
         arguments.clear();
      }

      std::string GetName() override
      {
         return "/connect";
      }
   };

   class Help: public Command<std::string>
   {
      std::unordered_map<std::string, std::string> helpMap = 
         {  {"/add","/add | Flags: -host - для запуска сервера на текущем компьютере пользователя, пустой - если нужно добавить сторонний сервер\n"},
            {"/erase", "/erase <ServerName>\n"},
            {"/find", "/find <flags> <ServerName/IP> | <flags>: -ip - для поиска по IP адресу сервера\n"},
            {"/getip:port", "/getip:port <ServerName>\n"},
            {"/info", "/info <ServerName>\n"},
            {"/print", "/print\n"},
            {"/exit", "/exit\n"},
            {"/connect", "/connect <ServerIPPort>(format: ip:port)"}};
   public:
      Help() { int i = 0;}
      void Execute (const std::string inputStruct) override
      {
         ParseStringIntoWords(arguments, inputStruct);
         
         if (arguments[1] == "-all")
         {  
            for (auto& kv : helpMap)
               std::cout << kv.second;
         } else {
            for (int i = 1; i < arguments.size(); i++)
            {
               if (helpMap.find(arguments[i]) != helpMap.end())
                  std::cout << helpMap[arguments[i]];
            }
         }
         arguments.clear();
      }
   
      std::string GetName() override
      {
         return "/help";
      }
   };

   class Exit: public Command<std::string>
   {
      std::unique_ptr<bool> exitCondition;
   public:
      Exit(bool* value)
      {
         exitCondition = std::unique_ptr<bool>(value);
      }
      
      void Execute (const std::string inputStruct) override
      {
         *exitCondition = true;
      }  

      std::string GetName() override
      {
         return "/exit";
      }
   };

//При добавлении новой команды, не забыть добавить её в вектор в функции IsNoArgumentsNeeded, если ей не нужны будут аргументы
}
