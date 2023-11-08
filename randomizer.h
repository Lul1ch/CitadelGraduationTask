#pragma once
#include <string>
#include <vector>

constexpr int minPort = 1;
constexpr int maxPort = 65535;
constexpr int minServerCapacity = 1;
constexpr int maxServerCapacity = 99;
constexpr int minPing = 10;
constexpr int maxPing = 191;
constexpr int deltaPing = 5;
constexpr int deltaPlayers = 5;

std::string RandomizeString(std::vector<std::string>& stringsVec)
{
   int iterationsNumber = rand() % 5 + 1;
   std::string str;
   for (int i = 0; i < iterationsNumber; i++)
   {
      int randIndex = rand() % stringsVec.size();
      str += stringsVec[randIndex];
   }
   return str;
}

int RandomizeNumber(int lowerBorder, int higherBorder)
{
   return rand() % higherBorder + lowerBorder;
}

bool RandomizeBool()
{
   return (bool)(rand() % 2);
}
