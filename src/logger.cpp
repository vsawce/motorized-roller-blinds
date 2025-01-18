#include "logger.h"

constexpr uint16_t  MICROSECONDS_IN_ONE_CENTISECOND = 10000;
constexpr uint8_t   CENTISECONDS_IN_ONE_SECOND      = 100;

uint32_t getUptimeCentiseconds(void)
{
    //pico-sdk hardware timer
    absolute_time_t current_time = get_absolute_time();
    
    return to_us_since_boot(current_time) / MICROSECONDS_IN_ONE_CENTISECOND;
}




//Logger class
Logger::Logger() {}

int Logger::init()
{
    stdio_init_all();
    printf("System initializing...\n");

    m_logQueue = xQueueCreate(LOG_QUEUE_SIZE, LOG_MESSAGE_MAX_LENGTH);
    if (m_logQueue == NULL) {
        printf("Failed to create log queue\n");
        return 1;
    }
    return 0;
}

void Logger::send(const char *message)
{
    if (m_logQueue != NULL) {
        if (xQueueSend(m_logQueue, message, pdMS_TO_TICKS(100)) != pdPASS) {
            // Handle queue full (optional, TODO?)
            //printf("Log queue full. Message dropped: %s\n", message); // Add this for debugging
        }
    }
}

void Logger::receive()
{
    char logBuffer[LOG_MESSAGE_MAX_LENGTH];

    // Wait for a log message from the queue
    if (xQueueReceive(m_logQueue, logBuffer, portMAX_DELAY) == pdPASS) {
        // Send the log message over USB
        printf("Log: %s\n", logBuffer);
    }
}
