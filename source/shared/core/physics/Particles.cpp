#include "Particles.hpp"

#include "core/physics/Particles.hpp"
#include "core/utility/Getline.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <fstream>
#include <map>

namespace Soldank
{
Particle::Particle(bool _active,
                   glm::vec2 _position,
                   glm::vec2 _old_position,
                   glm::vec2 _velocity,
                   glm::vec2 force,
                   float one_over_mass,
                   float timestep,
                   float gravity,
                   float e_damping,
                   float v_damping)
    : active(_active)
    , position(_position)
    , old_position(_old_position)
    , velocity_(_velocity)
    , force_(force)
    , one_over_mass_(one_over_mass)
    , timestep_(timestep)
    , gravity_(gravity)
    , e_damping_(e_damping)
    , v_damping_(v_damping)
{
}

void Particle::Euler()
{
    old_position = position;
    force_.y += gravity_;
    glm::vec2 current_force = force_;
    current_force *= one_over_mass_ * std::pow(timestep_, 2);
    velocity_ += current_force;
    position += velocity_;
    velocity_ *= e_damping_;
    force_ = glm::vec2(0.0, 0.0);
}

void Particle::Verlet()
{
    glm::vec2 a = position * (1.0F + v_damping_);
    glm::vec2 b = old_position * v_damping_;

    old_position = position;
    force_.y += gravity_;
    glm::vec2 current_force = force_;
    current_force *= one_over_mass_ * std::pow(timestep_, 2);
    position = a - b + current_force;
    force_ = glm::vec2(0.0, 0.0);
}

ParticleSystem::ParticleSystem(const std::vector<Particle>& particles,
                               const std::vector<Constraint>& constraints)
    : particles_(particles)
    , constraints_(constraints)
{
}

void ParticleSystem::DoVerletTimestep()
{
    for (Particle& particle : particles_) {
        if (particle.active) {
            particle.Verlet();
        }
    }
    SatisfyConstraints();
}

void ParticleSystem::DoVerletTimestepFor(unsigned int particle_num, unsigned int constraint_num)
{
    particles_[particle_num - 1].Verlet();
    SatisfyConstraintFor(constraint_num - 1);
}

void ParticleSystem::DoEulerTimestep()
{
    for (Particle& particle : particles_) {
        if (particle.active) {
            particle.Euler();
        }
    }
}

void ParticleSystem::DoEulerTimestepFor(unsigned int particle_num)
{
    particles_[particle_num - 1].Euler();
}

void ParticleSystem::SatisfyConstraints()
{
    for (const Constraint& constraint : constraints_) {
        if (constraint.active) {
            SatisfyConstraint(constraint, particles_);
        }
    }
}

void ParticleSystem::SatisfyConstraintFor(unsigned int constraint_num)
{
    SatisfyConstraint(constraints_[constraint_num - 1], particles_);
}

void ParticleSystem::SatisfyConstraint(const Constraint& constraint,
                                       std::vector<Particle>& particles)
{
    unsigned int a = constraint.particle_num.x - 1;
    unsigned int b = constraint.particle_num.y - 1;

    glm::vec2 delta = particles[b].position - particles[a].position;
    auto length = glm::length(delta);

    if (length > 0.0) {
        auto diff = (length - constraint.rest_length) / length;

        // delta *= diff / 2.0f;
        if (particles[a].GetOneOverMass() > 0.0) {
            particles[a].position += delta * diff / 2.0F;
        }

        if (particles[b].GetOneOverMass() > 0.0) {
            particles[b].position -= delta * diff / 2.0F;
        }
    }
}

std::shared_ptr<ParticleSystem> ParticleSystem::Load(ParticleSystemType particle_system_type,
                                                     float scale,
                                                     const IFileReader& file_reader)
{
    // TODO: const
    const float grav = 0.06F;
    switch (particle_system_type) {
        case ParticleSystemType::Soldier: {
            // TODO: load it at the application start
            static std::map<float, std::shared_ptr<ParticleSystem>>
              soldier_particle_system_by_scale;
            if (!soldier_particle_system_by_scale.contains(scale)) {
                soldier_particle_system_by_scale[scale] =
                  LoadFromFile("gostek.po", scale, 1.0F, 1.06F * grav, 0.0F, 0.9945F, file_reader);
            }
            return std::make_shared<ParticleSystem>(*soldier_particle_system_by_scale.at(scale));
        }
        case ParticleSystemType::Flag: {
            static std::map<float, std::shared_ptr<ParticleSystem>> flag_particle_system_by_scale;
            if (!flag_particle_system_by_scale.contains(scale)) {
                flag_particle_system_by_scale[scale] =
                  LoadFromFile("flag.po", scale, 1.0F, 1.06F * grav, 0.0F, 0.9945F, file_reader);
            }
            return std::make_shared<ParticleSystem>(*flag_particle_system_by_scale.at(scale));
        }
        case ParticleSystemType::Weapon: {
            static std::map<float, std::shared_ptr<ParticleSystem>> weapon_particle_system_by_scale;
            if (!weapon_particle_system_by_scale.contains(scale)) {
                weapon_particle_system_by_scale[scale] =
                  LoadFromFile("karabin.po", scale, 1.0F, 1.06F * grav, 0.0F, 0.9945F, file_reader);
            }
            return std::make_shared<ParticleSystem>(*weapon_particle_system_by_scale.at(scale));
        }
        case ParticleSystemType::Kit: {
            static std::map<float, std::shared_ptr<ParticleSystem>> kit_particle_system_by_scale;
            if (!kit_particle_system_by_scale.contains(scale)) {
                kit_particle_system_by_scale[scale] =
                  LoadFromFile("kit.po", scale, 1.0F, 1.06F * grav, 0.0F, 0.989F, file_reader);
            }
            return std::make_shared<ParticleSystem>(*kit_particle_system_by_scale.at(scale));
        }
        case ParticleSystemType::Parachute: {
            static std::map<float, std::shared_ptr<ParticleSystem>>
              parachute_particle_system_by_scale;
            if (!parachute_particle_system_by_scale.contains(scale)) {
                parachute_particle_system_by_scale[scale] =
                  LoadFromFile("para.po", scale, 1.0F, 1.06F * grav, 0.0F, 0.9945F, file_reader);
            }
            return std::make_shared<ParticleSystem>(*parachute_particle_system_by_scale.at(scale));
        }
        case ParticleSystemType::StationaryGun: {
            static std::map<float, std::shared_ptr<ParticleSystem>>
              stat_gun_particle_system_by_scale;
            if (!stat_gun_particle_system_by_scale.contains(scale)) {
                stat_gun_particle_system_by_scale[scale] =
                  LoadFromFile("stat.po", scale, 1.0F, 1.06F * grav, 0.0F, 0.9945F, file_reader);
            }
            return std::make_shared<ParticleSystem>(*stat_gun_particle_system_by_scale.at(scale));
        }
    }
}

std::shared_ptr<ParticleSystem> ParticleSystem::LoadFromFile(const std::string& file_name,
                                                             float scale,
                                                             float timestep,
                                                             float gravity,
                                                             float e_damping,
                                                             float v_damping,
                                                             const IFileReader& file_reader)
{
    std::vector<Particle> particles;
    std::vector<Constraint> constraints;

    std::filesystem::path file_path = "objects/";
    file_path += file_name;
    auto file_data = file_reader.Read(file_path.string());
    if (!file_data.has_value()) {
        std::string message = "Could not open file: " + file_path.string();
        throw std::runtime_error(message.c_str());
    }
    std::stringstream data_buffer{ *file_data };

    auto read_float = [](std::stringstream& buffer) {
        std::string line;
        GetlineSafe(buffer, line);
        return std::stof(line);
    };

    std::string line;
    GetlineSafe(data_buffer, line);

    while (line != "CONSTRAINTS") {
        float x = read_float(data_buffer);
        read_float(data_buffer);
        float z = read_float(data_buffer);
        glm::vec2 p = glm::vec2(-x * scale / 1.2, -z * scale);

        particles.emplace_back(true,
                               p,
                               p,
                               glm::vec2(0.0, 0.0),
                               glm::vec2(0.0, 0.0),
                               1.0,
                               timestep,
                               gravity,
                               e_damping,
                               v_damping);

        GetlineSafe(data_buffer, line);
    }

    while (!data_buffer.eof()) {
        GetlineSafe(data_buffer, line);

        if (data_buffer.eof() || line.empty() || line == "ENDFILE") {
            break;
        }
        line.erase(0, 1); // first character is always P
        unsigned int pa_num = std::stoul(line);

        GetlineSafe(data_buffer, line);
        line.erase(0, 1); // first character is always P
        unsigned int pb_num = std::stoul(line);

        auto delta = particles[pa_num - 1].position - particles[pb_num - 1].position;
        constraints.push_back({ true, glm::uvec2(pa_num, pb_num), glm::length(delta) });
    }

    spdlog::info("Particle {}, loaded {} particles and {} constraints",
                 file_path.string(),
                 particles.size(),
                 constraints.size());

    return std::make_shared<ParticleSystem>(particles, constraints);
}
} // namespace Soldank
