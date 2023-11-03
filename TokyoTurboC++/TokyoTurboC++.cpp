#include <opencv2/opencv.hpp>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Error: Could not open camera." << std::endl;
        return -1;
    }

    cv::namedWindow("Controls");
    int sensitivity = 0;
    cv::createTrackbar("Sensitivity", "Controls", &sensitivity, 255);

    while (true) {
        cv::Mat frame;
        cap >> frame;

        cv::Mat hsv;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        int lowerHue = std::max(0, 0 - sensitivity);
        int lowerSaturation = std::max(0, 120 - sensitivity);
        int lowerValue = std::max(0, 70 - sensitivity);
        cv::Scalar lowerRed(lowerHue, lowerSaturation, lowerValue);

        cv::Scalar upperRed(10, 255, 255);

        cv::Mat mask;
        cv::inRange(hsv, lowerRed, upperRed, mask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        if (!contours.empty()) {
            auto largestContour = *std::max_element(contours.begin(), contours.end(),
                [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
                    return cv::contourArea(c1) < cv::contourArea(c2);
                });

            cv::Moments M = cv::moments(largestContour);
            if (M.m00 != 0) {
                int cX = static_cast<int>(M.m10 / M.m00);
                int cY = static_cast<int>(M.m01 / M.m00);
                cv::circle(frame, cv::Point(cX, cY), 5, cv::Scalar(0, 255, 0), 2);
            }
        }

        cv::imshow("Red Color", frame);

        char key = static_cast<char>(cv::waitKey(1));
        if (key == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
