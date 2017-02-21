//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_MONITOR_TESTS_H
#define SHAKADB_MONITOR_TESTS_H

#include <test/framework/test-context.h>
#include <src/utils/monitor.h>
#include <src/utils/thread.h>
#include <list>
#include <vector>
#include <test/framework/assert.h>

namespace shakadb {
namespace test {

class MonitorTests {
 public:
  void create_delete_test(TestContext ctx) {
    Monitor *monitor = new Monitor();
    delete monitor;
  };

  void enter_test(TestContext ctx) {
    Monitor monitor;

    { auto scope2 = monitor.Enter(); }
    auto scope1 = monitor.Enter();
  };

  void enter_two_threads_test(TestContext ctx) {
    Monitor *monitor = new Monitor();
    std::vector<int> *result = new std::vector<int>();

    Thread worker([monitor, result](void *data) -> void {
      auto scope = monitor->Enter();
      scope->Wait();
      result->push_back(1);
    }, nullptr);

    worker.Start(nullptr);
    Thread::Sleep(100);
    auto scope = monitor->Enter();
    result->push_back(0);
    scope->Signal();
    scope->Exit();

    worker.Join();

    Assert::IsTrue(result->size() == 2);
    Assert::IsTrue(result->at(0) == 0);
    Assert::IsTrue(result->at(1) == 1);

    delete result;
    delete monitor;
  };
};

}
}

#endif //SHAKADB_MONITOR_TESTS_H
