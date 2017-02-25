//
// Created by Pawel Burzynski on 20/02/2017.
//

#ifndef SHAKADB_CONFIGURATION_H
#define SHAKADB_CONFIGURATION_H

#include <string>
#include <map>
namespace shakadb {

class Configuration {
 public:
  static Configuration *Load(std::string config_file);

  std::string GetLogFile();
  int GetServerPort();
  int GetServerBacklog();
  std::string GetDbFolder();
  int GetDbPointsPerChunk();
 private:
  Configuration();

  int ReadAsInt(std::string key, int defaultValue);
  std::string ReadAsString(std::string key, std::string defaultValue);

  std::map<std::string, std::string> config;
};

}

#endif //SHAKADB_CONFIGURATION_H
