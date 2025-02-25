#include "logger.h"

constexpr uint16_t  MICROSECONDS_IN_ONE_CENTISECOND = 10000;
constexpr uint8_t   CENTISECONDS_IN_ONE_SECOND      = 100;

constexpr char      DBG_STR[4] = "DBG";
constexpr char      ERR_STR[4] = "ERR";

QueueHandle_t logQueue; // Log queue (alr a ptr)

uint32_t log_getUptimeCentiseconds(void)
{
    //pico-sdk hardware timer
    absolute_time_t current_time = get_absolute_time();
    
    return to_us_since_boot(current_time) / MICROSECONDS_IN_ONE_CENTISECOND;
}

int log_init()
{
    stdio_init_all();
    printf("System initializing...\n");

    logQueue = xQueueCreate(LOG_QUEUE_SIZE, LOG_MESSAGE_MAX_LENGTH);
    if (logQueue == NULL) {
        printf("Failed to create log queue\n");
        return 1;
    }
    return 0;
}

void log_send(LogType lt, const char *format, ...)
{
    if (logQueue == NULL) {
        return; //Exit if not initialized
    }

    if (!LOG_ENABLE_DEBUG && lt == LogType::DEBUG) {
        return; //Debug messages not enabled
    }

    // Buffers for the timestamp and the formatted message
    char fullMessage[LOG_MESSAGE_MAX_LENGTH];
    char formattedMessage[LOG_MESSAGE_MAX_LENGTH - 11]; // Reserve space 11 spaces for timestamp? 10 digs for uint32_t, 1 for decimal 

    uint32_t uptimeCentiseconds = log_getUptimeCentiseconds();

    // Process variable arguments
    va_list args;
    va_start(args, format);
    vsnprintf(formattedMessage, LOG_MESSAGE_MAX_LENGTH, format, args);
    va_end(args);

    // Prepend the timestamp to the log message
    if (lt == LogType::STANDARD) {      //Standard message
        snprintf(fullMessage, sizeof(fullMessage), "[%u.%02u] %s", uptimeCentiseconds / 100, uptimeCentiseconds % 100, formattedMessage);
    }
    else if (lt == LogType::DEBUG) {    //Debug
        snprintf(fullMessage, sizeof(fullMessage), "[%u.%02u] [%s] %s", uptimeCentiseconds / 100, uptimeCentiseconds % 100, DBG_STR, formattedMessage);
    }
    else {                              //Error
        snprintf(fullMessage, sizeof(fullMessage), "[%u.%02u] [%s] %s", uptimeCentiseconds / 100, uptimeCentiseconds % 100, ERR_STR, formattedMessage);
    }

    if (xQueueSend(logQueue, fullMessage, pdMS_TO_TICKS(100)) != pdPASS) {
        // Handle queue full (optional, TODO?)
        //printf("Log queue full. Message dropped: %s\n", message); // Add this for debugging
    }
}

void log_receive()
{
    char logBuffer[LOG_MESSAGE_MAX_LENGTH];

    // Wait for a log message from the queue
    if (xQueueReceive(logQueue, logBuffer, portMAX_DELAY) == pdPASS) {
        // Send the log message over USB
        printf("%s\n", logBuffer);
    }
}
