/*
 * Copyright 2024 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef adpf_perfhintmgr_hpp
#define adpf_perfhintmgr_hpp

#if defined(__ANDROID__)

#include <android/performance_hint.h>

//#include "common.hpp"
//#include "display_manager.h"
//#include "system_event_manager.h"
//#include "user_input_manager.h"

//using namespace base_game_framework;

#define DEFAULT_TARGET_NS 16666666

class AdpfPerfHintMgr {
 private:
  AdpfPerfHintMgr();
  AdpfPerfHintMgr(const AdpfPerfHintMgr& obj) = delete;

  APerformanceHintManager *performance_hint_manager_;
  APerformanceHintSession *performance_hint_session_;
  AWorkDuration *work_duration_;

  bool gpu_timestamp_period_set_;
  float gpu_timestamp_period_;
  int64_t target_work_duration_;

 public:
  ~AdpfPerfHintMgr();

  static AdpfPerfHintMgr &getInstance() {
    static AdpfPerfHintMgr instance;
    return instance;
  }

  int64_t getTargetWorkDuration() { return target_work_duration_; }

  void initializePerformanceHintManager(
      int32_t *thread_ids, size_t thread_size,
      int64_t target_work_duration = DEFAULT_TARGET_NS);
  void uninitializePerformanceHintManager();

  void setupQueryTimer();
  static void timerCallback(void* user_data, int64_t cpu_time, int64_t gpu_time);

  void setGpuTimestampPeriod(float timestamp_period);

  void setWorkPeriodStartTimestampNanos(int64_t cpu_timestamp);
  void setActualCpuDurationNanos(int64_t cpu_duration);
  void setActualGpuDurationNanos(int64_t gpu_duration, bool apply_multiplier);
  void setActualTotalDurationNanos(int64_t cpu_duration);
  void updateTargetWorkDuration(int64_t target_work_duration);
  void reportActualWorkDuration();
};

#endif // #if defined(__ANDROID__)

#endif // adpf_perfhintmgr.hpp
