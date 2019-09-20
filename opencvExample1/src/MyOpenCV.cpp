#include <emscripten/bind.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <iostream>
#include <math.h>
#include <string.h>

using namespace std;
using namespace cv;
using namespace emscripten;

typedef std::vector<double> Corner;
typedef std::vector<Corner> Rectangle;

bool sortPoints(Point a, Point b){
    return a.y < b.y;
}

vector<Point> orderRect( vector<Point> p){

     std::sort(p.begin(), p.end(), sortPoints);

     //sort first two points by x
     if (p[0].x > p[1].x){
        Point temp = p[0];
        p[0] = p[1];
        p[1] = temp;
     }

     //sort last two points by x
      if (p[2].x < p[3].x){
         Point temp = p[2];
         p[2] = p[3];
         p[3] = temp;
      }


     return p;
}


Rectangle getDefaultRect(const size_t width, const size_t height){
    Rectangle defaultRect;
    Corner a;
    a.push_back(0);
    a.push_back(0);

    Corner b;
    b.push_back(width);
    b.push_back(0);

    Corner c;
    c.push_back(width);
    c.push_back(height);

    Corner d;
    d.push_back(0);
    d.push_back(height);

    defaultRect.push_back(a);
    defaultRect.push_back(b);
    defaultRect.push_back(c);
    defaultRect.push_back(d);

    return defaultRect;
}

Rectangle convertToRectangle(vector<Point> points){

    vector<Point> p = orderRect(points);
    Rectangle defaultRect;
    Corner a;
    a.push_back(p[0].x);
    a.push_back(p[0].y);

    Corner b;
    b.push_back(p[1].x);
    b.push_back(p[1].y);

    Corner c;
    c.push_back(p[2].x);
    c.push_back(p[2].y);

    Corner d;
    d.push_back(p[3].x);
    d.push_back(p[3].y);

    defaultRect.push_back(a);
    defaultRect.push_back(b);
    defaultRect.push_back(c);
    defaultRect.push_back(d);

    return defaultRect;

}


int angle( Point c, Point b, Point a )
{
    double dx1 = a.x - b.x;
    double dy1 = a.y - b.y;
    double dx2 = c.x - b.x;
    double dy2 = c.y - b.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}


bool isNotOnBorder(vector<Point> p, int w, int h){
    bool flag = true;
    int minDistance = 3;
    int maxX = w-3;
    int maxY = h-3;

    for(int i=0;i<4;i++){
       flag = flag && (p[i].x > minDistance && p[i].y > minDistance && p[i].x < maxX && p[i].y < maxY);
    }
    return flag;
}

bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i > j );
}

bool firstPointIsInside(Point p){
   return p.x > 0 && p.y > 0;
}

bool angleFilter(vector<Point> approx){
    double maxCosine = fabs(angle(approx[0], approx[3], approx[1]));

    for( int j = 2; j < 5; j++ )
    {
        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
        maxCosine = MAX(maxCosine, cosine);
    }
    return (maxCosine < 0.3) && firstPointIsInside(approx[0]);
}

double medianMat(cv::Mat Input)
{
    Input = Input.reshape(0,1);
    std::vector<double> vecFromMat;
    Input.copyTo(vecFromMat);
    std::nth_element(vecFromMat.begin(), vecFromMat.begin() + vecFromMat.size() / 2, vecFromMat.end());
    return vecFromMat[vecFromMat.size() / 2];
}

vector<vector<Point> > filterAllPoints(vector<vector<Point> > allPoints, int w, int h){

    //step1: sepeare inside and onside border points
    vector<vector<Point> > onTheBorder;
    vector<vector<Point> > insideBorder;
    for(int i=0;i< allPoints.size();i++){
        if(isNotOnBorder(allPoints[i], w, h)){
           insideBorder.push_back(allPoints[i]);
        }
        else{
           onTheBorder.push_back(allPoints[i]);
        }
    }

    //step1: take top 3 rectangles with inside high priority
    std::sort(insideBorder.begin(), insideBorder.end(), compareContourAreas);
    std::sort(onTheBorder.begin(), onTheBorder.end(), compareContourAreas);

    if(insideBorder.size() > 3){
        insideBorder = std::vector<vector<Point> >(insideBorder.begin(), insideBorder.begin()+3);
    }
    else{
       int i=0;
       while(i<onTheBorder.size()&&insideBorder.size()<3){
         insideBorder.push_back(onTheBorder[i]);
         i++;
       }
    }

    return insideBorder;

}

Rectangle findRectagleLogic( const Mat& image){

    //variables
    vector<vector<Point> > points;
    int width = image.cols;
    int height = image.rows;
    int area = width*height;
    int minimumRectangleArea = fabs(area*0.01);
    int thresholdRange = 5;
    float sigma=0.33;

    //step1: blur it by kernal size 9 to reduce the noise
    Mat bluredImage(image);
    medianBlur(image, bluredImage, 9);

    //step4: try different thresholds for ligher to darker color images in each color ranges
    Mat oneChannelImage(bluredImage.size(), CV_8U);
    Mat thresholdImage;
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&bluredImage, 1, &oneChannelImage, 1, ch, 1);
        double v = medianMat(oneChannelImage);
        int lower = (int)std::max(0.0,-0.33*v);
        int upper = (int)std::min(255.0, 0.33*v);
        Canny(oneChannelImage, thresholdImage, lower, upper, 3);

        //dilate(thresholdImage, thresholdImage, Mat(), Point(-1,-1), 1);

        vector<vector<Point> > contours;
        for( int l = 0; l < thresholdRange; l++ )
        {
           if(l>0){
            //first iteration just use canny image, no threshold
            thresholdImage = oneChannelImage >= (l+1)*255/thresholdRange;
           }

            //step5: find contours of each threshold image
            findContours(thresholdImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            //step6: filter contours
            vector<Point> approx;
            for( size_t i = 0; i < contours.size(); i++ )
            {
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.01, true);
                bool rectangleCondition = approx.size() == 4 && fabs(contourArea(Mat(approx))) > minimumRectangleArea && isContourConvex(Mat(approx)) ;
                if(rectangleCondition && angleFilter(approx))
                {
                   points.push_back(approx);
                }
            }
        }
    }

    bluredImage.release();
    oneChannelImage.release();
    thresholdImage.release();

    vector<vector<Point> > rects = filterAllPoints(points, width, height);
    if(rects.size()>0)
      return convertToRectangle(rects[0]);

    return getDefaultRect(width, height);
}

Rectangle findRectangle(const int & addr, const size_t width, const size_t height){
    uint8_t * data = reinterpret_cast<uint8_t *>(addr);
    Mat image(height, width, CV_8UC4, data);
    return findRectagleLogic(image);
}

EMSCRIPTEN_BINDINGS(my_module) {
  register_vector<Rectangle>("Rectangle");
  register_vector<Corner>("Corner");
  register_vector<double>("value");
  emscripten::function("findRectangle", &findRectangle, allow_raw_pointers());
}
