#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>
#include "server.h"
#include "database.h"
#include "commands.h"

bool isExit = false;

std::map<std::string, std::string> AddressesStorage::addresses;
std::unique_ptr<IPAddress> MyComputer::ip;

class Executable
{
   std::unordered_map<std::string, std::unique_ptr<Command<std::string>>> funcMap;

   template<class T, class D>
   void EmplaceCommand(D argument)
   {
      Command<std::string>* command = new T(argument);
      funcMap.insert(std::make_pair(command->GetName(), std::unique_ptr<Command<std::string>>(command)));
   }
   
   template<class T, class D>
   void EmplaceCommand(D* argument)
   {
      Command<std::string>* command = new T(argument);
      funcMap.insert(std::make_pair(command->GetName(), std::unique_ptr<Command<std::string>>(command)));
   }
   
   template<class T>
   void EmplaceCommand()
   {
      Command<std::string>* command = new T();
      funcMap.insert(std::make_pair(command->GetName(), std::unique_ptr<Command<std::string>>(command)));
   }
public:
   Executable(std::shared_ptr<ServerDatabase>& db)
   {
      EmplaceCommand<serversList::PrintList, std::shared_ptr<ServerDatabase>>(db);  
      EmplaceCommand<serversList::AddElement, std::shared_ptr<ServerDatabase>>(db);  
      EmplaceCommand<serversList::EraseElement, std::shared_ptr<ServerDatabase>>(db);  
      EmplaceCommand<serversList::FindElement, std::shared_ptr<ServerDatabase>>(db);  
      EmplaceCommand<serversList::GetIPPort, std::shared_ptr<ServerDatabase>>(db);  
      EmplaceCommand<serversList::Info, std::shared_ptr<ServerDatabase>>(db);  
      EmplaceCommand<serversList::Connect, std::shared_ptr<ServerDatabase>>(db);  
      EmplaceCommand<serversList::Help>();
      EmplaceCommand<serversList::Exit, bool>(&isExit);
   }
   
   void ExecuteCommand(const std::string& inputStr)
   {
      std::unique_ptr<MyComputer> myPC = std::unique_ptr<MyComputer>(new MyComputer());
      std::string commandName = inputStr.substr(0, inputStr.find(" "));
      
      auto search = funcMap.find(commandName);
      
      bool condition = serversList::IsAnyArguments(inputStr);
      serversList::IsNoArgumentsNeeded(condition, commandName);
      
      if (search != funcMap.end())
         if (condition)
            search->second->Execute(inputStr);
         else
            std::cout << "Invalid input. Call /help -all or /help <CommandName1> ... <CommandNameN>.\n";  
      else
         std::cout << "Unknown command!\n";
   }

};

class RuntimeUpdater
{
   std::thread thread;
   std::shared_ptr<ServerDatabase> db;
   
   void Routine()
   {
      int timer = 30;
      while(!isExit)
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
         timer--;
         if (timer <= 0)
         {
            db->UpdateDataInRuntime();
            timer = 30;
         }
      }
   }
public:
   RuntimeUpdater(std::shared_ptr<ServerDatabase>& db)
   {
      this->db = std::shared_ptr<ServerDatabase>(db);
      this->thread = std::thread(&RuntimeUpdater::Routine, this);
   }

   ~RuntimeUpdater()
   {
      this->thread.join();
   }
};

class InputHandler
{
public:
   void HandleInput(std::unique_ptr<Executable>& commands)
   {
      std::string inputStr;
      while(!isExit)
      {
         std::getline(std::cin, inputStr);
         commands->ExecuteCommand(inputStr);
      }      
   }
};

int main(int argc, char* argv[])
{
   std::shared_ptr<ServerDatabase> db = std::shared_ptr<ServerDatabase>(new ServerDatabase(10));   
   std::unique_ptr<Executable> commands = std::unique_ptr<Executable>(new Executable(db));  
   std::unique_ptr<RuntimeUpdater> updater = std::unique_ptr<RuntimeUpdater>(new RuntimeUpdater(db));
   std::unique_ptr<InputHandler> inputHandler = std::unique_ptr<InputHandler>(new InputHandler()); 
   
   inputHandler->HandleInput(commands);
   return 0;
}
