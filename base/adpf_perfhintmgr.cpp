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

#include "adpf_perfhintmgr.hpp"

#include <inttypes.h>
#include <sched.h>
#include <unistd.h>

#include <android/log.h>

#define LOG_TAG "ADPF"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__);
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__);
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);
#ifdef NDEBUG
#define ALOGV(...)
#else
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#endif

//#include "android/platform_util_android.h"
//#include "common.hpp"
//
//using namespace base_game_framework;

// verbose debug logs on?
//#define VERBOSE_LOGGING 1
//
//#if VERBOSE_LOGGING
//#define VLOGD ALOGI
//#else
//#define VLOGD
//#endif

AdpfPerfHintMgr::AdpfPerfHintMgr()
    : performance_hint_manager_(nullptr),
      performance_hint_session_(nullptr),
      work_duration_(nullptr),
      gpu_timestamp_period_set_(false),
      gpu_timestamp_period_(1.0f) {
  int32_t tids[1];
  tids[0] = gettid();
  initializePerformanceHintManager(tids, 1);
}

AdpfPerfHintMgr::~AdpfPerfHintMgr() { uninitializePerformanceHintManager(); }

void AdpfPerfHintMgr::initializePerformanceHintManager(int32_t *thread_ids,
                                               size_t thread_ids_size,
                                               int64_t target_work_duration) {
  ALOGI("AdpfPerfHintMgr::initializePerformanceHintManager %d", __ANDROID_API__);
  target_work_duration_ = target_work_duration;
#if __ANDROID_API__ >= 35
  performance_hint_manager_ = APerformanceHint_getManager();
  performance_hint_session_ = APerformanceHint_createSession(
      performance_hint_manager_, thread_ids, thread_ids_size, target_work_duration);
  work_duration_ = AWorkDuration_create();
#endif
}

void AdpfPerfHintMgr::uninitializePerformanceHintManager() {
#if __ANDROID_API__ >= 35
  if (work_duration_ != nullptr) {
    AWorkDuration_release(work_duration_);
    work_duration_ = nullptr;
  }
  if (performance_hint_session_ != nullptr) {
    APerformanceHint_closeSession(performance_hint_session_);
    performance_hint_session_ = nullptr;
  }
  performance_hint_manager_ = nullptr;
#endif
}

void AdpfPerfHintMgr::setGpuTimestampPeriod(float timestamp_period) {
  ALOGI("AdpfPerfHintMgr::setGpuTimestampPeriod: %f", timestamp_period);
  gpu_timestamp_period_set_ = true;
  gpu_timestamp_period_ = timestamp_period;
}

void AdpfPerfHintMgr::setWorkPeriodStartTimestampNanos(int64_t cpu_timestamp) {
  if (performance_hint_manager_ != nullptr &&
      performance_hint_session_ != nullptr && work_duration_ != nullptr) {
#if __ANDROID_API__ >= 35
    ALOGI("AdpfPerfHintMgr::setWorkPeriodStartTimestampNanos %" PRIu64 "",
          cpu_timestamp);
    AWorkDuration_setWorkPeriodStartTimestampNanos(work_duration_,
                                                   cpu_timestamp);
#endif
  } else {
    ALOGI(
        "AdpfPerfHintMgr::setWorkPeriodStartTimestampNanos performance_hint_manager_ = "
        "%p work_duration_ = %p",
        performance_hint_manager_, work_duration_);
  }
}
void AdpfPerfHintMgr::setActualCpuDurationNanos(int64_t cpu_duration) {
  if (performance_hint_manager_ != nullptr &&
      performance_hint_session_ != nullptr && work_duration_ != nullptr) {
#if __ANDROID_API__ >= 35
    ALOGI("AdpfPerfHintMgr::setActualCpuDurationNanos %" PRIu64 "", cpu_duration);
    AWorkDuration_setActualCpuDurationNanos(work_duration_, cpu_duration);
#endif
  } else {
    ALOGI(
        "AdpfPerfHintMgr::setActualCpuDurationNanos performance_hint_manager_ = %p "
        "work_duration_ = %p",
        performance_hint_manager_, work_duration_);
  }
}
void AdpfPerfHintMgr::setActualGpuDurationNanos(int64_t gpu_duration,
                                        bool apply_multiplier) {
  if (performance_hint_manager_ != nullptr &&
      performance_hint_session_ != nullptr && work_duration_ != nullptr) {
#if __ANDROID_API__ >= 35
    int64_t sent_duration = gpu_duration;
    if (apply_multiplier) {
      sent_duration = gpu_timestamp_period_ * gpu_duration;
    }
    ALOGI(
        "AdpfPerfHintMgr::setActualGpuDurationNanos gpu_timestamp_period_set: %d "
        "gpu_duration: %" PRId64
        " gpu_timestamp_period_: %f sent_duration %" PRId64 "",
        gpu_timestamp_period_set_, gpu_duration, gpu_timestamp_period_,
        sent_duration);
    AWorkDuration_setActualGpuDurationNanos(work_duration_, sent_duration);
#endif
  } else {
    ALOGI(
        "AdpfPerfHintMgr::setActualGpuDurationNanos performance_hint_manager_ = %p "
        "work_duration_ = %p",
        performance_hint_manager_, work_duration_);
  }
}
void AdpfPerfHintMgr::setActualTotalDurationNanos(int64_t cpu_duration) {
  if (performance_hint_manager_ != nullptr &&
      performance_hint_session_ != nullptr && work_duration_ != nullptr) {
#if __ANDROID_API__ >= 35
    ALOGI("AdpfPerfHintMgr::setActualTotalDurationNanos %" PRIu64 "", cpu_duration);
    AWorkDuration_setActualTotalDurationNanos(work_duration_, cpu_duration);
#endif
  } else {
    ALOGI(
        "AdpfPerfHintMgr::setActualTotalDurationNanos performance_hint_manager_ = %p "
        "work_duration_ = %p",
        performance_hint_manager_, work_duration_);
  }
}

void AdpfPerfHintMgr::updateTargetWorkDuration(int64_t target_work_duration) {
  if (performance_hint_manager_ != nullptr &&
      performance_hint_session_ != nullptr && work_duration_ != nullptr) {
#if __ANDROID_API__ >= 35
    if (target_work_duration_ != target_work_duration) {
      int result = APerformanceHint_updateTargetWorkDuration(
          performance_hint_session_, target_work_duration);
      if (result == 0) {
        // SUCCESS
        target_work_duration_ = target_work_duration;
      }
      ALOGI("AdpfPerfHintMgr::updateTargetWorkDuration %" PRIu64 " RESULT: %d",
            target_work_duration, result);
    }
#endif
  } else {
    ALOGI(
        "AdpfPerfHintMgr::updateTargetWorkDuration performance_hint_manager_ = %p "
        "work_duration_ = %p",
        performance_hint_manager_, work_duration_);
  }
}

void AdpfPerfHintMgr::reportActualWorkDuration() {
  if (performance_hint_manager_ != nullptr &&
      performance_hint_session_ != nullptr && work_duration_ != nullptr) {
#if __ANDROID_API__ >= 35
    ALOGI("AdpfPerfHintMgr::reportActualWorkDuration");
    APerformanceHint_reportActualWorkDuration2(performance_hint_session_,
                                               work_duration_);
#endif
  } else {
    ALOGI(
        "AdpfPerfHintMgr::reportActualWorkDuration performance_hint_manager_ = %p "
        "work_duration_ = %p",
        performance_hint_manager_, work_duration_);
  }
}
