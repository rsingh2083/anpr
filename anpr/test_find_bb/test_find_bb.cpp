#include <iostream>
#include "opencv2/opencv.hpp"

const cv::String keys = 
"{ img    | | Image path             }"
"{ thresh | | Thresholded image path }";

cv::String path_img = "";
cv::String path_img_thresh = "";
cv::Mat img;
cv::Mat img_thresh;
std::vector<cv::RotatedRect> rotated_boxes;

// Params
double aspect_ratio_min = 3;
double aspect_ratio_max = 6;
int width_min = 60;
int height_min = 20;


void findBoundingBoxes(const cv::Mat &img_thresh, std::vector<cv::RotatedRect> &rotated_boxes)
{
    CV_Assert(!img_thresh.empty() && img_thresh.channels() == 1 && img_thresh.type() == CV_8UC1);

    std::vector<std::vector<cv::Point> > cnts;

    // Find contours
    cv::findContours(img_thresh, cnts, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Filter the contours
    for (size_t i = 0; i < cnts.size(); ++i)
    {
        cv::Rect bounding_rect = cv::boundingRect(cnts[i]);
        double aspect_ratio = (double) bounding_rect.width / (double) bounding_rect.height;

        if (aspect_ratio >= aspect_ratio_min && aspect_ratio <= aspect_ratio_max && 
            bounding_rect.width >= width_min && bounding_rect.height >= height_min)
        {
            cv::RotatedRect bounding_rotated_rect = cv::minAreaRect(cnts[i]);
            rotated_boxes.push_back(bounding_rotated_rect);
        }
    }
}


int main(int argc, char **argv)
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
    cv::FileStorage fs(path_img_thresh, cv::FileStorage::READ);
    fs["img_thresh"] >> img_thresh;
    fs.release();
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

    // Find the bounding boxes of number plates
    findBoundingBoxes(img_thresh, rotated_boxes);
    for (size_t i = 0; i < rotated_boxes.size(); ++i)
    {
        cv::Point2f vertices[4];
        rotated_boxes[i].points(vertices);
        for (size_t j = 0; j < 4; ++j)
        {
            line(img, vertices[j], vertices[(j+1)%4], cv::Scalar(0 ,255, 0), 1, cv::LINE_AA);
        }
    }

    // Display the images
    cv::imshow("Image", img);
    cv::imshow("Thresholded", img_thresh);
    cv::waitKey();
    return 0;
}
