/* 
 * File:   LocalMap.h
 * Author: lucasaoki
 *
 * Created on June 14, 2013, 5:48 PM
 */

#ifndef LOCALMAP_H
#define	LOCALMAP_H

#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY
#ifndef _EiC
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <libplayerc/playerc.h>

#define WIN_WIDTH 400
#define WIN_HEIGHT 400
/* Buscar valores corretos */
#define LASER_LIMIT 3.8
#define TOTAL_LASER 360
#define FIRST_LASER 84
#define LAST_LASER 594

/***************************/

class LocalMap {
public:
    LocalMap();
    LocalMap(const LocalMap& orig);
    virtual ~LocalMap();

    void createWindow();
    void updateMap(playerc_laser_t *laser); //trocar nome?

    // OpenCV Variables  
    char _windowName[30];
private:
    int _line_type; // change it to 8 to see non-antialiased graphics
    CvPoint _pt1, _pt2;
    IplImage* _image;
    int _width, _height;
};

#endif	/* LOCALMAP_H */

