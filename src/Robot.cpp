/* 
 * File:   Robot.cpp
 * Author: lucasaoki
 * 
 * Created on June 14, 2013, 5:07 PM
 */

#include <Robot.h>

using namespace std;

Robot::Robot(const char *ip, int port) {

	printf("Connecting:\n");
    strcpy(_ipAdr, ip);
    _port = port;
	printf("Ip: %s\nPort: %d\n", _ipAdr, _port);

    newspeed = 0;
    newturnrate = 0;
    avg, ang = 0,2;
    get_obj = false;
    det_ = false;
    end = false;
    middle_angle = 343;    
    leng_min = 0.7;
}

Robot::Robot(const Robot& orig) {
}

Robot::~Robot() {

    playerc_laser_unsubscribe(_laser);
    //playerc_ranger_unsubscribe(ranger);
    playerc_laser_destroy(_laser);
    //playerc_ranger_destroy(ranger);
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

    //#define RANGER___ 1
    #ifdef RANGER___
    // Subscrite Ranger and Laser
    ranger = playerc_ranger_create(_client, 0);
    if (playerc_ranger_subscribe(ranger, PLAYERC_OPEN_MODE))
        return -1;
    #endif


    int i;
    printf("Warming up laser...\n"); 
    for (i = 0; i < 100000; i++) 
        printf("%d\r", i);
    printf ("\n");

    // Connect to Laser
    _laser = playerc_laser_create(_client, 0);
    if (playerc_laser_subscribe(_laser, PLAYERC_OPEN_MODE)) {
        return false;
    }

    // Read several times the robot data (delay)
    printf("Warming up laser...\n"); 
    for (i = 0; i < 100000; i++) 
        printf("%d\r", i);
    printf ("\n");
    
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    playerc_client_read(_client);
    
    return true;
}


void Robot::update() {

    playerc_client_read(_client);
    if(!emergencyStop()) {
        
        tracking();
        follow();
        if (!det_)
        {
            lost_detection();
        }
        //_localMap.updateMap(_laser);
    }
}

void Robot::tracking(){

    int i = 0;
    int LAng = 0, RAng = 0;
    double x, y;
    
    //playerc_client_read(_client);
    cvZero(_localMap._image);
    
    pt1.x = 200;
    pt1.y = WIN_HEIGHT;
    pt2.x = 200;
    pt2.y = WIN_HEIGHT - 200;
    cvLine(_localMap._image, pt1, pt2, CV_RGB(0, 0, 255), 2, CV_AA, 0);

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

    if(fabs(_laser->scan[avg][0]) > 4){
        
        det_ = false;
        printf("Perdeu o alvo!!!\n");
    } else {

        avg = (LAng + RAng) / 2;
        printf("Tracking do alvo - avg = %d\n", avg);

        for (i = 85; i < 595; i++) {
            if (_laser->scan[i][0] < 3.95 && _laser->scan[i][0] > 0.05) {

                x = _laser->scan[i][0]
                        * cos(_laser->scan[i][1] + 3.1415926 / 2.0);
                y = _laser->scan[i][0]
                        * sin(_laser->scan[i][1] + 3.1415926 / 2.0);

                pt1.x = (int) (x * 40 + WIN_WIDTH / 2); 
                pt1.y = (int) (WIN_HEIGHT - y * 40); 

                if (LMark[i] + LMark[i - 1] + LMark[i - 2] + LMark[i - 3] > 2){
                    if(i == avg){
                        cvCircle(_localMap._image, pt1, 2, CV_RGB(0, 255, 0), 3, CV_AA, 0);
                    }
                    else
                        cvCircle(_localMap._image, pt1, 2, CV_RGB(255, 0, 0), 1, CV_AA, 0);
                }
                else
                    cvCircle(_localMap._image, pt1, 2, CV_RGB(255, 255, 255), 1, CV_AA, 0);
            }
        }
    }
    cvShowImage(_localMap._windowName, _localMap._image);
    cvWaitKey(10);
}

void Robot::zeroMark(){
    for (int i = 0; i < 690; i++)
        LMark[i] = 0;
}

bool Robot::emergencyStop() {

    double smallValue = 999;
    int i = 0;
    //playerc_client_read(_client);

    for (int i = 10; i < 650; i++)
    {
        if (_laser->scan[i][0] < smallValue)
        {
            smallValue = _laser->scan[i][0];
        }
    }

    if (smallValue < MIN_DIST)
    {
        playerc_position2d_set_cmd_vel(_position2d, 0, 0, 0, 1);
        return true;
    }
    return false;
}

void Robot::lost_detection(){

	int LAng[690];
	int i, angmin, angmax, LDataOk = 0;
	float LData[690];
	double x,y;
	get_obj = false;

    //playerc_client_read(_client);
    printf("Passou....\n");

	zeroMark();
	while(!get_obj){
	
		cvWaitKey(10);

		playerc_client_read(_client);
		cvZero(_localMap._image);

		pt1.x = 200;
		pt1.y = WIN_HEIGHT;
		pt2.x = 200;
		pt2.y = WIN_HEIGHT - 200;
		cvLine(_localMap._image, pt1, pt2, CV_RGB(0, 0, 255), 2, CV_AA, 0);

		for( i = 85; i < 595; i++){

			if(!LDataOk){
				LAng[i] =  _laser->scan[i][1];
				LData[i] =  _laser->scan[i][0];
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

                pt1.x = (int) (x * 40 + WIN_WIDTH / 2); 
                pt1.y = (int) (WIN_HEIGHT - y * 40); 

				if (LMark[i] + LMark[i - 1] + LMark[i - 2] + LMark[i - 3] > 2)
					cvCircle(_localMap._image, pt1, 2, CV_RGB(0, 255, 0), 1, CV_AA, 0);
				else
					cvCircle(_localMap._image, pt1, 2, CV_RGB(255, 255, 255), 1,
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

			avg = i = (angmax + angmin) / 2;
			avg_len = _laser->scan[avg][0];

			x = _laser->scan[i][0] * cos(_laser->scan[i][1] + 3.1415926 / 2.0);
			y = _laser->scan[i][0] * sin(_laser->scan[i][1] + 3.1415926 / 2.0);

            pt1.x = (int) (x * 40 + WIN_WIDTH / 2); 
            pt1.y = (int) (WIN_HEIGHT - y * 40); 

			cvCircle(_localMap._image, pt1, 4, CV_RGB(0, 255, 0), 1, CV_AA, 0);
			printf("Angulo: %f\n",
					((_laser->scan[i][1] + 3.1415926 / 2.0) * 180) / 3.1415926);
			
            get_obj = true;
            det_ = true;
		}
		
        cvShowImage(_localMap._windowName, _localMap._image);
		cvWaitKey(10);
	}
}

void Robot::follow() {

    newspeed = 0;
    newturnrate = 0;
    //playerc_client_read(_client);

    if (det_) {
        newspeed = MAX_SPEED;

        int diff = avg - LASER_CENTRAL;
        if (diff < 0) { //está para a direita

            if (diff < -DIST2AVG) //está fora do limite
            {
                newturnrate = -TURNSPD; //vira para a direita
            }
        } else { //está para esquerda

            if (diff > DIST2AVG) //está fora do limite
            {
                newturnrate = TURNSPD; //vira para a esquerda
            }
        }
    }

    if (newspeed > MAX_SPEED)
    {
        newspeed = MAX_SPEED;
    }
    playerc_position2d_set_cmd_vel(_position2d, newspeed, 0, DTOR(newturnrate), 1);         
}
