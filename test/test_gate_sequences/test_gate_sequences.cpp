#include <unity.h>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include "../GateTestHarness.h"
#include "../SequenceParser.h"
#include "../../src/gate.cpp"

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::string gSequenceDir;

static void runSequence(const std::string& csvPath) {
    std::vector<SequenceRow> rows;
    try {
        rows = parseSequenceCSV(csvPath);
    } catch (const std::exception& e) {
        TEST_FAIL_MESSAGE(e.what());
        return;
    }

    GateTestHarness gate;
    // Seed time and initial sensors before initialize()
    if (!rows.empty()) {
        gate.setSimTime(rows[0].timestamp_ms);
        gate.setSensors(rows[0].sensors);
    }
    gate.initialize();

    // If row[0] carries a known state name, force that state directly.
    // This allows live recordings that start mid-sequence to bypass boot detection.
    if (!rows.empty()) {
        const std::string& s = rows[0].expectedState;
        if      (s == "CLOSED")  gate.forceState(GATE_CLOSED);
        else if (s == "OPEN")    gate.forceState(GATE_OPEN);
        else if (s == "OPENING") gate.forceState(GATE_OPENING);
        else if (s == "CLOSING") gate.forceState(GATE_CLOSING);
        else if (s == "UNKNOWN") gate.forceState(GATE_UNKNOWN);
    }

    for (size_t i = 1; i < rows.size(); ++i) {
        const auto& row = rows[i];
        gate.setSimTime(row.timestamp_ms);
        gate.setSensors(row.sensors);

        // Command markers: issue command then update; no state assertion on this row
        if (row.expectedState == "OPEN_CMD") {
            gate.openGate();
            gate.update();
            continue;
        }
        if (row.expectedState == "CLOSE_CMD") {
            gate.closeGate();
            gate.update();
            continue;
        }

        gate.update();

        if (!row.expectedState.empty()) {
            std::string actual = gate.getStateString().c_str();
            if (actual != row.expectedState) {
                char msg[256];
                snprintf(msg, sizeof(msg),
                    "[%s] t=%lu ms: expected '%s' but got '%s'",
                    csvPath.c_str(),
                    row.timestamp_ms,
                    row.expectedState.c_str(),
                    actual.c_str());
                TEST_FAIL_MESSAGE(msg);
                return;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Discover and register one Unity test function per CSV file.
// PlatformIO/Unity does not support dynamic test registration, so we use
// a fixed list that mirrors the files in test/sequences/.
// ---------------------------------------------------------------------------

void test_boot_closed() {
    runSequence(gSequenceDir + "/boot_closed.csv");
}

void test_boot_open() {
    runSequence(gSequenceDir + "/boot_open.csv");
}

void test_open_cycle() {
    runSequence(gSequenceDir + "/open_cycle.csv");
}

void test_close_cycle() {
    runSequence(gSequenceDir + "/close_cycle.csv");
}

void test_manual_close() {
    runSequence(gSequenceDir + "/manual_close.csv");
}

void test_full_open_cycle() {
    runSequence(gSequenceDir + "/full_open_cycle.csv");
}

void test_full_close_cycle() {
    runSequence(gSequenceDir + "/full_close_cycle.csv");
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
static const char* gTestFilter = nullptr;

#define RUN_TEST_FILTERED(fn) do { \
    if (!gTestFilter || std::string(#fn).find(gTestFilter) != std::string::npos) \
        RUN_TEST(fn); \
} while(0)

int main(int argc, char** argv) {
    gSequenceDir = "test/sequences";
    gTestFilter  = std::getenv("TEST_FILTER");

    UNITY_BEGIN();
    RUN_TEST_FILTERED(test_boot_closed);
    RUN_TEST_FILTERED(test_boot_open);
    RUN_TEST_FILTERED(test_open_cycle);
    RUN_TEST_FILTERED(test_close_cycle);
    RUN_TEST_FILTERED(test_manual_close);
    RUN_TEST_FILTERED(test_full_open_cycle);
    RUN_TEST_FILTERED(test_full_close_cycle);
    return UNITY_END();
}

void setUp()    {}
void tearDown() {}
