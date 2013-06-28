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

#define MAX_X 800   
#define MAX_Y 500   

#define IPHOKUYO "localhost"

playerc_client_t *clientHK;
playerc_laser_t *laser;
playerc_ranger_t *ranger;
IplImage* image;
int LMark[690];
CvPoint pt1, pt2;
char wndname[30] = "tracking";
double avg, ang = 0,2;
bool get_obj = false, det_ = false, end = false;
int middle_angle = 343;
double max_speed = 0,15;

void moveRobos_Ang(){
    if(avg > middle_angle){
        playerc_position2d_set_cmd_vel(position2d, max_speed, 0, ang, 1);
    }else

    if(avg < middle_angle)
        playerc_position2d_set_cmd_vel(position2d, max_speed, 0, -1*ang, 1);
    else {

    }
}



#define leng_min 0,1
void obstacle_detection(){
    double minus;
    int i = 214;
    playerc_client_read(clientHK);
    minus = leng_min;
    do{
        playerc_client_read(clientHK);
        minus = laser->scan[i][0];
        for( i = 214 ; i < 500; i++){
             if(laser->scan[i][0] < minus)
                minus = laser->scan[i][0];
        }
        if(minus < leng_min)
            playerc_position2d_set_cmd_vel(position2d, 0, 0, 0, 1);
    } while(minus < leng_min);

}




void lost_detection(IplImage* image){
	boolean livre = true;
	int LAng[690];
	int i, angmin, angmax, LDataOk = 0;
	float LData[690];

	while(get_obj){
		playerc_client_read(clientHK);
		cvZero(image);

		pt1.x = 400;
		pt1.y = MAX_Y;
		pt2.x = 400;
		pt2.y = MAX_Y - 400;
		cvLine(image, pt1, pt2, CV_RGB(0, 0, 255), 2, CV_AA, 0);
		get_obj = false;

		for( i = 85; i < 595; i++){
			if((laser->scan[i][0]) < 0.5)
				livre = false;
			if(!LDataOk){
				LAng[i] =  laser->scan[i][1];
				LAng[i] =  laser->scan[i][0];
				if(i==594)
					LDataOk++;
			}
			else {
				if(fabs(LData[i] - laser->scan[i][0]))
					LMark[i] = 1;
				else
					LMark[i] = 0;
			}

			if (laser->scan[i][0] < 3.95 && laser->scan[i][0] > 0.05) {
				x = laser->scan[i][0]
						* cos(laser->scan[i][1] + 3.1415926 / 2.0);
				y = laser->scan[i][0]
						* sin(laser->scan[i][1] + 3.1415926 / 2.0);

				pt1.x = (int) (x * 100 + 400);
				pt1.y = (int) (MAX_Y - y * 100);

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
			x = laser->scan[i][0] * cos(laser->scan[i][1] + 3.1415926 / 2.0);
			y = laser->scan[i][0] * sin(laser->scan[i][1] + 3.1415926 / 2.0);
			pt1.x = (int) (x * 100 + 400);
			pt1.y = (int) (MAX_Y - y * 100);
			cvCircle(image, pt1, 4, CV_RGB(0, 255, 0), 1, CV_AA, 0);
			get_obj = true;
		}
		cvShowImage(wndname, image);
		cvWaitKey(10);
	}
}


void tracking(){
	int i = 0;
	int LAng = 0, RAng = 0
	double x, y;

	playerc_client_read(clientHK);
	cvZero(image);
	pt1.x = 400;
	pt1.y = MAX_Y;
	pt2.x = 400;
	pt2.y = MAX_Y - 400;
	cvLine(image, pt1, pt2, CV_RGB(0, 0, 255), 2, CV_AA, 0);

	zeroMark();
	RAng = LAng = avg;

	while (fabs((double) (laser->scan[LAng][0] - laser->scan[LAng - 1][0])) < 0.1) {
		LMark[LAng] = 1;
		LAng--;
	}

	while (fabs(laser->scan[RAng][0] - laser->scan[RAng + 1][0]) < 0.1) {
		LMark[RAng] = 1;
		RAng++;
	}

	avg = (LAng + RAng) / 2;

	if(fabs(LAng-RAng) < 0,1){
		det_ = false
	} else {
		for (i = 85; i < 595; i++) {
			if (laser->scan[i][0] < 3.95 && laser->scan[i][0] > 0.05) {

				x = laser->scan[i][0]
						* cos(laser->scan[i][1] + 3.1415926 / 2.0);
				y = laser->scan[i][0]
						* sin(laser->scan[i][1] + 3.1415926 / 2.0);

				pt1.x = (int) (x * 100 + 400);
				pt1.y = (int) (MAX_Y - y * 100);

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
	cvShowImage(wndname, image);
	cvWaitKey(10);

}

void zeroMark(){
	for (i = 0; i < 690; i++)
		LMark[i] = 0;
}

int main(int argc,  const char **argv){
	int i;
	int portahk = 6665;
	double x, y;
	int width = MAX_X, height = MAX_Y; // 800 x 500 pixels
	char end_ip[20];

	// Player-Stage Variables
	playerc_client_t *clientHK;
	playerc_laser_t *laser;
	playerc_ranger_t *ranger;

	// Create a window
	image = cvCreateImage(cvSize(width, height), 8, 3);
	cvNamedWindow(wndname, 1);
	cvZero(image);

	// Exibe linhas de referencia na tela
	pt1.x = 400;
	pt1.y = MAX_Y;
	pt2.x = 400;
	pt2.y = MAX_Y - 400;
	cvLine(image, pt1, pt2, CV_RGB(255, 255, 255), 2, CV_AA, 0);
	pt2.x = 0;
	pt2.y = MAX_Y;
	cvLine(image, pt1, pt2, CV_RGB(255, 255, 255), 2, CV_AA, 0);
	pt2.x = 799;
	pt2.y = MAX_Y;
	cvLine(image, pt1, pt2, CV_RGB(255, 255, 255), 2, CV_AA, 0);
	cvShowImage(wndname, image);
	cvWaitKey(1000);

	cvZero(image);
	pt1.x = 0;
	pt1.y = MAX_Y;
	pt2.x = 799;
	pt2.y = MAX_Y - 400;
	cvRectangle(image, pt1, pt2, CV_RGB(255, 255, 255), 2, CV_AA, 0);
	cvShowImage(wndname, image);
	cvWaitKey(1000);

	strcpy(end_ip, "localhost");

	if (argc >= 2) /* Get Port */
		portahk = atoi(argv[1]);
	if (argc >= 3) /* Get IP Address */
		strcpy(end_ip, argv[2]);
	else
		strcpy(end_ip, IPHOKUYO);

	printf("Porta: %d\n", portahk);
	printf("IP: %s\n", end_ip);

	// Create a client and connect it to the server.
	clientHK = playerc_client_create(NULL, end_ip, portahk);
	if (0 != playerc_client_connect(clientHK))
		return -1;

	printf("Warming up ranger...\n");
	for (i = 1; i < 10000; i++)
		printf("%d\r", i);
	printf("\n");

	laser = playerc_laser_create(clientHK, 0);
	if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE))
		return -1;

	printf("Warming up laser...\n");
	for (i = 1; i < 10000; i++)
		printf("%d\r", i);
	printf("\n");

	playerc_client_read(clientHK);
	playerc_client_read(clientHK);
	playerc_client_read(clientHK);
	playerc_client_read(clientHK);
	playerc_client_read(clientHK);

	lost_detection();
	det_ = true;
	while (!end){
		tracking();
//		movement();
		if(det_){
			lost_detection;
			det_ = true;
		}
	}

	playerc_laser_unsubscribe(laser);
	playerc_laser_destroy(laser);
	playerc_client_disconnect(clientHK);
	playerc_client_destroy(clientHK);

}