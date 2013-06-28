#include "cv.h"
#include "highgui.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
 
#ifdef _EiC
#define WIN32
#endif
 
static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
 
void detect_and_draw( IplImage* image, IplImage* img2 );
 
const char* cascade_name = "haarcascade_frontalface_alt.xml";
/*    "haarcascade_profileface.xml";*/
 
int main(/* int argc, char** argv*/ )
{
    CvCapture* capture = 0;
	IplImage *frame, *frame_temp, *frame_gray, *frame_copy = 0;
    /*int optlen = strlen("--cascade=");
    const char* input_name;*/
 
   /* if( argc > 1 && strncmp( argv[1], "--cascade=", optlen ) == 0 )
    {
        cascade_name = argv[1] + optlen;
        input_name = argc > 2 ? argv[2] : 0;
    }*/
   /* else
    {*/
        cascade_name = "E:\\opencv\\data\\haarcascades\\haarcascade_frontalface_alt2.xml";
        //opencvװ�ú�haarcascade_frontalface_alt2.xml��·��,
       //Ҳ���԰�����ļ�������Ĺ����ļ�����Ȼ����д·����cascade_name= "haarcascade_frontalface_alt2.xml";  
       //����cascade_name ="C:\\Program Files\\OpenCV\\data\\haarcascades\\haarcascade_frontalface_alt2.xml"
   /*     input_name = argc > 1 ? argv[1] : 0;
    }*/
 
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
 
    if( !cascade )
    {
       /* fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        fprintf( stderr,
        "Usage: facedetect --cascade=\"<cascade_path>\" [filename|camera_index]\n" );*/
        return -1;
    }
    storage = cvCreateMemStorage(0);
 
    //if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        capture = cvCaptureFromCAM( 0 );
  /*  else
        capture = cvCaptureFromAVI( input_name ); */
	
 
    cvNamedWindow( "result", 1 );
 
    if( capture )
    {
        for(;;)
        {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
			
			frame_gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
			cvCvtColor(frame,frame_gray,CV_BGR2GRAY);
			frame_temp = frame;
			frame = frame_gray;

			//cvShowImage("frame_gray...", frame_gray ); 
            if( !frame )
                break;
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );
 
            detect_and_draw( frame_copy, frame_temp );
 
            if( cvWaitKey( 10 ) >= 0 )
                break;
        }
 
		cvReleaseImage( &frame );
        cvReleaseImage( &frame_copy );
		//cvReleaseImage( &frame_temp );
		//cvReleaseImage( &frame_gray );
        //cvReleaseCapture( &capture );
    }
    else
    {
        fprintf( stderr, "ERROR: Could not find a camera!\n" );
 
    }
	cvReleaseCapture( &capture);
 
    cvDestroyWindow("result");
 
    return 0;
}
 
void detect_and_draw( IplImage* img, IplImage* img2 )
{
   /* static CvScalar colors[] = 
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };*/
 
    double scale = 1.5;
   // IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                         cvRound (img->height/scale)),
                     8, 1 );
	
    int i;
 
   /* cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );*/

    cvResize( img, small_img, CV_INTER_LINEAR);
    cvEqualizeHist( small_img, small_img );
	cvShowImage("small",small_img);
    cvClearMemStorage( storage );
 
    if( cascade )
    {
        double t = (double)cvGetTickCount();
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
                                            1.5, 1, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
        t = (double)cvGetTickCount() - t;
        printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
            CvPoint center;
            int radius;
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
			cvCircle( img2, center, radius, /*colors[i%8]*/cvScalar(0,0,255), 3, 8, 0 );
        }
    }
 
    cvShowImage( "result", img2 );
    //cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
}

