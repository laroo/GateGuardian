#pragma once
#include <functional>
#include <vector>

// ---------------------------------------------------------------------------
// Minimal arduino-timer stub for native unit tests.
// Timers do NOT fire automatically; the harness drives time via _millis().
// Relay deactivation is tested implicitly through the safety-timeout path
// in Gate::update() (_relayActive + elapsed >= 500).
// ---------------------------------------------------------------------------
template<size_t max_tasks = 16, unsigned long (*time_func)() = nullptr>
class Timer {
public:
    struct Task {
        unsigned long fireAt;
        std::function<bool(void*)> cb;
        void* arg;
        bool active;
    };

    template<typename Handler>
    void in(unsigned long delay_ms, Handler handler, void* arg = nullptr) {
        _tasks.push_back({ delay_ms, handler, arg, true });
    }

    void tick() {
        // No-op in stub — relay deactivation is handled by the safety
        // timeout in Gate::update() which uses _millis() directly.
    }

    void cancel() { _tasks.clear(); }

private:
    std::vector<Task> _tasks;
};

template<size_t N = 16, unsigned long (*F)() = nullptr>
Timer<N, F> timer_create_default() { return {}; }
