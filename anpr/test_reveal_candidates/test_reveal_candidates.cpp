#include <iostream>
#include "opencv2/opencv.hpp"

const cv::String keys = 
"{img  | | Image path               }"
"{save | | Flag to save output image}";

cv::String path_img = "";
cv::Mat img;
cv::Mat img_thresh;

// Params
cv::Size sz_kernel_rect(13, 5); // Assumption: The number plate region is ~3x wider than it is tall.
cv::Size sz_kernel_square(3, 3);
cv::Size sz_kernel_gaussian(5, 5);
double thresh_light = 50;

void revealCandidates(const cv::Mat& img, cv::Mat& img_thresh)
{
    CV_Assert(!img.empty() && img.channels() == 3);

    cv::Mat img_gray;
    cv::Mat img_blackhat;
    cv::Mat img_gradX;
    cv::Mat img_light;

    cv::Mat kernel_rect;
    cv::Mat kernel_square;

    // Convert to grayscale
    cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

    // Reveal dark regions against light backgrounds
    kernel_rect = cv::getStructuringElement(cv::MORPH_RECT, sz_kernel_rect);
    cv::morphologyEx(img_gray, img_blackhat, cv::MORPH_BLACKHAT, kernel_rect);
    imshow("[dbg] blackhat", img_blackhat);

    // Reveal regions which are not only dark against light backgrounds 
    // but also contain vertical changes in gradient along the x-axis.
    // 3Å~3 Scharr filter may give more accurate results than the 3Å~3 Sobel
    cv::Sobel(img_blackhat, img_gradX, CV_32F, 1, 0, cv::FILTER_SCHARR);
    cv::normalize(cv::abs(img_gradX), img_gradX, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    imshow("[dbg] gradX", img_gradX);

    // Reveal a "somewhat" rectangular region
    cv::GaussianBlur(img_gradX, img_gradX, sz_kernel_gaussian, 0);
    cv::morphologyEx(img_gradX, img_gradX, cv::MORPH_CLOSE, kernel_rect);
    cv::threshold(img_gradX, img_thresh, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
    imshow("[dbg] thresh-original", img_thresh);
    
    // Clean up
    cv::erode(img_thresh, img_thresh, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(img_thresh, img_thresh, cv::Mat(), cv::Point(-1, -1), 2);
    imshow("[dbg] thresh-clean", img_thresh);

    // Reveal regions in the image which are light
    kernel_square = cv::getStructuringElement(cv::MORPH_RECT, sz_kernel_square);
    cv::morphologyEx(img_gray, img_light, cv::MORPH_CLOSE, kernel_square);
    cv::threshold(img_light, img_light, thresh_light, 255, cv::THRESH_BINARY);
    imshow("[dbg] light", img_light);

    // Reveal the candidates
    // Remove noise and irrelevant region
    cv::bitwise_and(img_thresh, img_thresh, img_thresh, img_light);
    cv::dilate(img_thresh, img_thresh, cv::Mat(), cv::Point(-1, -1), 2);
    cv::erode(img_thresh, img_thresh, cv::Mat(), cv::Point(-1, -1), 2);
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
    revealCandidates(img, img_thresh);
    if (parser.has("save"))
    {
        cv::String path_in = parser.get<cv::String>("img");
        cv::String path_out = path_in.substr(0, path_in.size() - 4) + "_thresh.xml";
        cv::FileStorage fs(path_out, cv::FileStorage::WRITE);
        fs << "img_thresh" << img_thresh;
        fs.release();
        std::cout << "[INF] Saved " << path_out << std::endl;
    }

    // Display the image
    cv::imshow("Image", img);
    cv::imshow("Thresholded", img_thresh);
    cv::waitKey();
    return 0;
}
