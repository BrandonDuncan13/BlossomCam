// Process blossom image

#include <filesystem>
#include <string>

#include "process_image.hpp"

// Defines
#define IMAGEPATH "images/image"
#define EXTENSION ".jpg"


// Here is where the image gets processed
std::string ProcessImage::get_processed_image()
{
    // Reset 'numBlossoms'
    numBlossoms = 0;

    // Counter to remember which photo it is using
    static int count = 0;
    count++;

    // Paths
    std::string originalImagePath = IMAGEPATH + std::to_string(count) + EXTENSION;
    std::string processedImagePath = "images/processed_image" + std::to_string(count) + EXTENSION;
    std::string cachePath = std::filesystem::temp_directory_path().string();

    // Finding cache path for different devices
    if (getOsName() == "Apple")
    {
        // Make sure there is no '/' at the end of the path
        if (cachePath.at(cachePath.length() - 1) == '/')
        {
            cachePath = cachePath.substr(0, cachePath.find_last_of("/"));
        }
    
        // Edit path to be cache directory
        cachePath = cachePath.substr(0, cachePath.find_last_of("/"));
        cachePath += "/Library/Caches/";
    }
    else
    {
        // This will be where the cache path is fixed for Android
        return "error: Android not yet supported";
    }

    // Finishing path variables
    originalImagePath = cachePath + originalImagePath;
    processedImagePath = cachePath + processedImagePath;

    // Filter the image using the algorithm
    cv::Mat originalImage = cv::imread(originalImagePath);    
    // cv::Mat copyImage = grayImage(originalImage);
    cv::Mat copyImage = filterImage(originalImage);
    // cv::Mat copyImage = dummyFilter(originalImage);

    // Write to processed image file
    cv::imwrite(processedImagePath, copyImage);

    // Generating the return string
    std::string myString = std::to_string(numBlossoms) + "$$" + processedImagePath;
    return myString;
}


// Find OS name
std::string ProcessImage::getOsName()
{
    #ifdef _WIN32
    return "Windows 32-bit";
    #elif _WIN64
    return "Windows 64-bit";
    #elif __APPLE__
    return "Apple";
    #elif __MACH__
    return "Mac OS";
    #elif __linux__
    return "Linux";
    #elif __FreeBSD__
    return "FreeBSD";
    #elif __unix || __unix__
    return "Unix";
    #else
    return "Other";
    #endif
}


/*
    Here are the image filters
*/


// The dummy filter
cv::Mat ProcessImage::dummyFilter(cv::Mat inputImage)
{
    cv::Mat filterImage = inputImage.clone();

    std::cout << filterImage.channels() << std::endl;

    // Applying color filter to isolate blossoms.
    for ( int i = 0; i < filterImage.rows; i++)
        for (int j = 0; j < filterImage.cols; j++)
            if ((double)filterImage.at<cv::Vec4b>(i,j)[0] == 255 && (double)filterImage.at<cv::Vec4b>(i,j)[1] == 255 && (double)filterImage.at<cv::Vec4b>(i,j)[2] == 255)
        {
            /*
            filterImage.at<cv::Vec4b>(i,j)[0] = 0;
            filterImage.at<cv::Vec4b>(i,j)[0] = 0;
            filterImage.at<cv::Vec4b>(i,j)[0] = 0;
             */
            filterImage.setTo(cv::Scalar::all(0));   
        }

    cv:: Mat grayImage;

    // Converting Image to gray scale.
    cv::cvtColor(filterImage, grayImage, cv::COLOR_BGR2GRAY);

    cv:: Mat ThresholdImage;

    cv::threshold(grayImage, ThresholdImage, 0, 255, cv::THRESH_BINARY);

    cv::Mat BinaryImage;

    cv::bitwise_not(ThresholdImage, BinaryImage);

    cv::Mat UnBinary;

    cv::bitwise_not(BinaryImage, UnBinary);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(UnBinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    int BlossomsDetected = 0;

    for (int pointer = 0; pointer < contours.size(); pointer++)
    {
        if (cv::contourArea(contours[pointer]) > 50 && cv::contourArea(contours[pointer]) < 60)
        {
            BlossomsDetected = BlossomsDetected + 1;
        }
    }

    // Added
    numBlossoms = BlossomsDetected;

    return filterImage;
}


// Filter image
cv::Mat ProcessImage::filterImage(cv::Mat inputImage)
{
    cv::Mat filterImage = inputImage.clone();

    std::cout << filterImage.channels() << std::endl;

    // Applying color filter to isolate blossoms.
    for ( int i = 0; i < filterImage.rows; i++)
        for (int j = 0; j < filterImage.cols; j++)
            if ((7 * (double)filterImage.at<cv::Vec3b>(i,j)[0] - 9 * (double)filterImage.at<cv::Vec3b>(i,j)[2] + 135) && (double)filterImage.at<cv::Vec3b>(i,j)[2] < 155)
        {
            filterImage.at<cv::Vec3b>(i,j)[0] = 0;
            filterImage.at<cv::Vec3b>(i,j)[1] = 0;
            filterImage.at<cv::Vec3b>(i,j)[2] = 0;
        }

    cv:: Mat grayImage;

    // Converting Image to gray scale.
    cv::cvtColor(filterImage, grayImage, cv::COLOR_BGR2GRAY);

    cv:: Mat ThresholdImage;

    cv::threshold(grayImage, ThresholdImage, 0, 255, cv::THRESH_BINARY);

    cv::Mat BinaryImage;

    cv::bitwise_not(ThresholdImage, BinaryImage);

    cv::Mat UnBinary;

    cv::bitwise_not(BinaryImage, UnBinary);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(UnBinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    int BlossomsDetected = 0;

    for (int pointer = 0; pointer < contours.size(); pointer++)
    {
        if (cv::contourArea(contours[pointer]) > 50 && cv::contourArea(contours[pointer]) < 60)
        {
            BlossomsDetected = BlossomsDetected + 1;
        }
    }

    // Added
    numBlossoms = BlossomsDetected;

    return UnBinary;
}


// Convert image to grayscale
cv::Mat ProcessImage::grayImage(cv::Mat inputImage)
{
    cv::Mat grayscaleImage;

    cv::cvtColor(inputImage, grayscaleImage, cv::COLOR_BGR2GRAY);

    return grayscaleImage;
}
