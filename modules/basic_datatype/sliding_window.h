#pragma once
#include <memory>
#include <mutex>
#include "frame.h"
#include "mappoint.h"
#include "util_datatype.h"
#include "../ceres/marginalization_factor.h"
class SlidingWindow {
public:
    SlidingWindow(int max_len_ = 10);

    void clear_all();

    void push_kf(FramePtr keyframe);
    void push_mp(MapPointPtr mappoint);

    size_t size_kfs() const;
    size_t size_mps() const;

    std::vector<FramePtr> get_kfs() const;
    std::vector<MapPointPtr> get_mps();
    std::mutex big_mutex;
    int max_len;
private:

    mutable std::mutex kfs_mutex;
    mutable std::mutex mps_mutex;
    std::deque<FramePtr> kfs_buffer;
    std::deque<MapPointWPtr> mps_buffer;
};
SMART_PTR(SlidingWindow);
