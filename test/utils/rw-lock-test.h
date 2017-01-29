//
// Created by Pawel Burzynski on 29/01/2017.
//

#ifndef APOLLO_STORAGE_RW_LOCK_TEST_H
#define APOLLO_STORAGE_RW_LOCK_TEST_H

#include <src/utils/rw-lock.h>
#include <memory>
namespace apollo {
namespace test {

void rwlock_double_read_lock_test(TestContext ctx) {
  RwLock lock;

  auto s1 = lock.LockRead();
  auto s2 = lock.LockRead();
}

void rwlock_upgrade_lock_test(TestContext ctx) {
  RwLock lock;

  auto s1 = lock.LockRead();
  s1->UpgradeToWrite();
}

void rwlock_release_and_lock_again_test(TestContext ctx) {
  RwLock lock;

  auto s1 = lock.LockRead();

  // Scope
  {
    auto s2 = lock.LockRead();
  }

  s1->UpgradeToWrite();
}

}
}

#endif //APOLLO_STORAGE_RW_LOCK_TEST_H
