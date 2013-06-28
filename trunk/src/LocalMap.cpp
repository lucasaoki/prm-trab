/* 
 * File:   LocalMap.cpp
 * Author: lucasaoki
 * 
 * Created on June 14, 2013, 5:48 PM
 */

#include <LocalMap.h>

LocalMap::LocalMap() {

    strcpy(_windowName, "Local Mapping");
    _line_type = CV_AA;
    _width = WIN_WIDTH;
    _height = WIN_HEIGHT;

    createWindow();
}

LocalMap::LocalMap(const LocalMap & orig) {
}

LocalMap::~LocalMap() {
}

void LocalMap::createWindow() {
    _image = cvCreateImage(cvSize(_width, _height), 8, 3);
    cvNamedWindow(_windowName, 1);
    cvZero(_image);
    _pt1.x = 100;
    _pt1.y = WIN_HEIGHT;
    _pt2.x = 100;
    _pt2.y = WIN_HEIGHT - 80;
    cvLine(_image, _pt1, _pt2, CV_RGB(255, 255, 255), 2, _line_type, 0);
    _pt2.x = 20;
    _pt2.y = WIN_HEIGHT;
    cvLine(_image, _pt1, _pt2, CV_RGB(255, 255, 255), 2, _line_type, 0);
    _pt2.x = 180;
    _pt2.y = WIN_HEIGHT;
    cvLine(_image, _pt1, _pt2, CV_RGB(255, 255, 255), 2, _line_type, 0);
    cvShowImage(_windowName, _image);
    cvWaitKey(1000);

    cvZero(_image);
    _pt1.x = 20;
    _pt1.y = WIN_HEIGHT;
    _pt2.x = 160;
    _pt2.y = WIN_HEIGHT - 80;
    cvRectangle(_image, _pt1, _pt2, CV_RGB(255, 255, 255), 2, _line_type, 0);
    cvShowImage(_windowName, _image);
    cvWaitKey(1000);
}

void LocalMap::updateMap(playerc_laser_t * laser) {

    double x, y;

    cvZero(_image);
    for (int i = FIRST_LASER; i < LAST_LASER; i++) {

        //        if (laser->scan[i][0] < LASER_LIMIT) {
        x = laser->scan[i][0] * cos(laser->scan[i][1] + 3.1415926 / 2.0);
        y = laser->scan[i][0] * sin(laser->scan[i][1] + 3.1415926 / 2.0);

        _pt1.x = (int) (x * 40 + WIN_WIDTH / 2);
        _pt1.y = (int) (WIN_HEIGHT - y * 40);
        cvCircle(_image, _pt1, 2, CV_RGB(255, 255, 255), 1, _line_type, 0);
        //        }
    }
    cvShowImage(_windowName, _image);

    cvWaitKey(10);
}
