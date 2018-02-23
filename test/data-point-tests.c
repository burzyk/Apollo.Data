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
  points_list_t list = {.points = array, .count = 0, .point_size = 4};
  sdb_assert(data_point_find(&list, 13) == NULL, "Expected NULL");
}

void test_search_left_out(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=5}, {.time=6}, {.time=7}, {.time=8}, {.time=111}};
  points_list_t list = {.points = array, .count = 6, .point_size = 12};
  sdb_assert(data_point_find(&list, 2)->time == 4, "Expected index 4");
}

void test_search_right_out(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=5}, {.time=6}, {.time=7}, {.time=8}, {.time=111}};
  points_list_t list = {.points = array, .count = 6, .point_size = 12};
  sdb_assert(data_point_find(&list, 200) == data_point_at(&list, 6), "Expected right boundary");
}

void test_search_left_approx(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}};
  points_list_t list = {.points = array, .count = 6, .point_size = 12};
  sdb_assert(data_point_find(&list, 55)->time == 60, "Expected 60");
}

void test_search_right_approx(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}};
  points_list_t list = {.points = array, .count = 6, .point_size = 12};
  sdb_assert(data_point_find(&list, 90)->time == 111, "Expected 111");
}

void test_search_exactly(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}};
  points_list_t list = {.points = array, .count = 6, .point_size = 12};
  sdb_assert(data_point_find(&list, 50)->time == 50, "Expected 50");
}

void test_search_even(test_context_t ctx) {
  data_point_t
      array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}, {.time=200}, {.time=1000}};
  points_list_t list = {.points = array, .count = 8, .point_size = 12};
  sdb_assert(data_point_find(&list, 50)->time == 50, "Expected 50");
}

void test_search_odd(test_context_t ctx) {
  data_point_t array[] = {{.time=4}, {.time=50}, {.time=60}, {.time=70}, {.time=80}, {.time=111}, {.time=1000}};
  points_list_t list = {.points = array, .count = 7, .point_size = 12};
  sdb_assert(data_point_find(&list, 111)->time == 111, "Expected 111");
}

void test_search_duplicates(test_context_t ctx) {
  data_point_t array[] = {{.time=1}, {.time=1}, {.time=1}, {.time=4}, {.time=4}, {.time=4}, {.time=4}, {.time=4}};
  points_list_t list = {.points = array, .count = 8, .point_size = 12};
  sdb_assert(data_point_find(&list, 3) == data_point_at(&list, 3), "Expected element at index 3");
}
