#include "init.h"

int init_wifi_led()
{
    return wifi.init();
}

int init_logger(Logger *log_ptr)
{
    return log_ptr->init();
}
