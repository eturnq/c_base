#pragma once

#include "test.h"

TestResult *basic_linear_alloc_init_deinit(TestResult*);
TestResult *basic_linear_alloc_alloc_free(TestResult*);
TestResult *basic_linear_alloc_freeall(TestResult*);
TestResult *basic_linear_alloc_clone(TestResult*);

TestResult *linear_alloc_init_deinit(TestResult*);
TestResult *linear_alloc_alloc_free(TestResult*);
TestResult *linear_alloc_freeall(TestResult*);
