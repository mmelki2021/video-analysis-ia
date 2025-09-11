#include "visualizer.hpp"

cv::Mat Visualizer::draw(const cv::Mat& frame, const std::vector<TrackedObject>& trackedObjects) {
    cv::Mat output = frame.clone();
    for (const auto& obj : trackedObjects) {
        cv::rectangle(output, obj.bbox, cv::Scalar(0, 255, 0), 2);
        cv::putText(output, "ID: " + std::to_string(obj.id),
                    cv::Point(obj.bbox.x, obj.bbox.y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);
    }
    return output;
}
