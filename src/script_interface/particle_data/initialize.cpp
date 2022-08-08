/*
 * Copyright (C) 2022 The ESPResSo project
 *
 * This file is part of ESPResSo.
 *
 * ESPResSo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ESPResSo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "initialize.hpp"

#include "ParticleHandle.hpp"
#include "ParticleList.hpp"
#include "ParticleSlice.hpp"
#include "Polymer.hpp"

namespace ScriptInterface {
namespace Particles {

void initialize(Utils::Factory<ObjectHandle> *om) {
  om->register_new<ParticleHandle>("Particles::ParticleHandle");
  om->register_new<ParticleList>("Particles::ParticleList");
  om->register_new<ParticleSlice>("Particles::ParticleSlice");
  om->register_new<Polymer>("Particles::Polymer");
}

} // namespace Particles
} // namespace ScriptInterface
