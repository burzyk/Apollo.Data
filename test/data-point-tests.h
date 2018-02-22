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

#ifndef TEST_DATA_POINT_TESTS_H_
#define TEST_DATA_POINT_TESTS_H_

#include "test/framework.h"

void test_search_empty(test_context_t ctx);
void test_search_left_out(test_context_t ctx);
void test_search_right_out(test_context_t ctx);
void test_search_left_approx(test_context_t ctx);
void test_search_right_approx(test_context_t ctx);
void test_search_exactly(test_context_t ctx);
void test_search_even(test_context_t ctx);
void test_search_odd(test_context_t ctx);
void test_search_duplicates(test_context_t ctx);

#endif  // TEST_DATA_POINT_TESTS_H_
