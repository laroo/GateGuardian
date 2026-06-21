#ifndef GATEGUARDIAN_WEBMANAGER_H
#define GATEGUARDIAN_WEBMANAGER_H

#include <WebServer.h>

extern WebServer webServer;

void setupWebServer(const char* clientId);
void loopWebServer();

#endif // GATEGUARDIAN_WEBMANAGER_H
