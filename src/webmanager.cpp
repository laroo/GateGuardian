#include "Arduino.h"
#include "webmanager.h"
#include <ElegantOTA.h>
#include "config.h"
#include "gate.h"

extern Gate* gate;

WebServer webServer(80);

static const char* _clientId = nullptr;

void setupWebServer(const char* clientId) {
  _clientId = clientId;

  webServer.on("/", []() {
    unsigned long uptimeSec = millis() / 1000;
    unsigned long h = uptimeSec / 3600;
    unsigned long m = (uptimeSec % 3600) / 60;
    unsigned long s = uptimeSec % 60;
    char uptimeStr[12];
    snprintf(uptimeStr, sizeof(uptimeStr), "%02lu:%02lu:%02lu", h, m, s);
    String gateState = gate ? gate->getStateString() : "unknown";

    String html = F(
      "<!DOCTYPE html>"
      "<html lang=\"en\"><head>"
      "<meta charset=\"UTF-8\">"
      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
      "<title>GateGuardian</title>"
      "<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css\">"
      "<script src=\"https://unpkg.com/htmx.org@2.0.4\"></script>"
      "</head><body>"
      "<main class=\"container\">"
      "<h1>GateGuardian</h1>"
      "<article>"
      "<table>"
      "<tbody>"
      "<tr><th>Client ID</th><td id=\"clientId\">");
    html += _clientId;
    html += F("</td></tr>"
      "<tr><th>Uptime</th><td id=\"uptime\">");
    html += uptimeStr;
    html += F("</td></tr>"
      "<tr><th>Gate State</th><td id=\"gateState\">");
    html += gateState;
    html += F("</td></tr>"
      "<tr><th>In Motion</th><td id=\"inMotion\">");
    html += (gate && gate->isInMotion()) ? "Yes" : "No";
    html += F("</td></tr>"
      "</tbody>"
      "</table>"
      "<div hx-get=\"/status\" hx-trigger=\"every 1s\" hx-swap=\"none\" hx-on::after-request=\""
        "var d=JSON.parse(event.detail.xhr.responseText);"
        "document.getElementById('clientId').textContent=d.clientId;"
        "document.getElementById('uptime').textContent=d.uptime;"
        "document.getElementById('gateState').textContent=d.gateState;"
        "document.getElementById('inMotion').textContent=d.inMotion?'Yes':'No';"
      "\"></div>"
      "</article>"
      "<article>"
      "<h2>Controls</h2>"
      "<div style=\"display:flex;gap:1rem;\">"
      "<button hx-get=\"/gate/open\" hx-swap=\"none\">Open</button>"
      "<button hx-get=\"/gate/close\" hx-swap=\"none\" class=\"secondary\">Close</button>"
      "</div>"
      "</article>"
      "</main>"
      "</body></html>"
    );
    webServer.send(200, "text/html", html);
  });
  webServer.on("/status", []() {
    unsigned long uptimeSec = millis() / 1000;
    unsigned long h = uptimeSec / 3600;
    unsigned long m = (uptimeSec % 3600) / 60;
    unsigned long s = uptimeSec % 60;
    char uptimeStr[12];
    snprintf(uptimeStr, sizeof(uptimeStr), "%02lu:%02lu:%02lu", h, m, s);
    String gateState = gate ? gate->getStateString() : "unknown";
    String json = "{\"clientId\":\"";
    json += _clientId;
    json += "\",\"uptime\":\"";
    json += uptimeStr;
    json += "\",\"gateState\":\"";
    json += gateState;
    json += "\",\"inMotion\":";
    json += (gate && gate->isInMotion()) ? "true" : "false";
    json += "}";
    webServer.send(200, "application/json", json);
  });
  webServer.on("/gate/close", []() {
    gate->closeGate();
    webServer.send(200, "text/plain", "Gate closing...");
  });
  webServer.on("/gate/open", []() {
    gate->openGate();
    webServer.send(200, "text/plain", "Gate opening...");
  });
  webServer.on("/gate/stop", []() {
    gate->stopGate();
    webServer.send(200, "text/plain", "Gate stopping...");
  });
  ElegantOTA.begin(&webServer, OTA_USERNAME_STR, OTA_PASSWORD_STR);
  webServer.begin();
  Serial.println("[INIT] HTTP server started");
}

void loopWebServer() {
  webServer.handleClient();
  ElegantOTA.loop();
}
