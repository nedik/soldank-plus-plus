#ifndef __WORLD_HPP__
#define __WORLD_HPP__

namespace Soldat::World
{
void Init();
void Update(double delta_time);
void Render(double frame_percent);
void Free();
} // namespace Soldat::World

#endif
