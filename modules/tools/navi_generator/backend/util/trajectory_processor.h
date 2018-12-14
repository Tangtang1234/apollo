/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
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
 *****************************************************************************/

/**
 * @file
 * @brief This file provides the declaration of the class
 * "TrajectoryProcessor".
 */

#ifndef MODULES_TOOLS_NAVI_GENERATOR_BACKEND_UTIL_TRAJECTORY_PROCESSOR_H_
#define MODULES_TOOLS_NAVI_GENERATOR_BACKEND_UTIL_TRAJECTORY_PROCESSOR_H_

#include <condition_variable>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "modules/tools/navi_generator/backend/database/db_operator.h"
#include "modules/tools/navi_generator/backend/util/trajectory_converter.h"

/**
 * @namespace apollo::planning
 * @brief apollo::planning
 */
namespace apollo {
namespace navi_generator {
namespace util {

struct CommonBagFileInfo {
  std::uint16_t total_file_num;
  std::uint8_t left_lane_num;
  std::uint8_t right_lane_num;
};
struct FileSegment {
  std::uint16_t cur_file_index;
  std::uint16_t cur_file_seg_count;
  std::uint16_t cur_file_seg_index;
  std::string cur_file_name;
  std::string next_file_name;
  std::string cur_file_seg_data;
};
struct BagFileInfo {
  std::uint16_t file_index = 0;
  std::string raw_file_name;
  std::string next_raw_file_name;
};
struct NaviFile {
  std::uint8_t navi_index;
  std::string navi_file_name;
};
struct FileInfo {
  BagFileInfo bag_file_info;
  std::vector<NaviFile> navi_files;
  std::uint8_t speed_min = 0;  // TODO(wmz): read the speed limits from bag file
  std::uint8_t speed_max = 0;
};

class TrajectoryProcessor {
 public:
  TrajectoryProcessor();
  ~TrajectoryProcessor();

 public:
  bool SetCommonBagFileInfo(const CommonBagFileInfo& common_file_info);
  bool ProcessBagFileSegment(const FileSegment& file_segment);
  bool SaveFilesToDatabase();
  bool Reset();

 private:
  void FileSegmentThread();
  bool ExportSegmentsToFile(const std::string& file_name);
  void ProcessFiles();
  bool ProcessFile(const BagFileInfo& bag_file_info);

 private:
  CommonBagFileInfo common_file_info_;
  // first: cur_file_seg_index, second: FileSegment
  std::map<std::uint16_t, FileSegment> cur_file_segment_s_;
  // first: file_index, second: BagFileInfo
  std::map<std::uint16_t, BagFileInfo> bag_files_to_process_;

  // A thread and its related variables that receive and process the bag files.
  std::unique_ptr<std::thread> file_seg_thread_;
  std::condition_variable file_seg_cv_;
  mutable std::mutex file_seg_mut_;
  bool file_seg_finished_ = false;
};

}  // namespace util
}  // namespace navi_generator
}  // namespace apollo

#endif  // MODULES_TOOLS_NAVI_GENERATOR_BACKEND_UTIL_TRAJECTORY_PROCESSOR_H_