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

#include "common-tests.h"

int sdb_test_cmp(void *lhs, void *rhs) {
  int a = *((int *)lhs);
  int b = *((int *)rhs);
  return a < b ? -1 : a > b ? 1 : 0;
}

void sdb_test_search_empty(sdb_tests_context_t ctx) {
  int array[] = {};
  int elem = 13;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == -1, "Expected -1");
}

void sdb_test_search_left_out(sdb_tests_context_t ctx) {
  int array[] = {4, 5, 6, 7, 8, 111};
  int elem = 2;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 0, "Expected 0");
}

void sdb_test_search_right_out(sdb_tests_context_t ctx) {
  int array[] = {4, 5, 6, 7, 8, 111};
  int elem = 200;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 6, "Expected 6");
}

void sdb_test_search_left_approx(sdb_tests_context_t ctx) {
  int array[] = {4, 50, 60, 70, 80, 111};
  int elem = 55;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 2, "Expected 2");
}

void sdb_test_search_right_approx(sdb_tests_context_t ctx) {
  int array[] = {4, 50, 60, 70, 80, 111};
  int elem = 90;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 5, "Expected 5");
}

void sdb_test_search_exactly(sdb_tests_context_t ctx) {
  int array[] = {4, 50, 60, 70, 80, 111};
  int elem = 50;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 1, "Expected 1");
}

void sdb_test_search_even(sdb_tests_context_t ctx) {
  int array[] = {4, 50, 60, 70, 80, 111, 200, 1000};
  int elem = 50;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 1, "Expected 1");
}

void sdb_test_search_odd(sdb_tests_context_t ctx) {
  int array[] = {4, 50, 60, 70, 80, 111, 1000};
  int elem = 111;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 5, "Expected 5");
}

void sdb_test_search_duplicates(sdb_tests_context_t ctx) {
  int array[] = {1, 1, 1, 4, 4, 4, 4, 4};
  int elem = 3;
  sdb_assert(sdb_find(array, sizeof(int), sizeof(array) / sizeof(int), &elem, sdb_test_cmp) == 3, "Expected 3");
}
