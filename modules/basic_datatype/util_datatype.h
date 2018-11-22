#pragma once
#include <memory>
#include <vector>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

using ImagePyr = std::vector<cv::Mat>;
using VecVector2d = std::vector<Eigen::Vector2d, Eigen::aligned_allocator<Eigen::Vector2d>>;
using VecVector3d = std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>>;
using VecVector2f = std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>>;
using VecVector3f = std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>;

#define SMART_PTR(NAME) \
using NAME##Ptr = std::shared_ptr<NAME>; \
using NAME##ConstPtr = std::shared_ptr<const NAME>; \
using NAME##WPtr = std::weak_ptr<NAME>;
