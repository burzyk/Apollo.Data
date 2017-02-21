//
// Created by Pawel Burzynski on 12/02/2017.
//

#ifndef SHAKADB_STORAGE_RING_BUFFER_TESTS_H
#define SHAKADB_STORAGE_RING_BUFFER_TESTS_H

#include <test/framework/test-context.h>
#include <src/utils/ring-buffer.h>
#include <test/framework/assert.h>

namespace shakadb {
namespace test {

class RingBufferTests {
 public:
  void create_delete_test(TestContext ctx) {
    RingBuffer *buffer = new RingBuffer(5);
    delete buffer;
  };

  void empty_read_test(TestContext ctx) {
    RingBuffer buffer(5);
    byte_t data[255] = {0};

    Assert::IsTrue(buffer.Read(data, 20) == 0);
    Assert::IsTrue(buffer.Read(data, 20) == 0);
    Assert::IsTrue(buffer.Read(data, 20) == 0);
    Assert::IsTrue(buffer.Read(data, 20) == 0);
    Assert::IsTrue(buffer.Read(data, 20) == 0);
  };

  void empty_peek_test(TestContext ctx) {
    RingBuffer buffer(5);
    byte_t data[255] = {0};

    Assert::IsTrue(buffer.Peek(data, 20) == 0);
    Assert::IsTrue(buffer.Read(data, 20) == 0);
    Assert::IsTrue(buffer.Peek(data, 20) == 0);
    Assert::IsTrue(buffer.Read(data, 20) == 0);
    Assert::IsTrue(buffer.Peek(data, 20) == 0);
  };

  void simple_write_test(TestContext ctx) {
    RingBuffer buffer(5);
    const char *data = "ala ma kota";

    buffer.Write((byte_t *)data, 11);

    Assert::IsTrue(buffer.GetSize() == 11);
  };

  void multiple_write_with_peerk_and_read_test(TestContext ctx) {
    RingBuffer buffer(5);
    const char *data_1 = "ala ma kota";
    const char *data_2 = "ola ma asa";
    byte_t result[255] = {0};

    buffer.Write((byte_t *)data_1, 11);
    buffer.Write((byte_t *)data_2, 10);
    Assert::IsTrue(buffer.GetSize() == 11 + 10);

    memset(result, 0, 255);
    buffer.Peek(result, 11);
    Assert::IsTrue(buffer.GetSize() == 11 + 10);
    Assert::IsTrue(strcmp((const char *)result, data_1) == 0);

    memset(result, 0, 255);
    buffer.Read(result, 11);
    Assert::IsTrue(buffer.GetSize() == 10);
    Assert::IsTrue(strcmp((const char *)result, data_1) == 0);

    memset(result, 0, 255);
    int read_count = buffer.Read(result, 100);
    Assert::IsTrue(read_count == 10);
    Assert::IsTrue(buffer.GetSize() == 0);
    Assert::IsTrue(strcmp((const char *)result, data_2) == 0);
  };

  void multiple_write_hitting_limit_test(TestContext ctx) {
    RingBuffer buffer(5);
    const char *data_1 = "ala ma kota";
    const char *data_2 = " ola ma";
    byte_t result[255] = {0};

    buffer.Write((byte_t *)data_1, 11);

    buffer.Read(result, 6);
    Assert::IsTrue(buffer.GetSize() == 5);
    Assert::IsTrue(strcmp((const char *)result, "ala ma") == 0);

    buffer.Write((byte_t *)data_2, 7);

    memset(result, 0, 255);
    int read_count = buffer.Read(result, 100);
    Assert::IsTrue(read_count == 12);
    Assert::IsTrue(buffer.GetSize() == 0);
    Assert::IsTrue(strcmp((const char *)result, " kota ola ma") == 0);
  };

  void multiple_write_and_read_loop_test(TestContext ctx) {
    RingBuffer buffer(5);
    byte_t input[10000] = {0};
    byte_t result[10000] = {0};
    byte_t expected[10000] = {0};

    for (int i = 0; i < 1000; i++) {
      input[i] = i % 100;
    }

    for (int i = 0; i < 1000; i++) {
      buffer.Write(input + i, 7);
      buffer.Read(result, 6);

      memcpy(expected + i * 7, input + i, 7);

      Assert::IsTrue(memcmp(result, expected + i * 6, 6) == 0);
    }

    Assert::IsTrue(buffer.GetSize() == 1000);
  };
};

}
}

#endif //SHAKADB_STORAGE_RING_BUFFER_TESTS_H
