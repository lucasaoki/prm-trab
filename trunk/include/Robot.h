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

#include <cstdlib>
//#include <string.h>
#include <cstdio>
#include <cmath>

#include <libplayerc/playerc.h>
#include "LocalMap.h"

#define MAX_SPEED 0.15

class Robot {
public:
    Robot(const char *ip, int port);
    Robot(const Robot& orig);
    virtual ~Robot();

    bool initRobot();
    void update();
private:

    bool _free;
    int _port;
    char _ipAdr[20];

    // Player-Stage Variables
    playerc_client_t *_client;
    playerc_position2d_t *_position2d;
    playerc_laser_t *_laser;

    LocalMap _localMap;
};

#endif	/* ROBOT_H */

