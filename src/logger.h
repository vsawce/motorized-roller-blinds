#ifndef LOGGER_H //Prevent multiple definitions of same header
#define LOGGER_H

#include <stdarg.h> // For va_list, va_start, va_end
#include <stdio.h>

#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"  // Add this line


// Logging definitions
#define LOG_QUEUE_SIZE 10           // Maximum number of messages in the queue
#define LOG_MESSAGE_MAX_LENGTH 64   // Maximum length of each log message

uint32_t log_getUptimeCentiseconds(void); //Centiseconds should last ~1.36 years with uint32_t

int log_init();
void log_send(const char *format, ...);
void log_receive();

#endif