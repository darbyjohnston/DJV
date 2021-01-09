#include <opencv2/opencv.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    const int w = 64;
    const int h = 64;
    cv::Mat cvImage(w, h, CV_8UC1);
    cv::Mat cvImage2(w, h, CV_8UC1);
    cv::threshold(cvImage, cvImage2, 128, 255, cv::THRESH_BINARY);
    return 0;
}
