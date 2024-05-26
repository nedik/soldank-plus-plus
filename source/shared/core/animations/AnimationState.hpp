#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "core/animations/AnimationData.hpp"

#include "core/math/Glm.hpp"

#include <vector>
#include <memory>
#include <optional>

namespace Soldank
{
struct Soldier;

class AnimationState
{
public:
    AnimationState(std::shared_ptr<const AnimationData> animation_data);
    AnimationState(const AnimationState& other) = default;
    virtual ~AnimationState() = default;

    void DoAnimation();
    const glm::vec2& GetPosition(unsigned int index) const;
    unsigned int GetFramesCount() const;
    bool IsAny(const std::vector<AnimationType>& animations) const;

    AnimationType GetType() const { return animation_data_->GetAnimationType(); };

    int GetSpeed() const { return speed_; }
    void SetSpeed(int new_speed) { speed_ = new_speed; }

    unsigned int GetFrame() const { return frame_; }
    void SetFrame(unsigned int new_frame) { frame_ = new_frame; }
    void SetNextFrame() { frame_++; }

    virtual void Enter(Soldier& soldier);
    virtual std::optional<std::shared_ptr<AnimationState>> HandleInput(Soldier& soldier);
    virtual void Update(Soldier& soldier);

protected:
    std::shared_ptr<const AnimationData> animation_data_;

    int speed_;
    int count_;
    unsigned int frame_;
};
} // namespace Soldank

#endif
