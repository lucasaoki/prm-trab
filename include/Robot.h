/* 
 * File:   Robot.h
 * Author: lucasaoki
 *
 * Created on June 14, 2013, 5:07 PM
 */

#ifndef ROBOT_H
#define	ROBOT_H

#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <libplayerc/playerc.h>

class Robot {
public:
    Robot();
    Robot(const Robot& orig);
    virtual ~Robot();
private:

};

#endif	/* ROBOT_H */

