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

    Robot rob("localhost", 6665);
    //Robot rob(argv[1], argv[2]);
    if (!rob.initRobot()) {
        return -1;
    }

    while (true) {

        rob.update();
    }


    return 0;
}

