#include <iostream>
#include "opencv2/opencv.hpp"

const cv::String keys = "{img | | Image path}";

cv::String path_img = "";
cv::Mat img;

// Params
cv::Size kernel_size_rect(13, 5); // Assumption: The number plate region is ~3x wider than it is tall.


void revealCandidates(const cv::Mat& img)
{
    CV_Assert(!img.empty() && img.channels() == 3);

    cv::Mat img_gray;
    cv::Mat img_blackhat;
    cv::Mat img_gradX;
    cv::Mat kernel_rect;

    // Convert to grayscale
    cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

    // Reveal dark regions against light backgrounds
    kernel_rect = cv::getStructuringElement(cv::MORPH_RECT, kernel_size_rect);
    cv::morphologyEx(img_gray, img_blackhat, cv::MORPH_BLACKHAT, kernel_rect);
    imshow("[dbg] blackhat", img_blackhat);

    // Reveal regions which are not only dark against light backgrounds 
    // but also contain vertical changes in gradient along the x-axis.
    // 3Å~3 Scharr filter may give more accurate results than the 3Å~3 Sobel
    cv::Sobel(img_blackhat, img_gradX, CV_32F, 1, 0, cv::FILTER_SCHARR);
    cv::normalize(cv::abs(img_gradX), img_gradX, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    imshow("[dbg] gradX", img_gradX);
}


int main(int argc, char** argv)
{
    std::cout << "Test revealing the candidates of number plates\n"; 

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

    // Reveal the candidates of number plates
    revealCandidates(img);

    // Display the image
    cv::imshow("Image", img);
    cv::waitKey();
    return 0;
}
