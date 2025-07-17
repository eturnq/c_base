#pragma once

#include "test.h"

TestResult *heap_allocation(TestResult *result);
TestResult *heap_reallocation(TestResult *result);
TestResult *heap_freeall_should_fail(TestResult *result);
TestResult *heap_clone(TestResult *result);
TestResult *heap_slice_split(TestResult *result);
