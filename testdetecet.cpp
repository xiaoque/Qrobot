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

void detect( IplImage* image,CvMemStorage* storage, CvHaarClassifierCascade* cascade);
void show_detecttime( double t);
void draw(IplImage* img, CvSeq* faces, int i,double scale);

int main( int argc, char** argv )
{
	CvMemStorage* storage = 0;
	CvHaarClassifierCascade* cascade = 0;
    CvCapture* capture = 0;
	IplImage *frame, *frame_copy = 0;
	char* cascade_name = "E:\\opencv\\data\\haarcascades\\haarcascade_frontalface_alt2.xml";
 
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );

	//delete[] cascade_name;
	cascade_name = NULL;
 
    if( !cascade )
    {
        return -1;
    }

    storage = cvCreateMemStorage(0);
	capture = cvCaptureFromCAM( 0 );

    cvNamedWindow( "result", 1 );
 
    if( capture )
    {
        for(;;)
        {
            if( !cvGrabFrame( capture ))
                break;

            frame = cvRetrieveFrame( capture );

            if( !frame )
                break;
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );

            detect( frame_copy, storage, cascade);
 
            if( cvWaitKey( 10 ) >= 0 )
                break;
        }

		cvReleaseHaarClassifierCascade(&cascade);
        cvReleaseImage( &frame_copy );
		cvReleaseMemStorage( &storage );
        cvReleaseCapture( &capture );

    }
    else
    {
        fprintf( stderr, "ERROR: Could not find a camera!\n" );
 
    }
 
    cvDestroyWindow("result");
 
    return 0;
}
 
void detect( IplImage* img ,CvMemStorage* storage,CvHaarClassifierCascade* cascade)
{
    double scale = 2;
    IplImage* gray = cvCreateImage( cvSize(img->width,img->height), IPL_DEPTH_8U, 1 );
    IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                         cvRound (img->height/scale)),
                     IPL_DEPTH_8U, 1 );
	
    int i;
 
    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );

    cvEqualizeHist( small_img, small_img );
	//cvShowImage("small",small_img);
    cvClearMemStorage( storage );

			CvPoint frame_center;
			frame_center.x = cvRound(img->width*0.5);
			frame_center.y = cvRound(img->height*0.5);
			cvCircle( img, frame_center, 64,cvScalar(255,0,0), 2, 8, 0 );
 
    if( cascade )
    {
        double t = (double)cvGetTickCount();
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
                                            1.3, 1, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
        
		show_detecttime(t);

        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
           draw( img, faces, i, scale );
        }
    }
 
    cvShowImage( "result", img );
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
}

void show_detecttime(double t )
{
	t = (double)cvGetTickCount() - t;
    printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
}

void draw(IplImage* img, CvSeq* faces, int i, double scale)
{
	CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
    CvPoint face_center;
	CvPoint frame_center;
    int radius;
	double x_gap, y_gap;
	double x_rotate, y_rotate;
	double threshold;

    face_center.x = cvRound((r->x + r->width*0.5)*scale);
    face_center.y = cvRound((r->y + r->height*0.5)*scale);
    radius = cvRound((r->width + r->height)*0.2*scale);

	cvCircle( img, face_center, radius,cvScalar(0,0,255), 3, 8, 0 );
	cvCircle( img, face_center, 0,cvScalar(0,0,255), 3, 8, 0 );

	frame_center.x = cvRound(img->width*0.25*scale);
	frame_center.y = cvRound(img->height*0.25*scale);
	cvCircle( img, frame_center, 1,cvScalar(0,255,0), 3, 8, 0 );
	
	x_gap = face_center.x - frame_center.x;
	y_gap = -(face_center.y - frame_center.y);

	int test = sqrt( x_gap*x_gap + y_gap*y_gap);

	threshold = 64 - sqrt( x_gap*x_gap + y_gap*y_gap);

	if(threshold<0)
	{
		x_rotate = 50 + (50 * (x_gap * 1.0)/640);

		if( y_gap > 0 )
		{
			y_rotate = 100 * (y_gap * 1.0)/240;
		}
		else y_rotate = 0;

		printf("水平转动：%lf；上下转动：%lf\n",x_rotate, y_rotate);
	}
	

}