//
// Created by Pawel Burzynski on 21/02/2017.
//

#ifndef SHAKADB_END_TO_END_H
#define SHAKADB_END_TO_END_H

#include <src/utils/stopwatch.h>
#include <test/framework/test-context.h>
#include <src/bootstrapper.h>

namespace shakadb {
namespace test {

Bootstrapper *e2e_init(TestContext ctx) {
  std::string config_file_name = ctx.GetWorkingDirectory() + "/server.cfg";
  std::string config = "db.folder = " + ctx.GetWorkingDirectory() + "/data";
  File f(config_file_name);

  f.Write((byte_t *)config.c_str(), config.size());
  f.Flush();

  return Bootstrapper::Run(config_file_name);
}

Stopwatch e2e_initial_write(TestContext ctx) {
  return Stopwatch();
}

}
}

#endif //SHAKADB_END_TO_END_H
