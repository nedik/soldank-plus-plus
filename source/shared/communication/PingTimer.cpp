#include "communication/PingTimer.hpp"

namespace Soldank
{
void PingTimer::Start()
{
    start_time_ = std::chrono::system_clock::now();
    is_running_ = true;
}

void PingTimer::Stop()
{
    is_running_ = false;
    Update();
}

void PingTimer::Update()
{
    auto current_time = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = (current_time - start_time_);
    last_ping_ = (std::uint16_t)(diff.count() * 1000.0);
    std::uint16_t run_time_threshold_milliseconds = RUN_TIME_THRESHOLD_SECONDS * 1000.0;
    if (last_ping_ > run_time_threshold_milliseconds) {
        last_ping_ = run_time_threshold_milliseconds;
    }
}

std::uint16_t PingTimer::GetLastPingMeasure() const
{
    return last_ping_;
}

bool PingTimer::IsRunning() const
{
    return is_running_;
}

bool PingTimer::IsOverThreshold() const
{
    auto current_time = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = (current_time - start_time_);
    return diff.count() > RUN_TIME_THRESHOLD_SECONDS;
}
} // namespace Soldank
