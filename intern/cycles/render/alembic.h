/*
 * Copyright 2011-2018 Blender Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "graph/node.h"
#include "render/procedural.h"
#include "util/util_vector.h"

//#ifdef WITH_ALEMBIC

#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcGeom/All.h>

using namespace Alembic::AbcGeom;

CCL_NAMESPACE_BEGIN

class Geometry;
class Object;
class Shader;

class AlembicObject : public Node {
 public:
  NODE_DECLARE

  AlembicObject();
  ~AlembicObject();

  ustring path;
  Shader *shader = nullptr;

  Object *object = nullptr;
  Geometry *geometry = nullptr;

  // runtime data
  bool data_loaded = false;

  // TODO : this is only for Meshes at the moment
  // TODO : handle attributes as well
  struct DataCache {
      bool dirty = false;
      array<float3> vertices{};
      array<int3> triangles{};
  };

  // TODO : find a way to store the time, for now we presume that frame 0 == index 0
  vector<DataCache> frame_data;
};

class AlembicProcedural : public Procedural {
  bool need_update = true;
  bool need_update_for_frame_change = true;

 public:
  NODE_DECLARE

  typedef std::pair<const ustring, Shader *> pathShaderType;

  AlembicProcedural();
  ~AlembicProcedural();
  void generate(Scene *scene);

  bool use_motion_blur;
  ustring filepath;
  float frame;
  float frame_rate;
  array<AlembicObject *> objects;

  void tag_update()
  {
    need_update = true;
  }

  bool should_remove(const Node * /*node*/) const
  {
    return false;
  }

  void set_current_frame(Scene *scene, float frame_);

 private:
  void read_mesh(Scene *scene,
                 AlembicObject *abc_object,
                 Transform xform,
                 IPolyMesh &mesh,
                 Abc::chrono_t frame_time);
  void read_curves(Scene *scene,
                   AlembicObject *abc_object,
                   Transform xform,
                   ICurves &curves,
                   Abc::chrono_t frame_time);
};

CCL_NAMESPACE_END
