#include <iostream>
#include "opencv2/opencv.hpp"

const cv::String keys = "{img | | Image path}";

cv::String path_img = "";
cv::Mat img;

int main(int argc, char** argv)
{
    std::cout << "Test capturing an image\n"; 

    // Parse the arguments
    cv::CommandLineParser parser(argc, argv, keys);
    if (!parser.has("img"))
    {
        std::cout << "[ERR] Missing the argument --img\n";
        parser.printMessage();
        return 0;
    }

    path_img += parser.get<cv::String>("img");
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    // Read the image
    img = cv::imread(path_img, cv::IMREAD_COLOR);
    if (img.empty())
    {
        std::cout << "[ERR] Cannot read the image: ";
        std::cout << path_img << std::endl;
        return 0;
    }
    
    // Display the image
    cv::imshow("Image", img);
    cv::waitKey();
    return 0;
}
