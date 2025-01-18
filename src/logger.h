#ifndef LOGGER_H //Prevent multiple definitions of same header
#define LOGGER_H

#include <stdio.h>

#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"  // Add this line


// Logging definitions
#define LOG_QUEUE_SIZE 10           // Maximum number of messages in the queue
#define LOG_MESSAGE_MAX_LENGTH 64   // Maximum length of each log message

uint32_t getUptimeCentiseconds(void); //Centiseconds should last ~1.36 years with uint32_t

class Logger
{
    public:
        Logger();
        int init();
        void send(const char *message);
        void receive();

    private:
        QueueHandle_t m_logQueue; // Log queue (alr a ptr)
};

#endif