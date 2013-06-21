#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <libplayerc/playerc.h>

#define MAX_X 200
#define MAX_Y 200

int main(int argc, const char **argv) {
    int i;
    int porta = 6665;
    double x, y;
    char livre;
    char end_ip[20];

    // OpenCV Variables  
    char wndname[30] = "Drawing Demo";
    int line_type = CV_AA; // change it to 8 to see non-antialiased graphics
    CvPoint pt1, pt2;
    IplImage* image;
    int width = MAX_X, height = MAX_Y; // 200 x 100 pixels 

    // Player-Stage Variables
    playerc_client_t *client;
    playerc_position2d_t *position2d;
    playerc_laser_t *laser;

    // Create a window
    image = cvCreateImage(cvSize(width, height), 8, 3);
    cvNamedWindow(wndname, 1);
    cvZero(image);
    pt1.x = 100;
    pt1.y = MAX_Y;
    pt2.x = 100;
    pt2.y = MAX_Y - 80;
    cvLine(image, pt1, pt2, CV_RGB(255, 255, 255), 2, line_type, 0);
    pt2.x = 20;
    pt2.y = MAX_Y;
    cvLine(image, pt1, pt2, CV_RGB(255, 255, 255), 2, line_type, 0);
    pt2.x = 180;
    pt2.y = MAX_Y;
    cvLine(image, pt1, pt2, CV_RGB(255, 255, 255), 2, line_type, 0);
    cvShowImage(wndname, image);
    cvWaitKey(1000);

    cvZero(image);
    pt1.x = 20;
    pt1.y = MAX_Y;
    pt2.x = 160;
    pt2.y = MAX_Y - 80;
    cvRectangle(image, pt1, pt2, CV_RGB(255, 255, 255), 2, line_type, 0);
    cvShowImage(wndname, image);
    cvWaitKey(1000);

    strcpy(end_ip, "localhost");

    if (argc >= 2) /* Get Port */
        porta = atoi(argv[1]);
    if (argc >= 3) /* Get IP Address */
        strcpy(end_ip, argv[2]);

    printf("Porta: %d\n", porta);
    printf("IP: %s\n", end_ip);

    client = playerc_client_create(NULL, end_ip, porta);
    if (playerc_client_connect(client) != 0)
        return -1;

    // Connect to Position
    position2d = playerc_position2d_create(client, 0);
    if (playerc_position2d_subscribe(position2d, PLAYERC_OPEN_MODE) != 0) {
        fprintf(stderr, "error: %s\n", playerc_error_str());
        return -1;
    }

    // Enable motor control
    playerc_position2d_enable(position2d, 1);

    // Connect to Laser
    laser = playerc_laser_create(client, 0);
    if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE))
        return -1;

    // Read several times the robot data (delay)
    playerc_client_read(client);
    playerc_client_read(client);
    playerc_client_read(client);
    playerc_client_read(client);
    playerc_client_read(client);

    while (1) {
        playerc_client_read(client);

        // scan for free 100 cm in front of robot
        livre = 1;
        cvZero(image);
        for (i = 0; i < 360; i++) {
            if ((laser->scan[i][0]) < 0.5)
                livre = 0;

            //  Debug: if (laser->scan[i][0] <= 0) printf("#");

            if (laser->scan[i][0] < 7.8) {
                x = laser->scan[i][0] * cos(laser->scan[i][1] + 3.1415926 / 2.0);
                y = laser->scan[i][0] * sin(laser->scan[i][1] + 3.1415926 / 2.0);

                pt1.x = (int) (x * 10 + 100);
                pt1.y = (int) (MAX_Y - y * 10);
                cvCircle(image, pt1, 2, CV_RGB(255, 255, 255), 1, line_type, 0);
            }
        }
        cvShowImage(wndname, image);

        // if free moves, otherwise turns
        if (livre)
            playerc_position2d_set_cmd_vel(position2d, 0.2, 0, 0.0, 1);
        else
            playerc_position2d_set_cmd_vel(position2d, 0.0, 0, 0.4, 1);

        cvWaitKey(10);
    }

    playerc_laser_unsubscribe(laser);
    playerc_laser_destroy(laser);
    playerc_client_disconnect(client);
    playerc_client_destroy(client);

    return 0;
}
