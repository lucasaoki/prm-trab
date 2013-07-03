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
#include <LocalMap.h>

#define MAX_SPEED 0.15
#define SPEED_ACEL 0.01
#define TURNSPD 20
#define DIST2AVG 25 // feixes em relação ao central
#define MIN_DIST 0.5 //dist min de qualquer objeto
#define LASER_CENTRAL 343

class Robot {
public:
    Robot(const char *ip, int port);
    Robot(const Robot& orig);
    virtual ~Robot();

    bool initRobot();
    void update();
    void follow();
    void tracking();
    bool emergencyStop();
    void zeroMark();
    void lost_detection();
    void obstacle_detection();
    bool det_;
    
private:

    char _ipAdr[20];
    int _port;

    double newspeed;
    double newturnrate;

    int LMark[690];
    int avg;
    int middle_angle;
    bool get_obj, end;
    double ang, leng_min,avg_len;

    // Player-Stage Variables
    playerc_client_t *_client;
    playerc_position2d_t *_position2d;
    playerc_laser_t *_laser;
    playerc_ranger_t *ranger;

    LocalMap _localMap;

    CvPoint pt1, pt2;
    int width, height;
};

#endif	/* ROBOT_H */

