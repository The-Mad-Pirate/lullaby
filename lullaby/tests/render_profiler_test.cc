/*
Copyright 2017 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS-IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <thread>

#include "gtest/gtest.h"
#include "lullaby/systems/render/detail/profiler.h"
#include "lullaby/util/math.h"

namespace lull {

// Empty implementation of Shader so we can use ShaderPtr.
class Shader {
 public:
  Shader() {}
};

namespace {

TEST(RenderProfilerTest, CpuTime) {
  detail::Profiler profiler;

  // Skip first frame since it can't be complete.
  profiler.BeginFrame();
  profiler.EndFrame();

  profiler.BeginFrame();
  std::this_thread::sleep_for(std::chrono::milliseconds(16));
  profiler.EndFrame();

  EXPECT_GE(profiler.GetCpuFrameMs(), 16);
}

TEST(RenderProfilerTest, Fps) {
  detail::Profiler profiler;

  const int kNumFrames = 100;
  for (int i = 0; i < kNumFrames; ++i) {
    profiler.BeginFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(17));
    profiler.EndFrame();
  }

  EXPECT_LT(profiler.GetLastFps(), 60.0f);
  EXPECT_LT(profiler.GetFilteredFps(), 60.0f);
}

TEST(RenderProfilerTest, Stats) {
  detail::Profiler profiler;

  // Skip first frame since it can't be complete.
  profiler.BeginFrame();
  profiler.EndFrame();

  ShaderPtr shader1(new Shader);
  ShaderPtr shader2(new Shader);

  profiler.BeginFrame();
  profiler.RecordDraw(shader1, 100, 134);
  profiler.RecordDraw(shader2, 37, 3);
  profiler.RecordDraw(shader2, 40, 73);
  profiler.EndFrame();

  EXPECT_EQ(profiler.GetNumDraws(), 3);
  EXPECT_EQ(profiler.GetNumShaderSwaps(), 2);
  EXPECT_EQ(profiler.GetNumVerts(), 100 + 37 + 40);
  EXPECT_EQ(profiler.GetNumTris(), 134 + 3 + 73);
}

}  // namespace
}  // namespace lull
