#include <iostream>
#include "opencv2/opencv.hpp"

const cv::String keys = 
"{ img    | | Image path             }"
"{ thresh | | Thresholded image path }";


cv::String path_img = "";
cv::String path_img_thresh = "";
cv::Mat img;
cv::Mat img_thresh;

int main(int argc, char** argv)
{
    std::cout << "Test finding the bounding boxes of number plates\n";

    // Parse the arguments
    cv::CommandLineParser parser(argc, argv, keys);
    if (!parser.has("img") || !parser.has("thresh"))
    {
        if (!parser.has("img"))
        {
            std::cout << "[ERR] Missing the argument --img\n";
        }
        if (!parser.has("thresh"))
        {
            std::cout << "[ERR] Missing the argument --thresh\n";
        }
        parser.printMessage();
        return 0;
    }

    path_img += parser.get<cv::String>("img");
    path_img_thresh += parser.get<cv::String>("thresh");
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    // Read the images
    img = cv::imread(path_img, cv::IMREAD_COLOR);
    img_thresh = cv::imread(path_img_thresh, cv::IMREAD_COLOR); 
    if (img.empty() || img_thresh.empty())
    {
        if (img.empty())
        {
            std::cout << "[ERR] Cannot read the image: ";
            std::cout << path_img << std::endl;
        }
        if (img_thresh.empty())
        {
            std::cout << "[ERR] Cannot read the thresholded image: ";
            std::cout << path_img_thresh << std::endl;
        }
        return 0;
    }

    // Display the images
    cv::imshow("Image", img);
    cv::imshow("Thresholded", img_thresh);
    cv::waitKey();
    return 0;
}