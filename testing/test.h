#pragma once

#include <stdio.h>
#include <string.h>

enum test_status {
	TEST_PASS,
	TEST_FAIL
};

#define TEST_MESSAGE_SIZE 100
typedef struct {
	enum test_status status;
	char message[TEST_MESSAGE_SIZE];
} TestResult;

typedef TestResult *(*Test)(TestResult*);

#define INIT_RESULT(result, name) \
result->status = TEST_FAIL; \
sprintf(result->message, name)

#define MSG_PRINT(result, msg) sprintf(result->message + strlen(result->message), msg)
