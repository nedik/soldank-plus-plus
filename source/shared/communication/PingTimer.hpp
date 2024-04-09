#ifndef __PING_TIMER_HPP__
#define __PING_TIMER_HPP__

#include <chrono>
#include <cstdint>

namespace Soldank
{
class PingTimer
{
public:
    void Start();
    void Stop();
    void Update();

    std::uint16_t GetLastPingMeasure() const;
    bool IsRunning() const;
    bool IsOverThreshold() const;

private:
    constexpr const static double RUN_TIME_THRESHOLD_SECONDS = 9.999;

    std::chrono::time_point<std::chrono::system_clock> start_time_;
    std::uint16_t last_ping_;
    bool is_running_{};
};
} // namespace Soldank

#endif
