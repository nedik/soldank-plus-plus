#include "core/animations/AnimationState.hpp"

#include "core/entities/Soldier.hpp"

#include <utility>
#include <algorithm>

namespace Soldank
{
AnimationState::AnimationState(std::shared_ptr<const AnimationData> animation_data)
    : animation_data_(std::move(animation_data))
    , speed_(animation_data_->GetSpeed())
    , count_(0)
    , frame_(1)
{
}

void AnimationState::DoAnimation()
{
    count_ += 1;

    if (count_ == speed_) {
        count_ = 0;
        frame_ += 1;

        if (frame_ > GetFramesCount()) {
            if (animation_data_->GetLooped()) {
                frame_ = 1;
            } else {
                frame_ = GetFramesCount();
            }
        }
    }
}

const glm::vec2& AnimationState::GetPosition(unsigned int index) const
{
    return animation_data_->GetFrames().at(frame_ - 1).positions.at(index - 1);
}

unsigned int AnimationState::GetFramesCount() const
{
    return animation_data_->GetFrames().size();
}

bool AnimationState::IsAny(const std::vector<AnimationType>& animations) const
{
    return std::ranges::any_of(animations, [this](AnimationType animation_type) {
        return animation_type == animation_data_->GetAnimationType();
    });
}

void AnimationState::HandleInput(Soldier& soldier){
    // TODO: Delete this after migration is finished
};

void AnimationState::Update(Soldier& soldier)
{
    // TODO: Delete this after migration is finished
}
} // namespace Soldank
