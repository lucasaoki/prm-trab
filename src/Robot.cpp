/* 
 * File:   Robot.cpp
 * Author: lucasaoki
 * 
 * Created on June 14, 2013, 5:07 PM
 */

#include "Robot.h"

using namespace std;

Robot::Robot(const char *ip, int port) {

    cout << "Connecting:" << endl;
    strcpy(_ipAdr, ip);
    _port = port;
    cout << "Ip: " << _ipAdr << endl << "Port: " << _port << endl;
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
        return 0;
    }

    // Connect to Position
    _position2d = playerc_position2d_create(_client, 0);
    if (playerc_position2d_subscribe(_position2d, PLAYERC_OPEN_MODE) != 0) {
        fprintf(stderr, "error: %s\n", playerc_error_str());
        return 0;
    }

    // Enable motor control
    playerc_position2d_enable(_position2d, 1);

    // Connect to Laser
    _laser = playerc_laser_create(_client, 0);
    if (playerc_laser_subscribe(_laser, PLAYERC_OPEN_MODE)) {
        return 0;
    }

    // Read several times the robot data (delay)
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
}

void Robot::update() {

    playerc_client_read(_client);

    _free = true;

    for (int i = FIRST_LASER; i < LAST_LASER; i++) {

        if (_laser->scan[i][0] < 0.5) {
            // _free = false;
        }
    }

    _localMap.updateMap(_laser);

    if (_free) {
        playerc_position2d_set_cmd_vel(_position2d, 0.2, 0, 0.0, 1);
    } else {
        playerc_position2d_set_cmd_vel(_position2d, 0.0, 0, 0.4, 1);
    }


}
