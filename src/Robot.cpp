/* 
 * File:   Robot.cpp
 * Author: lucasaoki
 * 
 * Created on June 14, 2013, 5:07 PM
 */

#include <Robot.h>

using namespace std;

Robot::Robot(const char *ip, int port) {

    cout << "Connecting:" << endl;
    strcpy(_ipAdr, ip);
    _port = port;
    cout << "Ip: " << _ipAdr << endl << "Port: " << _port << endl;


    avg, ang = 0,2;
    get_obj = false;
    det_ = false;
    end = false;
    middle_angle = 343;    
}

Robot::Robot(const Robot& orig) {
}

Robot::~Robot() {

    playerc_laser_unsubscribe(_laser);
    playerc_laser_destroy(_laser);
    playerc_client_disconnect(_client);
    playerc_client_destroy(_client);
}

bool Robot::initRobot() {
    _client = playerc_client_create(NULL, _ipAdr, _port);
    if (playerc_client_connect(_client) != 0) {
        return false;
    }

    // Connect to Position
    _position2d = playerc_position2d_create(_client, 0);
    if (playerc_position2d_subscribe(_position2d, PLAYERC_OPEN_MODE) != 0) {
        fprintf(stderr, "error: %s\n", playerc_error_str());
        return false;
    }

    // Enable motor control
    playerc_position2d_enable(_position2d, 1);

    // Connect to Laser
    _laser = playerc_laser_create(_client, 0);
    if (playerc_laser_subscribe(_laser, PLAYERC_OPEN_MODE)) {
        return false;
    }

    // Read several times the robot data (delay)
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);

    return true;
}

void Robot::update() {

    playerc_client_read(_client);

    _free = true;

    for (int i = FIRST_LASER; i < LAST_LASER; i++) {

        if (_laser->scan[i][0] < 0.5) {
            // _free = false;
        }
    }
    tracking();
    _localMap.updateMap(_laser);

    if (_free) {
        playerc_position2d_set_cmd_vel(_position2d, 0.2, 0, 0.0, 1);
    } else {
        playerc_position2d_set_cmd_vel(_position2d, 0.0, 0, 0.4, 1);
    }
}


void Robot::tracking(){

    int i = 0;
    int LAng = 0, RAng = 0;
    double x, y;
    
    cvZero(image);
    pt1.x = 400;
    pt1.y = WIN_HEIGHT;
    pt2.x = 400;
    pt2.y = WIN_HEIGHT - 400;
    cvLine(image, pt1, pt2, CV_RGB(0, 0, 255), 2, CV_AA, 0);

    zeroMark();
    RAng = LAng = avg;

    while (fabs((double) (_laser->scan[LAng][0] - _laser->scan[LAng - 1][0])) < 0.1) {
        LMark[LAng] = 1;
        LAng--;
    }

    while (fabs(_laser->scan[RAng][0] - _laser->scan[RAng + 1][0]) < 0.1) {
        LMark[RAng] = 1;
        RAng++;
    }

    avg = (LAng + RAng) / 2;

    if(fabs(LAng-RAng) < 0,1){
        det_ = false;
    } else {
        for (i = 85; i < 595; i++) {
            if (_laser->scan[i][0] < 3.95 && _laser->scan[i][0] > 0.05) {

                x = _laser->scan[i][0]
                        * cos(_laser->scan[i][1] + 3.1415926 / 2.0);
                y = _laser->scan[i][0]
                        * sin(_laser->scan[i][1] + 3.1415926 / 2.0);

                pt1.x = (int) (x * 100 + 400); 
                pt1.y = (int) (WIN_HEIGHT - y * 100); 

                if (LMark[i] + LMark[i - 1] + LMark[i - 2] + LMark[i - 3] > 2){
                    if(i == avg){
                        cvCircle(image, pt1, 2, CV_RGB(0, 255, 0), 3, CV_AA, 0);
                    }
                    else
                        cvCircle(image, pt1, 2, CV_RGB(255, 0, 0), 1, CV_AA, 0);
                }
                else
                    cvCircle(image, pt1, 2, CV_RGB(255, 255, 255), 1, CV_AA, 0);
            }
        }
    }
    cvShowImage(_localMap._windowName, image);
    cvWaitKey(10);
    
}
void Robot::zeroMark(){
    for (int i = 0; i < 690; i++)
        LMark[i] = 0;
}
/*
void Robot::lost_detection(IplImage* image){
    boolean livre = true;
    int LAng[690];
    int i, angmin, angmax, LDataOk = 0;
    float LData[690];

    while(get_obj){
        cvZero(image);

        pt1.x = 400;
        pt1.y = WIN_HEIGHT;
        pt2.x = 400;
        pt2.y = WIN_HEIGHT - 400;
        cvLine(image, pt1, pt2, CV_RGB(0, 0, 255), 2, CV_AA, 0);
        get_obj = false;

        for( i = 85; i < 595; i++){
            if((_laser->scan[i][0]) < 0.5)
                livre = false;
            if(!LDataOk){
                LAng[i] =  _laser->scan[i][1];
                LAng[i] =  _laser->scan[i][0];
                if(i==594)
                    LDataOk++;
            }
            else {
                if(fabs(LData[i] - _laser->scan[i][0]))
                    LMark[i] = 1;
                else
                    LMark[i] = 0;
            }

            if (_laser->scan[i][0] < 3.95 && _laser->scan[i][0] > 0.05) {
                x = _laser->scan[i][0]
                        * cos(_laser->scan[i][1] + 3.1415926 / 2.0);
                y = _laser->scan[i][0]
                        * sin(_laser->scan[i][1] + 3.1415926 / 2.0);

                pt1.x = (int) (x * 100 + 400);
                pt1.y = (int) (WIN_HEIGHT - y * 100);

                if (LMark[i] + LMark[i - 1] + LMark[i - 2] + LMark[i - 3] > 2)
                    cvCircle(image, pt1, 2, CV_RGB(255, 0, 0), 1, CV_AA, 0);
                else
                    cvCircle(image, pt1, 2, CV_RGB(255, 255, 255), 1,
                            CV_AA, 0);
            }
        }
        angmin = -1;
        angmax = -1;
        for (i = 85; i < 595; i++) {
            if ((LMark[i] + LMark[i - 1] + LMark[i - 2] + LMark[i - 3] > 2)
                    && angmin == -1)
                angmin = i;
            if (LMark[i] + LMark[i - 1] + LMark[i - 2] + LMark[i - 3] > 2)
                angmax = i;
        }
        if (angmin >= 0 && angmax >= 0) {
            avg = i = (angmax - angmin) / 2 + angmin;
            x = _laser->scan[i][0] * cos(_laser->scan[i][1] + 3.1415926 / 2.0);
            y = _laser->scan[i][0] * sin(_laser->scan[i][1] + 3.1415926 / 2.0);
            pt1.x = (int) (x * 100 + 400);
            pt1.y = (int) (WIN_HEIGHT - y * 100);
            cvCircle(image, pt1, 4, CV_RGB(0, 255, 0), 1, CV_AA, 0);
            get_obj = true;
        }
        cvShowImage(wndname, image);
        cvWaitKey(10);
    }
}
*/
