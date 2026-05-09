#include <opencv2/opencv.hpp>
#include <iostream>
int main() {
    std::cout << "OpenCV version: " << CV_VERSION << "\n";
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "FAILED: Cannot open webcam\n";
        return -1;
    }
    std::cout << "SUCCESS: Webcam works!\n";
    cap.release();
    return 0;
}
