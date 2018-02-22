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

#include "test/data-point-tests.h"

#include "src/data-point.h"

void test_search_empty(test_context_t ctx) {
  data_point_t array[] = {};
  data_point_t elem = {.time=13};
  sdb_assert(data_point_find(array, 0, elem) == NULL, "Expected NULL");
}

void test_search_left_out(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=5}, {.time=6}, {.time=7}, {.time=8}, {.time=111}};
  data_point_t elem = {.time=2};
  sdb_assert(data_point_find(array, 6, elem) == array, "Expected index 0");
}

void test_search_right_out(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=5}, {.time=6}, {.time=7}, {.time=8}, {.time=111}};
  data_point_t elem = {.time=200};
  sdb_assert(data_point_find(array, 6, elem) == array + 6, "Expected 6");
}

void test_search_left_approx(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}};
  data_point_t elem = {.time=55};
  sdb_assert(data_point_find(array, 6, elem) == array + 2, "Expected 2");
}

void test_search_right_approx(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}};
  data_point_t elem = {.time=90};
  sdb_assert(data_point_find(array, 6, elem) == array + 5, "Expected 5");
}

void test_search_exactly(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}};
  data_point_t elem = {.time=50};
  sdb_assert(data_point_find(array, 6, elem) == array + 1, "Expected 1");
}

void test_search_even(test_context_t ctx) {
  data_point_t
      array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}, {.time=200}, {.time=1000}};
  data_point_t elem = {.time=50};
  sdb_assert(data_point_find(array, 8, elem) == array + 1, "Expected 1");
}

void test_search_odd(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}, {.time=1000}};
  data_point_t elem = {.time=111};
  sdb_assert(data_point_find(array, 7, elem) == array + 5, "Expected 5");
}

void test_search_duplicates(test_context_t ctx) {
  data_point_t array[] = {{.time=1}, {.time=1}, {.time=1}, {.time=4}, {.time=4}, {.time=4}, {.time=4}, {.time=4}};
  data_point_t elem = {.time=3};
  sdb_assert(data_point_find(array, 8, elem) == array + 3, "Expected 3");
}
