#ifndef __IWORLD_HPP__
#define __IWORLD_HPP__

#include "core/math/Glm.hpp"
#include "core/state/State.hpp"

#include <memory>
#include <functional>
#include <optional>

namespace Soldat
{
class IWorld
{
private:
    using TShouldStopGameLoopCallback = std::function<bool()>;
    using TPreGameLoopIterationCallback = std::function<void()>;
    using TPreWorldUpdateCallback = std::function<void()>;
    using TPostGameLoopIterationCallback =
      std::function<void(const std::shared_ptr<State>&, double, int)>;

public:
    virtual ~IWorld() = default;
    virtual void RunLoop(int fps_limit) = 0;
    virtual void Update(double delta_time) = 0;
    virtual const std::shared_ptr<State>& GetState() const = 0;
    virtual const Soldier& GetSoldier(unsigned int soldier_id) const = 0;

    virtual const Soldier& CreateSoldier(
      std::optional<unsigned int> force_soldier_id = std::nullopt) = 0;
    virtual glm::vec2 SpawnSoldier(unsigned int soldier_id,
                                   std::optional<glm::vec2> spawn_position = std::nullopt) = 0;

    virtual void UpdateFireButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateJetsButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateLeftButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateRightButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateJumpButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateCrouchButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateProneButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateChangeButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateThrowGrenadeButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateDropButtonState(unsigned int soldier_id, bool pressed) = 0;
    virtual void UpdateMousePosition(unsigned int soldier_id, glm::vec2 mouse_position) = 0;

    virtual void SetShouldStopGameLoopCallback(TShouldStopGameLoopCallback callback) = 0;
    virtual void SetPreGameLoopIterationCallback(TPreGameLoopIterationCallback callback) = 0;
    virtual void SetPreWorldUpdateCallback(TPreWorldUpdateCallback callback) = 0;
    virtual void SetPostGameLoopIterationCallback(TPostGameLoopIterationCallback callback) = 0;
};
} // namespace Soldat

#endif
