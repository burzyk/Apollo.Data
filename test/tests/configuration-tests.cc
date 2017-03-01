//
// Created by Pawel Burzynski on 01/03/2017.
//

#include "test/tests/configuration-tests.h"

namespace shakadb {
namespace test {

void ConfigurationTests::init_test(TestContext ctx) {
  Configuration *config = Configuration::Load("/dummy_file");

  Assert::IsTrue(config->GetServerBacklog() == 10);

  delete config;
}

void ConfigurationTests::full_test(TestContext ctx) {
  const char *config_raw = "# this is an example configuration file\n"
      "# all values here are defaults\n"
      "log.file = some_log\n"
      "server.port = 1010\n"
      "server.backlog = 99\n"
      "db.folder = some_bad_folder\n"
      "\n"
      "# Number of data points per chunk in data file\n"
      "db.points_per_chunk = 2\n"
      "\n"
      "# Size of the buffer used when receiving the data points\n"
      "# for insertion\n"
      "write_handler.buffer_size = 4\n"
      "\n"
      "# Size of the buffer used when sending the data points\n"
      "# to the client\n"
      "read_handler.buffer_size = 7"
      "";

  std::string config_file = ctx.GetWorkingDirectory() + "/config.cfg";
  File *f = new File(config_file);
  f->Write((void *)config_raw, strlen(config_raw));
  delete f;

  Configuration *config = Configuration::Load(config_file);

  Assert::IsTrue(config->GetLogFile() == "some_log");
  Assert::IsTrue(config->GetServerPort() == 1010);
  Assert::IsTrue(config->GetServerBacklog() == 99);
  Assert::IsTrue(config->GetDbFolder() == "some_bad_folder");
  Assert::IsTrue(config->GetDbPointsPerChunk() == 2);

  delete config;
}

}  // namespace test
}  // namespace shakadb
