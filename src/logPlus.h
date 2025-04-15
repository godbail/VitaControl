#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>

#define LOG_PATH "ux0:dump/"
#define LOG_FILE LOG_PATH "VitaControl_log.txt"
// #define LOG_PATH "ux0:/myplugin_log.txt"

void log_start();
void log_write(const char *buffer, size_t length);

#ifndef RELEASE
#define LOG(...)                                                       \
	do                                                                 \
	{                                                                  \
		char buffer_Update[256];                                       \
		snprintf(buffer_Update, sizeof(buffer_Update), ##__VA_ARGS__); \
		log_write(buffer_Update, strlen(buffer_Update));               \
	} while (0)
#else
#define LOG(...) (void)0
#endif

#endif