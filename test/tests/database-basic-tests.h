/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef TEST_TESTS_DATABASE_BASIC_TESTS_H_
#define TEST_TESTS_DATABASE_BASIC_TESTS_H_

#include "test/tests/base-database-tests.h"

namespace shakadb {
namespace test {

class DatabaseBasicTests : public BaseDatabaseTests {
 public:
  void simple_database_initialization_test(TestContext ctx);
  void basic_database_write_and_read_all(TestContext ctx);
  void write_database_in_one_big_batch(TestContext ctx);
  void write_database_in_multiple_small_batches(TestContext ctx);
  void database_multi_write_and_read_all(TestContext ctx);
  void database_write_history(TestContext ctx);
  void database_write_close_and_write_more(TestContext ctx);
  void database_continuous_write(TestContext ctx);
  void database_continuous_write_with_pickup(TestContext ctx);
  void database_write_batch_size_equal_to_page_capacity(TestContext ctx);
  void database_write_batch_size_greater_than_page_capacity(TestContext ctx);
  void database_read_inside_single_chunk(TestContext ctx);
  void database_read_span_two_chunks(TestContext ctx);
  void database_read_span_three_chunks(TestContext ctx);
  void database_read_chunk_edges(TestContext ctx);
  void database_read_duplicated_values(TestContext ctx);
  void database_read_with_limit(TestContext ctx);
  void database_truncate(TestContext ctx);
  void database_truncate_multiple(TestContext ctx);
  void database_truncate_write_again(TestContext ctx);
};

}  // namespace test
}  // namespace shakadb

#endif  // TEST_TESTS_DATABASE_BASIC_TESTS_H_
