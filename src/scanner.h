#pragma once

#include <Arduino.h>

typedef void (*tScanner_callback)();

void scanner_scan(tScanner_callback callback);
