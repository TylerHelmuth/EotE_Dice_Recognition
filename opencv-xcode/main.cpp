/**
 * Dice Recogniction
 *
 * @author Glenn De Backer >
 *
 * @license Creative Commons Attribution-NonCommercial 3.0 / CC BY-NC 3.0
 *
 */

// std lib
#include <iostream>
#include <sstream>

// OpenCV
#include <opencv2/core.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;
/// Global variables

//Mat src, src_gray;
//Mat dst, detected_edges;
//
//int edgeThresh = 1;
//int lowThreshold = 65;
////int const max_lowThreshold = 200;
//int cannyRatio = 3;
//int kernel_size = 3;
//string window_name = "Edge Map";

const int same = 0;
const int previous = 1;
const int first_child = 2;
const int parent = 3;

void on_low_h_thresh_trackbar(int, void *);
void on_high_h_thresh_trackbar(int, void *);
void on_low_s_thresh_trackbar(int, void *);
void on_high_s_thresh_trackbar(int, void *);
void on_low_v_thresh_trackbar(int, void *);
void on_high_v_thresh_trackbar(int, void *);
//int low_h=50, low_s=81, low_v=85;
//int high_h=95, high_s=255, high_v=255;

//green
int low_h=50, low_s=70, low_v=70;
int high_h=95, high_s=255, high_v=255;

//yellow
//int low_h=16, low_s=0, low_v=44;
//int high_h=58, high_s=255, high_v=255;

//const int advantageContourIndex = 3;
//const double advantageThreshold = 0.3;

//const int successContourIndex = 0;
//const double successThreshold = 0.1;

//const int triumphContourIndex = 25;
//const double triumphThreshold = 1;


void getContours(const Mat& src, Mat& dst, vector<vector<Point>>& contours, vector<Vec4i>& heirarchy) {
    cvtColor(src, dst, COLOR_BGR2HSV);
    inRange(dst,Scalar(low_h,low_s,low_v), Scalar(high_h,high_s,high_v),dst);
    floodFill(dst, Point(0,0), Scalar(255));
    blur(dst, dst, Size(5, 5));
    bitwise_not(dst, dst);
    findContours(dst, contours, heirarchy, CV_RETR_TREE, THRESH_TOZERO);
}

void getContoursForMaster(const Mat& src, Mat& dst, vector<vector<Point>>& contours, vector<Vec4i>& heirarchy) {
    threshold(src, dst, 100, 255, THRESH_BINARY);
    blur(dst, dst, Size(5, 5));
    findContours(dst, contours, heirarchy, CV_RETR_TREE, THRESH_TOZERO);
}


void showContour(const Mat& src, const vector<vector<Point>>& contours, const vector<Point>& contour, const string& window) {
    RotatedRect rect = minAreaRect(contour);
    
    Mat M, rotated, cropped;
    
    float angle = rect.angle;
    Size rect_size = rect.size;
    if (rect.angle < -45.) {
        angle += 90.0;
        swap(rect_size.width, rect_size.height);
    }
    
    // get the rotation matrix
    M = getRotationMatrix2D(rect.center, angle, 1.0);
    // perform the affine transformation
    warpAffine(src, rotated, M, src.size(), INTER_CUBIC);
    // crop the resulting image
    getRectSubPix(rotated, rect_size, rect.center, cropped);
    
    
    if (cropped.data) {
        
        //        Mat temp;
        //        cvtColor(src, temp, COLOR_GRAY2RGB);
        //        drawContours(temp, contours, 25, Scalar(0, 255, 0));
        //        imshow(window + "with contour", temp);
        imshow(window, cropped);
        //        waitKey();
        //        vector<int> compression_params;
        //        compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
        //        compression_params.push_back(9);
        //        imwrite("success2.png", cropped, compression_params);
    }
}

bool hasOneSignificantChild(int currentIndex, const vector<vector<Point>>& contours, const vector<Vec4i>& hierarchies) {
    int siblingIndex = hierarchies[currentIndex][same];
    
    while (siblingIndex != -1) {
        double area = contourArea(contours[currentIndex]);
        if (area > 20) {
            return false;
        } else {
            currentIndex = siblingIndex;
            siblingIndex = hierarchies[currentIndex][same];
        }
    }
    
    return true;
}

bool isSuccess(const vector<Point>& master, const int& index, const vector<vector<Point>>& contours, const vector<Vec4i>& hierarchies) {
    int firstChildIndex = hierarchies[index][first_child];
    
    if (firstChildIndex > -1) {
        if (hasOneSignificantChild(firstChildIndex, contours, hierarchies)) {
            double result = matchShapes(master, contours[index], CV_CONTOURS_MATCH_I3, 0.0);
            cout << result << endl;
//            return result < 0.1;
            return true;
        }
    }
    return false;
}

int main(int argc, const char** argv)
{
    //    namedWindow("Video Capture", WINDOW_NORMAL);
    //    namedWindow("Object Detection", WINDOW_NORMAL);
    //    //-- Trackbars to set thresholds for RGB values
    //    createTrackbar("Low H","Object Detection", &low_h, 179, on_low_h_thresh_trackbar);
    //    createTrackbar("High H","Object Detection", &high_h, 179, on_high_h_thresh_trackbar);
    //    createTrackbar("Low S","Object Detection", &low_s, 255, on_low_s_thresh_trackbar);
    //    createTrackbar("High S","Object Detection", &high_s, 255, on_high_s_thresh_trackbar);
    //    createTrackbar("Low V","Object Detection", &low_v, 255, on_low_v_thresh_trackbar);
    //    createTrackbar("High V","Object Detection", &high_v, 255, on_high_v_thresh_trackbar);
    //
    //    Mat hsv, hsv_threshold, frame = imread("triumph.png");
    //
    //
    //    do{
    //        cvtColor(frame, hsv, COLOR_BGR2HSV);
    //
    //        inRange(hsv,Scalar(low_h,low_s,low_v), Scalar(high_h,high_s,high_v),hsv_threshold);
    //        floodFill(hsv_threshold, Point(0,0), Scalar(255));
    //        bitwise_not(hsv_threshold, hsv_threshold);
    //
    //        imshow("Video Capture",frame);
    //        imshow("Object Detection",hsv_threshold);
    //
    //        cout << low_h << " " << low_s << " " << low_v << "\r\n";
    //        cout << high_h << " " << high_s << " " << high_v << "\r\n\r\n";
    //    }while((char)waitKey(0)!='q');
    
    
    Mat img1 = imread("success.png", IMREAD_GRAYSCALE), img2 = imread("green-advantage-hard.JPG"), hsv1, hsv2;
    vector<vector<Point>> img1Contours, img2Contours;
    vector<Vec4i> img1Hierarchy, img2Hierarchy;
    
    getContoursForMaster(img1, hsv1, img1Contours, img1Hierarchy);
    //    getContours(img1, hsv1, img1Contours, img1Hierarchy);
    getContours(img2, hsv2, img2Contours, img2Hierarchy);
    
//            imshow("hsv1", hsv1);
//            imshow("hsv2", hsv2);
//            waitKey();
    
    //    for (int i = 0; i < img1Contours.size(); i++) {
    //        if (img1Contours[i].size() > 19) {
    //            showContour(hsv1, img1Contours[i], to_string(i));
    //            waitKey();
    //        }
    //    }
    //
    //    for (int i = 0; i < img2Contours.size(); i++) {
    //        if (contourArea(img2Contours[i]) > 20)
    //            showContour(hsv2, img2Contours[0], to_string(0));
    //    }
    
    
    
//    showContour(hsv1, img1Contours, img1Contours[25], "master");
    
    for (int i = 0; i < img2Contours.size(); i++) {
        if (isSuccess(img1Contours[0], i, img2Contours, img2Hierarchy)) {
            showContour(hsv2, img2Contours, img2Contours[i], "contour" + to_string(i));
        }
    }
    
    waitKey();
    
    return 0;
}

void on_low_h_thresh_trackbar(int, void *)
{
    low_h = min(high_h-1, low_h);
    setTrackbarPos("Low R","Object Detection", low_h);
}
void on_high_h_thresh_trackbar(int, void *)
{
    high_h = max(high_h, low_h+1);
    setTrackbarPos("High R", "Object Detection", high_h);
}
void on_low_s_thresh_trackbar(int, void *)
{
    low_s = min(high_s-1, low_s);
    setTrackbarPos("Low G","Object Detection", low_s);
}
void on_high_s_thresh_trackbar(int, void *)
{
    high_s = max(high_s, low_s+1);
    setTrackbarPos("High G", "Object Detection", high_s);
}
void on_low_v_thresh_trackbar(int, void *)
{
    low_v= min(high_v-1, low_v);
    setTrackbarPos("Low B","Object Detection", low_v);
}
void on_high_v_thresh_trackbar(int, void *)
{
    high_v = max(high_v, low_v+1);
    setTrackbarPos("High B", "Object Detection", high_v);
}

