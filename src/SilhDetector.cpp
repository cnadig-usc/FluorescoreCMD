//#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <omp.h>
#include "SilhDetector.h"
#include <fstream>
#include <iostream>
#include <deque>
#include <vector>
#include <math.h>
#include <iomanip>

using namespace std;


#define UPIXEL(img, i, j, k)	(*( (unsigned char*) ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define PIXEL(img, i, j, k)		(*( (char*)          ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))

cSilhDetector::cSilhDetector()
{
	isInited = false;
	bgMean = NULL;
	alpha = (float) (1/30.0);
	//threshold = 0x7F;
	//threshold = -10;


	denoisedChangeMask = NULL;
	changeMask = NULL;
	threshold = 20.0;
}

void cSilhDetector::initialize(IplImage *firstFrame)
{
	if( bgMean != NULL )
	{
		cvReleaseImage(&bgMean);
		bgMean = NULL;
	}
	bgMean = cvCloneImage(firstFrame);		//Take first frame as mean.
	changeMask = cvCreateImage( cvGetSize(firstFrame),IPL_DEPTH_8U, 1);
	cvSetZero(changeMask);
	denoisedChangeMask = cvCloneImage( changeMask);
	isInited = true;

}

inline double cSilhDetector::pixelDistance(unsigned char *thisFrame, unsigned char *bg, int nChannels)
{
	//Can be faster if you don't use double calculations. use ints.
	if(nChannels == 3)
		return (double) ( 0.299 * (((int) thisFrame[2]) - bg[2]) + 0.587 * (((int) thisFrame[1]) - bg[1]) + 0.114 * ( ((int)thisFrame[0]) - bg[0]) );
	else
		return (double) (thisFrame[0] - bg[0]);
}

void cSilhDetector::segmentFrame(IplImage *thisFrame)
{
	//if(isInited == false)
	//	initialize(thisFrame); // I just make sure I am initializing background using calculated bg
//printf("\nbefore segment frame in sildetect");
	double t = (double)cvGetTickCount();
	IplImage* thisFrameClone;
	thisFrameClone = cvCloneImage(thisFrame);

	cvZero(denoisedChangeMask);
	cvZero(changeMask);

	IplImage* omp_changeMask = cvCloneImage(changeMask);
	cvZero(omp_changeMask);
//printf("\n =====================pt1 x = %d y = %d==================",pt1.x, pt1.y);
//printf("\n ==================pt2 x = %d y = %d==================",pt2.x, pt2.y);


	int i,j; // Defined above so as to unable parallelization (OpenMP)
//	cvNamedWindow("thisFrameClone",CV_WINDOW_AUTOSIZE);
//	cvNamedWindow("omp_changeMask",CV_WINDOW_AUTOSIZE);
//	cvNamedWindow("bgMean",CV_WINDOW_AUTOSIZE);
//	cvNamedWindow("thisFrame",CV_WINDOW_AUTOSIZE);



#pragma omp parallel shared(thisFrame, omp_changeMask) private(i,j)		//SHOULD BE SHARING BGMEAN TOO. BUT IS THROWING AN ERROR :'( WHY??? Dunnooo. Default access clause is sharing though.
	{
#pragma omp for schedule(dynamic)
		for( j= min(pt1.y, pt2.y)+1; j<max(pt1.y, pt2.y)-1; ++j) //just look for silhouettes in ROI
		{
			for( i=min(pt1.x, pt2.x)+1; i<max(pt1.x, pt2.x)-1; ++i)

			{
				double res = pixelDistance( &UPIXEL(thisFrame, i, j, 0), &UPIXEL(bgMean, i, j, 0), 3 );
				if (res<threshold)
				{
					for(int k = 0; k < bgMean->nChannels; ++k)
					{
						UPIXEL(bgMean, i, j, k) = (unsigned char) (alpha * UPIXEL(thisFrame, i, j, k) + (1-alpha) * UPIXEL(bgMean, i, j, k));
						// we can uncomment this line to keep modelling background ...
					}
				}
				else
				{
					PIXEL(thisFrameClone, i, j, 0) |= 0xFF;
					PIXEL(thisFrameClone, i, j, 3) |= 0xFF;
					PIXEL(thisFrameClone, i, j, 1) &= 0000;
					PIXEL(omp_changeMask,i, j, 0) |= 0xFF;

				}
			}
		}
//		cvShowImage("thisFrameClone",thisFrameClone);
//		cvShowImage("omp_changeMask",omp_changeMask);
//		cvShowImage("bgMean",bgMean);
//		cvShowImage("thisFrame",thisFrame);
//
	}

	cvReleaseImage(&changeMask);
	changeMask = cvCloneImage(omp_changeMask);

	IplImage* myTemp= cvCloneImage(changeMask);
	//cvZero(myTemp);

	//I DO NOT DENOISE USING MORPHOLOGICAL ANYMORE 
	//cvErode(changeMask,myTemp,NULL,1);
	//cvDilate(myTemp,denoisedChangeMask,NULL,1);

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;


	int Nc = cvFindContours(myTemp,storage,&first_contour,sizeof(CvContour),CV_RETR_EXTERNAL);//	CV_RETR_LIST);
	int immacountingthecontours = 0;

	int width =  myTemp->width;
	int height=  myTemp->height;

//	CvVideoWriter *vw = cvCreateVideoWriter("denoisedChangeMask.avi", CV_FOURCC('X','V','I','D'), 30.0, cvSize(width,height),0);
	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) //make the contourlist I just want to include contours with Area > MINAREAOFCONTOURS;
	{
//		immacountingthecontours++;
		if (fabs(cvContourArea(c)) >  10)
		{
			cvDrawContours(denoisedChangeMask,c, cvScalar(255,255,255), cvScalar(255), -1, CV_FILLED, 8);
		}
		//cout<<"\nno of contours: "<<immacountingthecontours;
//		cvWriteFrame(vw, denoisedChangeMask);
	}
//	cvReleaseVideoWriter(&vw);


//	cvNamedWindow("denoisedChangeMask",CV_WINDOW_AUTOSIZE);
//	cvShowImage("denoisedChangeMask", denoisedChangeMask);
//	cvWaitKey(0);

	cvReleaseMemStorage(&storage);
	t = (double)cvGetTickCount() - t;
	//printf( "\n tiiimmeee = in my silh : %.1f\n", t/(cvGetTickFrequency()*1000.) );
	cvReleaseImage(&myTemp);
	cvReleaseImage(&thisFrameClone);
	cvReleaseImage(&omp_changeMask);




}

void cSilhDetector::finish()
{
	if (bgMean != NULL)
		cvReleaseImage(&bgMean);
	bgMean = NULL;
	isInited = false;
}
