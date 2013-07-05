/* 
 * File:   main.cpp
 * Author: lucasaoki
 *
 * Created on June 14, 2013, 4-:50 PM
 */

#include <cstdlib>

#include <Robot.h>

/*
 * 
 */
 int main(int argc, char** argv) {

    char ip[20] = "localhost";
    if (argc == 2)
        strcpy(ip, argv[1]);

    Robot rob(ip, 6665);

    if (!rob.initRobot()) {
        return -1;
    }

    rob.zeroMark();
    while(true){
        rob.update();
    }

    return 0;
}

