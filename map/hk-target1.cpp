#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <libplayerc/playerc.h>

#define MAX_X 800   /* Tamanho da Janela Gráfica */
#define MAX_Y 500   /* 800 x 500 pixels                */

#define IPHOKUYO "localhost"

int main(int argc, const char **argv)
{
  int i;
  int portahk=6665;
  int sc;
  double x,y;
  char livre;
  char end_ip[20];

  // OpenCV Variables  
  char wndname[30] = "Drawing Demo";
  int line_type = CV_AA; // change it to 8 to see non-antialiased graphics
  CvPoint pt1,pt2;
  IplImage* image;
  int width = MAX_X, height = MAX_Y;    // 800 x 500 pixels 

  // Player-Stage Variables
  playerc_client_t *clientHK;
  playerc_laser_t *laser;
  playerc_ranger_t *ranger;

  float LAng[690];
  float LData[690];
  int LMark[690];
  int LDataOk=0;
  int angmin,angmax;
  
  // Create a window
  image = cvCreateImage( cvSize(width,height), 8, 3 );
  cvNamedWindow(wndname, 1 );
  cvZero( image );
  
  // Exibe linhas de referencia na tela
  pt1.x=400;
  pt1.y=MAX_Y;
  pt2.x=400;
  pt2.y=MAX_Y-400;
  cvLine( image, pt1, pt2, CV_RGB(255,255,255), 2, line_type, 0 );
  pt2.x=0;
  pt2.y=MAX_Y;
  cvLine( image, pt1, pt2, CV_RGB(255,255,255), 2, line_type, 0 );
  pt2.x=799;
  pt2.y=MAX_Y;
  cvLine( image, pt1, pt2, CV_RGB(255,255,255), 2, line_type, 0 );
  cvShowImage(wndname,image);
  cvWaitKey(1000);
  // Retangulo de referencia
  cvZero( image );
  pt1.x=0; pt1.y=MAX_Y; pt2.x=799; pt2.y=MAX_Y-400;
  cvRectangle( image,pt1, pt2, CV_RGB(255,255,255), 2, line_type, 0 );
  cvShowImage(wndname,image);
  cvWaitKey(1000);

  strcpy(end_ip,"localhost");

  if (argc >= 2)             /* Get Port */
     portahk=atoi(argv[1]);
  if (argc >= 3)             /* Get IP Address */
     strcpy(end_ip,argv[2]);
  else
    strcpy(end_ip,IPHOKUYO);

  printf("Porta: %d\n",portahk);
  printf("IP: %s\n",end_ip);

   // Create a client and connect it to the server.
   clientHK = playerc_client_create(NULL, end_ip, portahk);
   if (0 != playerc_client_connect(clientHK))
      return -1;

  // Subscrite Ranger and Laser
   //ranger = playerc_ranger_create(clientHK, 0);
   //if (playerc_ranger_subscribe(ranger, PLAYERC_OPEN_MODE))
   //    return -1;

   printf("Warming up ranger...\n"); 
   for (i=1; i<10000; i++) printf("%d\r",i);   
   printf ("\n");

   laser = playerc_laser_create(clientHK, 0);
   if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE))
      return -1;

   printf("Warming up laser...\n"); 
   for (i=1; i<10000; i++) printf("%d\r",i);   
   printf ("\n");
   for (i=0; i < 690; i++) LMark[i]=0;

  // Read several times the robot data (delay)
  playerc_client_read(clientHK);
  playerc_client_read(clientHK);
  playerc_client_read(clientHK);
  playerc_client_read(clientHK);
  playerc_client_read(clientHK);
 
  while(1)
  {
    playerc_client_read(clientHK);

    // scan for free 50 cm in front of robot
    livre=1;
    cvZero( image );

    pt1.x=400;
    pt1.y=MAX_Y;
    pt2.x=400;
    pt2.y=MAX_Y-400;
    cvLine( image, pt1, pt2, CV_RGB(0,0,255), 2, line_type, 0 );

    for (i=85; i < 595; i++)
    {
        if ((laser->scan[i][0]) < 0.5)
           livre=0; 
 
        //  Debug: if (laser->scan[i][0] <= 0) printf("#");

        if (!LDataOk)
        {
            LAng[i]=laser->scan[i][1];
            LData[i]=laser->scan[i][0];
            if (i==594) LDataOk++;
        }     
        else
        { 
            if ((fabs(LData[i] - laser->scan[i][0])) > 0.1)
               LMark[i]=1;
            else
               LMark[i]=0;
        } 

        if (laser->scan[i][0] < 3.95 && laser->scan[i][0] > 0.05) 
        {

           // Converte de coordenada polar (theta,dist) em cartesiana (x,y)
           //  laser->scan[i][1]  => Retorna o valor de theta em radianos
           //  laser->scan[i][0]  => Retorna o valor de dist correspondente ao theta

           x = laser->scan[i][0]*cos(laser->scan[i][1]+3.1415926/2.0);
           y = laser->scan[i][0]*sin(laser->scan[i][1]+3.1415926/2.0);

           // X e Y sao coordenadas locais em referencia ao sensor do robo
           // X e Y valem entre -4.0 e +4.0 (alcance do laser)
           // * 100 = Valor em centimetros de -400 a +400

           pt1.x=(int)(x*100+400);      // +400 para ficar no meio da tela, com -400 ou +400
           pt1.y=(int)(MAX_Y-y*100);  //  Inverte o eixo Y (MAX_Y - valor)

           if (LMark[i]+LMark[i-1]+LMark[i-2]+LMark[i-3] > 2)
              cvCircle( image, pt1, 2, CV_RGB(255,0,0), 1, line_type, 0 );
           else
              cvCircle( image, pt1, 2, CV_RGB(255,255,255), 1, line_type, 0 );

        }
    }
   
   /* Localiza o angulo mais a esquerda e o mais a direita e calcula o ponto medio */ 
   angmin=-1;
   angmax=-1;
   for (i=85; i < 595; i++)
   {
        if ((LMark[i]+LMark[i-1]+LMark[i-2]+LMark[i-3] > 2) && angmin==-1)  angmin=i;
        if (LMark[i]+LMark[i-1]+LMark[i-2]+LMark[i-3] > 2) angmax=i;
   }
   if (angmin >= 0 && angmax >=0)
   {
           i = (angmax - angmin)/2+angmin;
           x = laser->scan[i][0]*cos(laser->scan[i][1]+3.1415926/2.0);
           y = laser->scan[i][0]*sin(laser->scan[i][1]+3.1415926/2.0);
           pt1.x=(int)(x*100+400);     
           pt1.y=(int)(MAX_Y-y*100);  
           /* ESTE EH O ALVO! */
           cvCircle( image, pt1, 4, CV_RGB(0,255,0), 1, line_type, 0 );  
           printf("Angulo: %f\n",((laser->scan[i][1]+3.1415926/2.0)*180)/3.1415926);        
   }

    cvShowImage(wndname,image);

    // cvWaitKey(10);
    if( cvWaitKey( 10 ) >= 0 )
         goto _cleanup_;
  } 

_cleanup_:

  playerc_laser_unsubscribe(laser);
  playerc_laser_destroy(laser);
  playerc_client_disconnect(clientHK);
  playerc_client_destroy(clientHK);

  return 0;
}
