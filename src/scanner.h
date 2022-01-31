#pragma once

#include <Arduino.h>
#include "terminal.h"
#include "config.h"

typedef void (*tScanner_callback)();

void scanner_set_terminal_and_config(Terminal *term, Config *conf);
void scanner_scan(tScanner_callback callback);
