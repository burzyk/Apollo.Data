//
// Created by Pawel Burzynski on 20/02/2017.
//

#ifndef SHAKADB_CONFIGURATION_TESTS_H
#define SHAKADB_CONFIGURATION_TESTS_H

#include <test/framework/test-context.h>
#include <src/configuration.h>
#include <test/framework/assert.h>
#include <src/utils/file.h>

namespace shakadb {
namespace test {

void configuration_init_test(TestContext ctx) {
  Configuration *config = Configuration::Load("/dummy_file");

  Assert::IsTrue(config->GetServerBacklog() == 10);

  delete config;
}

void configuration_full_test(TestContext ctx) {
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
  Assert::IsTrue(config->GetWriteHandlerBufferSize() == 4);
  Assert::IsTrue(config->GetReadHandlerBufferSize() == 7);

  delete config;
}

}
}

#endif //SHAKADB_CONFIGURATION_TESTS_H
