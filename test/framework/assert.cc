//
// Created by Pawel Burzynski on 19/01/2017.
//

#include "assert.h"
#include "validation-exception.h"

namespace apollo {
namespace test {

void Assert::IsTrue(bool condition) {
  if (!condition) {
    throw ValidationException("The expression is not true");
  }
}

}
}