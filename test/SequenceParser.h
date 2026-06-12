#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "GateTestHarness.h"

struct SequenceRow {
    unsigned long timestamp_ms;
    SensorRow     sensors;
    std::string   expectedState;  // empty = no assertion
};

inline std::vector<SequenceRow> parseSequenceCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open sequence file: " + path);
    }

    std::vector<SequenceRow> rows;
    std::string line;
    bool headerSkipped = false;

    while (std::getline(file, line)) {
        // Skip blank lines and comments
        if (line.empty() || line[0] == '#') continue;

        if (!headerSkipped) {
            headerSkipped = true;
            continue;  // skip header row
        }

        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> fields;
        while (std::getline(ss, field, ',')) {
            // trim whitespace
            auto f = field.find_first_not_of(" \t\r\n");
            auto l = field.find_last_not_of(" \t\r\n");
            fields.push_back(f == std::string::npos ? "" : field.substr(f, l - f + 1));
        }

        if (fields.size() < 5) continue;  // malformed row

        SequenceRow row;
        row.timestamp_ms              = std::stoul(fields[0]);
        row.sensors.sensorLock         = fields[1] == "1";
        row.sensors.sensorLights       = fields[2] == "1";
        row.sensors.sensorPhotoEye     = fields[3] == "1";
        row.sensors.sensorExternalRelay = fields[4] == "1";
        row.expectedState              = fields.size() > 5 ? fields[5] : "";
        rows.push_back(row);
    }

    return rows;
}
