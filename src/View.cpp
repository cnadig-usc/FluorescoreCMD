//#pragma once

#include "view.h"

//#include "setThreshdlg.h"
//#include <omp.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>


//#include <QMessageBox>
//#include <QtGui>

//#define PIXEL(img, i, j, k) *(img->imageData + img->widthStep * i + img->nChannels * j + k)
//#define UPIXEL(img, i, j, k) *( (unsigned char*) (img->imageData + img->widthStep * i + img->nChannels * j + k) )

#define UPIXEL(img, i, j, k)	(*( (unsigned char*) ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define PIXEL(img, i, j, k)		(*( (char*)          ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))


#define Point2D CvPoint2D64f

//IplImage* CView::myMask  = cvCreateImage(cvSize(800,600),8,1); // HardCoded, need to be changed ..
// THIS ALSO GET COMMENDED AFTER CHANGING FROM STATIC TO DYNAMIC


//twoPointsROI temp;
//temp.pt1.x = 0;
//temp.pt1.y = 0;
//temp.pt2.x = 800;
//temp.pt2.y = 600;
//twoPointsROI CView::projROI;// = temp;


//twoPointsROI CView::projROI;
//static twoPointsROI tempROI;
//CView::CView(void):silhDetect(this)

double mypixelDistance(unsigned char *thisFrame, unsigned char *bg, int nChannels)
{
	//Can be faster if you don't use double calculations. use ints.
	if(nChannels == 3)
		return (double) ( 0.299 * (((int) thisFrame[2]) - bg[2]) + 0.587 * (((int) thisFrame[1]) - bg[1]) + 0.114 * ( ((int)thisFrame[0]) - bg[0]) );
	else
		return (double) (thisFrame[0] - bg[0]);
}


Point2D calculateCenterofContour (CvSeq *c) //returns center of a contour, should go to utility file ..
{
	Point2D center;
	double totalX=0.0,totalY=0.0;
	double total=0.0;
	for( int k=0; k<c->total;k++)
	{
		CvPoint* p = (CvPoint*)cvGetSeqElem(c,k);
		totalX += p->x;
		totalY += p->y;
		total++;
	}
	center.x = totalX/total;
	center.y = totalY/total;

	return center;
}


CView::CView(void)
{

	//inAVIFileName = {};
	//outCSVFileName = {};
	isOpen = false;
	toStop = false;
	askAgain = true;
	//cvZero(myMask);
	//	cvNot(myMask,myMask);
	//projROI.pt1 = cvPoint(0,0); // initialize the ROI , s
	//projROI.pt2 = cvPoint(frameWidth,frameHeight);
	isMaskSet = false;
	doDenoising = true;
	saveSeparateSilh = true;
	showInputVideo = true;
	showChangeMask = true;
	showDenoisedChangeMask = true;
	showProcessedArea = true;
	showMask = true;

	saveGreen = true;
	saveBlue = true;
	saveRed = true;
	saveCSVoutput = saveBlue || saveGreen || saveGreen;
	frameCount = 0;
	viewThresh = 20; //default threshold;
	justForDefiningThreshold = false;
	fp_outputCSV = NULL;
	numBins = 4;
	useStatSilh = false;

	showBGModelingProcess = true;

}

CView::~CView(void)
{
}


void CView::processNewFrame(void)
{
	//printf("\nbeginning process new frame in view");
	if (silhDetect.isInited == false)
		silhDetect.initialize(calculatedInitialBG); // I want to make silh to use calculated background instead of using first frame as bg 

	silhDetect.setThreshold( viewThresh); // I use the Threshold Value that I get from Slider Control
	//silhDetect.setThreshold( -10 ); // I use the Threshold Value that I get from Slider Control

	silhDetect.setAlpha(0.05); // this rate was 1/3.0 I'm changin it to 0.05

	//printf("\nbefore segment frame");
	//printf(("\n for mask x = %d y = %d "),myMask->width,myMask->height);
	//printf(("\n pt1 x = %d y = %d "),projROI.pt1.x,projROI.pt1.y );
	//printf(("\n pt2 x = %d y = %d "),projROI.pt2.x,projROI.pt2.y );

	silhDetect.pt1 = projROI.pt1;
	silhDetect.pt2 = projROI.pt2;

	silhDetect.segmentFrame(currFrame); // here I would have the output in vectors..

	//		printf("\nend process new frame in view");


}


void CView::processNewFrame2(void)
{


	//    if (silhDetect.isInited == false)
	//        silhDetect.initialize(currFrame);

	statSilhDetect.setThreshold( (char) (viewThresh)); // I use the Threshold Value that I get from Slider Control
	//silhDetect.setThreshold( -10 ); // I use the Threshold Value that I get from Slider Control
	statSilhDetect.setAlpha(0.5); // this rate was 1/3.0 I'm changin it to 0.05
	statSilhDetect.segmentFrame(currFrame); // here I would have the output in vectors..
}



void CView::closeView()
{
	toStop = true;
	if (fp_outputCSV != NULL)
		fclose(fp_outputCSV);
	silhDetect.finish();
	cvDestroyAllWindows(); //close all windows

	//Close();

	//cvReleaseImage(&currFrame); //check it later
	cvReleaseCapture(&inVideoCapture);
	isOpen = false;

}

//void CView::roiMouseHandler(int events, int x, int y, int flags, void* param)
//{
//
//	IplImage* img = (IplImage*) param;
//	static IplImage* roiTemp = cvCloneImage(img);
//	static CvPoint startPt;
//	//	CvPoint endPt;
//	static bool firstRightFlag;
//	static bool upflag;
//	switch(events)
//	{
//	case CV_EVENT_RBUTTONDOWN:
//		firstRightFlag = true;
//		startPt.x = x;
//		startPt.y = y;
//	case CV_EVENT_RBUTTONUP:
//		upflag = !upflag;
//		if (!upflag)
//		{
//			//tempROI.pt1 = startPt;
//			projROI.pt1 = startPt;
//			projROI.pt2 = cvPoint(x,y);
//			//tempROI.pt2 = cvPoint(x,y);
//		}
//		if (firstRightFlag == true)
//		{
//			firstRightFlag = false;
//			// tempROI.pt1 = startPt;
//			//   CView::projROI.pt1 = startPt;
//			//   CView::projROI.pt2 = cvPoint(x,y);
//			//tempROI.pt2 = cvPoint(x,y);
//			if( cvWaitKey(1)== 13 )
//			{
//				cvDestroyWindow("MYROI");
//				return;
//			}
//		}
//	}
//	if (flags & CV_EVENT_FLAG_RBUTTON)
//	{
//		if (!firstRightFlag)
//		{
//			switch(events){
//	case CV_EVENT_MOUSEMOVE:
//		roiTemp = cvCloneImage(img); //make a fresh copy of img for roiTemp, (delete the previous ROI)
//		cvRectangle(roiTemp,startPt,cvPoint(x,y),CV_RGB(255,255,0) ,3,IPL_DEPTH_8U);
//		cvNamedWindow("MYROI");
//		cvShowImage("MYROI",roiTemp);
//			}
//		}
//	}
//	return;
//}
//

void onTrackbarSlide(int thresh)
{



}


//void CView::maskMouseHandler(int events, int x, int y, int flags, void* param)
//{
//	static CvPoint startPt;
//	static bool firsttime;
//
//	IplImage* thickFrame = (IplImage*) param;
//
//	//	printf(" counter = %d I am here event = %d x,y = %d %d, flag = %d",cnt,events,x,y,flags);
//	//std::cout<<std::endl<<"counter = "<<cnt<<" event = "<<events;
//
//	switch(events){
//	case CV_EVENT_MOUSEMOVE:
//		if (flags & CV_EVENT_FLAG_LBUTTON)
//		{
//			if (!firsttime)
//			{
//				startPt.x = x;
//				startPt.y = y;
//			};
//			firsttime = true;
//			cvLine(thickFrame,startPt,cvPoint(x,y),cvScalar(255,0,0),2);
//			cvLine(myMask,startPt,cvPoint(x,y),cvScalar(255),1);
//			startPt.x = x;
//			startPt.y = y;
//			cvShowImage("SRCWITHMASK",thickFrame);
//			cvNamedWindow("DefinedMask");
//			cvShowImage("DefinedMask", myMask);
//
//			startPt.x = x;
//			startPt.y = y;
//			break;
//		}
//		if( cvWaitKey(1)== 13 )
//		{
//			cvDestroyWindow("SRCWITHMASK");
//			cvDestroyWindow("DefinedMask");
//		}
//		break;
//	case CV_EVENT_LBUTTONUP:
//		firsttime = false;
//		break;
//	}
//	return;
//}


//
//void CView::setViewROI()
//{
//	IplImage *thisFrame;//, *thisClone;//, *roiTemp;
//
//	thisFrame = cvCreateImage(cvSize(frameWidth,frameHeight),8,3);
//	//for (int i = 0;i<10;i++)
//	thisFrame = cvQueryFrame(inVideoCapture); // grab the 10th frame
//	cvNamedWindow("MYROI");
//	cvShowImage("MYROI", thisFrame);
//	//cvSetMouseCallback("MYROI",&(CView::roiMouseHandler),thisFrame);
//	printf("\n I am before callback in setMask");
//	cvSetMouseCallback("MYROI",&(CView::roiMouseHandler),thisFrame);
//
//
//	printf("\n bfore releasing this frame");
//	cvWaitKey();
//	printf("\nafter cvWaitkey");
//	//projROI = tempROI;
//	cvDestroyWindow("MYROI");
//	printf("\nafter destrywindow");
//	//cvReleaseImage(&thisFrame);
//	printf("\nI am at the end of setViewROI");
//}

// To set Mask for taking out glares
//void CView::setMask(void)
//{
//	cvZero(myMask);
//	//cvZero(viewMask);
//	isMaskSet = true;
//	IplImage *myFrame1, *tmpdilate;
//	tmpdilate = cvCreateImage(cvSize(frameWidth,frameHeight),8,1);
//	//for (int i = 0;i<10;i++)
//	myFrame1 = cvQueryFrame(inVideoCapture); // grab the 10th frame
//	cvNamedWindow("SRCWITHMASK");
//	cvShowImage("SRCWITHMASK", myFrame1);
//	//cvSetMouseCallback("SRCWITHMASK",&(CView::maskMouseHandler),myFrame1);
//
//	cvSetMouseCallback("SRCWITHMASK",&(CView::maskMouseHandler),myFrame1);
//	cvWaitKey();
//
//	cvDilate(myMask, tmpdilate, NULL,1);
//
//	//cvErode(myMask, tmpdilate, NULL,1);
//	//cvNamedWindow("DilatedMask");
//	//cvShowImage("DilatedMask",tmpdilate);
//
//	// finding contours in Dilated mask and filling them
//
//	CvMemStorage* storage = cvCreateMemStorage();
//	CvSeq* first_contour = NULL;
//
//	int Nc = cvFindContours(tmpdilate,storage,&first_contour,sizeof(CvContour),CV_RETR_EXTERNAL);//	CV_RETR_LIST);
//
//	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) {
//		cvDrawContours( myMask, c, cvScalar(255), cvScalar(255), -1, CV_FILLED, 8);
//		// cvDrawContours(viewMask,c,cvScalar(255), cvScalar(255), -1, CV_FILLED, 8);
//	}
//	cvNamedWindow("Final Mask");
//	cvShowImage( "Final Mask", myMask );
//
//	cvWaitKey(0);
//
//	viewMask = cvCloneImage(myMask);
//
//	cvReleaseImage(&tmpdilate);
//	cvReleaseMemStorage(&storage);
//	cvDestroyAllWindows();
//}
//
//

//bool CView::readNewFrame(void)
//{
//    printf("\n at the begining of read new frame");
//    //if(currFrame!=NULL) // Relese currFrame, if some frame has been previously grabbed.
//    //    cvReleaseImage(&currFrame);
//    //currFrame = NULL;
//    IplImage *temp = cvQueryFrame(inVideoCapture); // Read the next frame ("temp" points to internal OpenCV memory and should not be freed : See OpenCV doc on cvQueryFrame() )
//    if(temp==NULL) // Some error occured in grabbing frame from AVI
//        return false;

//	printf("\n Im before cloning");
//    currFrame = cvCloneImage(temp); // Copy the temp image to currFrame
//    return true;

//}

////////////////////////////////////////////////

bool CView::readNewFrame(unsigned long frameIndex)
{
	if(isOpen == false) // Cannot grab if view is not open
		return (readFrameResult = false);

	//        if(currFrame!=NULL) // Relese currFrame, if some frame has been previously grabbed.
	//            cvReleaseImage(&currFrame);
	//        currFrame = NULL;

	//    printf("\n frameIndex in readNewFrame in View %d is %d",viewID,frameIndex);
	if((long)frameIndex>=0L && (long)frameIndex<numFrames) // <==> if(Valid Frame number)
		cvSetCaptureProperty(inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(frameIndex));
	else
		return (readFrameResult = false); // Cannot grab .. illegal frame number

	//    IplImage *temp = cvQueryFrame(inVideoCapture); // Read the next frame ("temp" points to internal OpenCV memory and should not be freed : See OpenCV doc on cvQueryFrame() )
	//    if(temp==NULL) // Some error occured in grabbing frame from AVI
	//        return (readFrameResult = false);
	//      currFrame = cvCloneImage(temp); // Copy the temp image to currFrame
	currFrame = cvQueryFrame(inVideoCapture);

	if(currFrame==NULL) // Some error occured in grabbing frame from AVI
		return (readFrameResult = false);
	return (readFrameResult = true); // Success
}
//////////////////////////////////////////////////


bool CView::openView(void)
{
	inVideoCapture= cvCreateFileCapture(inAVIFileName); // Create the file capture

	if(inVideoCapture == NULL) // Cannot open File
	{

		printf("\ncan not open input file %s",inAVIFileName);
		getchar();
		//QMessageBox msgBox2;
		//msgBox2.setText("can not open input file");
		//msgBox2.exec();
		return false;
	}


	frameHeight   = (int) cvGetCaptureProperty(inVideoCapture, CV_CAP_PROP_FRAME_HEIGHT);
	frameWidth   = (int) cvGetCaptureProperty(inVideoCapture, CV_CAP_PROP_FRAME_WIDTH);
	numFrames = (int) cvGetCaptureProperty(inVideoCapture,  CV_CAP_PROP_FRAME_COUNT); // will use it later ...

	isOpen = true;
	return true;

}

bool CView::initProcessing (void)
{

	//   QMessageBox msgBox;
	//   char str[100];
	//   sprintf(str,"do you want to set a Mask for processing for view %d?",viewID);
	//   msgBox.setText(str);
	//   msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	//   msgBox.setDefaultButton(QMessageBox::Yes);
	//   int ret = msgBox.exec();
	//   switch (ret) 
	//{
	//   case QMessageBox::Yes:
	//       setMask();
	//       break;
	//   case QMessageBox::No:
	//       viewMask = cvCreateImage(cvSize(frameWidth,frameHeight),8,1);
	//   default:
	//       // should never be reached
	//       break;
	//   }


	//QMessageBox msgBox2;
	//char str2[100];
	//sprintf(str2,"do you want to set ROI for view %d?",viewID);
	//msgBox2.setText(str2);
	//msgBox2.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	//msgBox2.setDefaultButton(QMessageBox::Yes);
	//int ret2 = msgBox2.exec();
	//switch (ret2) {
	//case QMessageBox::Yes:
	//    setViewROI();
	//    silhDetect.pt1 = projROI.pt1;
	//    silhDetect.pt2 = projROI.pt2;
	//    break;
	//case QMessageBox::No:
	//    projROI.pt1 = cvPoint(0,0); // initialize the ROI , I think it should go to contructor of the project
	//    projROI.pt2 = cvPoint(frameWidth,frameHeight);
	//    silhDetect.pt1 = projROI.pt1;
	//    silhDetect.pt2 = projROI.pt2;
	//default:
	//    // should never be reached
	//    break;
	//}
	//printf("\n pt1.x = %d, pt1.y = %d, pt2.x = %d, pt2.y = %d",projROI.pt1.x,projROI.pt1.y,projROI.pt2.x,projROI.pt2.y);
	//silhDetect.isInited = false; // to make sure we get the first frame as bgmean ..

	return true;
}

void CView::writeHeaderinCSV()
{
	if (outCSVFileName == "")
	{
		//QMessageBox msg;
		//msg.setText("cannot write to output file...");
		//msg.exec();

		printf("\ncannot write to output file...");
		getchar();
		return;
	}

	if (saveCSVoutput || true)  /////CHANGE THISS!! QUICK HACK TO CALL SAVE FUNCTIOM
	{
		fp_outputCSV = fopen(outCSVFileName,"w");

		fprintf(fp_outputCSV,"frameNum, Nc,");

		if (saveBlue)
			for (int nb = 1;nb<=numBins;nb++)
			{
				char temp[20];
				sprintf(temp,"b%dNum,b%dSum,",nb,nb);
				fprintf(fp_outputCSV,"%s",temp);
				//				printf("%s",temp);
			}

		//fprintf(fp_outputCSV,"b1Num,b1Sum,b2Num,b2Sum,b3Num,b3Sum,b4Num,b4Sum,");
		if (saveGreen)
			for (int nb = 1;nb<=numBins;nb++)
			{
				char temp[20];
				sprintf(temp,"g%dNum,g%dSum,",nb,nb);
				fprintf(fp_outputCSV,"%s",temp);
				//					printf("%s",temp);
			}

		//fprintf(fp_outputCSV,"g1Num,g1Sum,g2Num,g2Sum,g3Num,g3Sum,g4Num,g4Sum,");
		if (saveRed)
			for (int nb = 1;nb<=numBins;nb++)
			{
				char temp[20];
				sprintf(temp,"r%dNum,r%dSum,",nb,nb);
				fprintf(fp_outputCSV,"%s",temp);
				//						printf("%s",temp);
			}

		//        fprintf(fp_outputCSV,"r1Num,r1Sum,r2Num,r2Sum,r3Num,r3Sum,r4Num,r4Sum,");

		fprintf(fp_outputCSV, "totalNumOfPixels,x,y");

		//       fclose(fp_outputCSV);


	}

}

void CView::writeCurrFrameInfo2CSV(void)
{
	// first check if the output file is open..
	//	cout<<endl<<"writecurrframeinfo2csv view begin";
	if (fp_outputCSV == NULL)
		writeHeaderinCSV();


	if (currFrameInfo.rChannel.size() == 0)  // if nothing has been detected .. rChannel could be anything here ..put zero in the output file
	{

		int nch = (int)(saveBlue + saveGreen + saveRed);
		fprintf(fp_outputCSV,"\n%d,",frameCount);
		for (int i = 0;i<1+nch*2*numBins+1+1+1;i++)
			fprintf(fp_outputCSV,"0,");
	}

	for (int j = 0;j<currFrameInfo.rChannel.size();j++) // a loop on number of detected components
	{
		fprintf(fp_outputCSV,"\n%d,",frameCount);
		fprintf(fp_outputCSV,"%d,",currFrameInfo.rChannel.size());// number of detected components

		if(saveBlue)
		{
			for (int k = 0;k<currFrameInfo.rChannel[j].size();k++)
				fprintf(fp_outputCSV,"%d," ,currFrameInfo.bChannel[j][k]);
		}

		if(saveGreen)
		{

			for (int k = 0;k<currFrameInfo.bChannel[j].size();k++)
				fprintf(fp_outputCSV,"%d," ,currFrameInfo.gChannel[j][k]);
		}

		if (saveRed)
		{
			for (int k = 0;k<currFrameInfo.gChannel[j].size();k++)
				fprintf(fp_outputCSV,"%d," ,currFrameInfo.rChannel[j][k]);
		}

		for (int k = 0;k<currFrameInfo.pos_size[j].size();k++)
			fprintf(fp_outputCSV,"%.2lf," ,currFrameInfo.pos_size[j][k]);

	}

	frameCount++;


}


void CView::extractCurrFrameInfo(void)
{

	IplImage* processMask;
	processMask = cvCloneImage(myMask);
	///cvSaveImage("myMaskInExtractCurrFrameInfo.jpg",myMask);

	//IplImage* changeMask;
	// IplImage* denoisedChangeMask;
	//IplImage* myTemp;
	IplImage* currFrameClone;
	currFrameClone = cvCloneImage(currFrame);

	//changeMask = cvCloneImage(c
	// changeMask = cvCreateImage( cvGetSize(currFrame),IPL_DEPTH_8U, 1); //Reza: these can be single channel, coz we just care about changes/ later: now it is! :D
	//cvZero(changeMask);
	//denoisedChangeMask = cvCreateImage( cvGetSize(currFrame),IPL_DEPTH_8U, 1);
	//myTemp = cvCreateImage( cvGetSize(currFrame),IPL_DEPTH_8U, 1);

	////////////////////////////////FROM HERE I WANT TO SAVE RGB HISTOGRAMS FOR EACH INDIVIDUALS

	IplImage *processedArea, *notProcessMask;
	processedArea = cvCreateImage(cvSize(currFrame->width, currFrame->height),IPL_DEPTH_8U, 1);
	notProcessMask = cvCreateImage(cvSize(currFrame->width, currFrame->height),IPL_DEPTH_8U, 1);
	//processmask is 1 in the pixels that we want to remove (glare) So I make a NOT version of that here
	cvNot(processMask,notProcessMask);

	if (!useStatSilh)
		cvAnd(notProcessMask,silhDetect.denoisedChangeMask,processedArea);
	else
		cvAnd(notProcessMask,statSilhDetect.bg_model->foreground,processedArea);

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;




	//        cvNamedWindow("dcm");
	//        cvShowImage("dcm",silhDetect.denoisedChangeMask);



	if (viewProcessedArea != NULL)
		cvReleaseImage(&viewProcessedArea);
	viewProcessedArea = cvCloneImage(processedArea);

	//CvRect rect;
	//rect.x = projROI.pt1.x;
	//rect.y = projROI.pt1.y;
	//rect.height = projROI.pt2.y-projROI.pt1.y;
	//rect.width = projROI.pt2.x-projROI.pt1.x;
	//cvSetImageROI( processedArea,rect  );


	int Nc = cvFindContours(processedArea,storage,&first_contour,sizeof(CvContour),CV_RETR_EXTERNAL);//	CV_RETR_LIST);

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) //{
		cvDrawContours( processedArea, c, cvScalar(255), cvScalar(255), -1, CV_FILLED, 8);

	CvBox2D box;
	CvPoint2D32f box_vtx[4];
	CvPoint pt, pt0;
	//int cnt = 0;

	currFrameInfo.bChannel.clear();
	currFrameInfo.gChannel.clear();
	currFrameInfo.rChannel.clear();
	currFrameInfo.pos_size.clear();

	//////////////////before filling it up/////////////////////


	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next )
	{
		Point2D pos;
		pos = calculateCenterofContour(c);


		CvPoint TopLeft, RightBut;
		box = cvMinAreaRect2( c, 0 );
		cvBoxPoints( box, box_vtx );

		pt0.x = cvRound(box_vtx[3].x);
		pt0.y = cvRound(box_vtx[3].y);
		TopLeft.x = pt0.x;	TopLeft.y = pt0.y;	RightBut.x = pt0.x;	RightBut.y = pt0.y;
		for( int i = 0; i < 4; i++ )
		{
			pt.x = cvRound(box_vtx[i].x);
			pt.y = cvRound(box_vtx[i].y);
			if (TopLeft.x > pt.x)
				TopLeft.x = pt.x;
			if (TopLeft.y > pt.y)
				TopLeft.y = pt.y;
			if (RightBut.x < pt.x)
				RightBut.x = pt.x;
			if (RightBut.y < pt.y)
				RightBut.y = pt.y;
			//cvLine(processedArea, pt0, pt, cvScalar(100), 1, CV_AA, 0);
			pt0 = pt;


			//STUPID BOX FIND GIVES SOME VALUES WHICH ARE OUT OF BOUNDRY SOME TIME, IN NEEDS TO BE CHECKED HERE

			if (RightBut.y>currFrame->height)
				RightBut.y = currFrame->height;
			if (RightBut.y<0)
				RightBut.y = 0;


			if (RightBut.x>currFrame->width)
				RightBut.x = currFrame->width;
			if (RightBut.x<0)
				RightBut.x = 0;


			if (TopLeft.y>currFrame->height)
				TopLeft.y = currFrame->height;
			if (TopLeft.y<0)
				TopLeft.y = 0;


			if (TopLeft.x>currFrame->width)
				TopLeft.x = currFrame->width;
			if (TopLeft.x<0)
				TopLeft.x = 0;


		}

		///CALCULATE CENTER OF THE CONTOUR AND RECORD IT



		//////CALCULATE THE HISTOGRAM FOR CURRENT CONTOUR, SEARCH LOCALLY USING TOP-LEFT AND RIGHT-BUT POINTS///


		//printf("\n^^^^^^^^^^^^%d^^^^^^^^^^^^^^",cnt++);
		int totalNumOfDetectedPixels = 0;
		int i, j;

		//for(int k = 0; k < currFrameClone->nChannels; ++k) // k is for channels, it was currFrameClone->nChannels-1, but now in currFrameClone nchannel = 3 so
		//         {
		//                 for(int l = 0; l < 2*numBins; l+=2)
		//                 {
		//                          bins[k][l] = -1;
		//                         bins[k][l+1] = -1;

		//                 }

		//}
		std::vector<std::vector<int> >bins;
		for(int i=0; i < 3; ++i)

		{
			std::vector <int> subbin;
			subbin.resize( 2 * numBins, 0);
			bins.push_back(subbin);
		}

		std::vector <double> threshholds;
		for(int i=0; i<numBins; ++i)
			threshholds.push_back( (i + 1) * 256/numBins );

		for(j= TopLeft.y +1 ; j<= RightBut.y-1; j++)
		{
			for(i=TopLeft.x+1; i<= RightBut.x-1; i++)
			{

				if ((PIXEL(silhDetect.denoisedChangeMask,i,j,0) !=0) &&  (PIXEL(processMask,i,j,0) == 0))
				{
					totalNumOfDetectedPixels++;
					for(int k = 0; k < currFrameClone->nChannels; ++k) // k is for channels, it was currFrameClone->nChannels-1, but now in currFrameClone nchannel = 3 so
					{
						for(int l = 0; l < 2*numBins; l+=2)
						{
							if( (int) UPIXEL(currFrameClone, i, j, k) < threshholds[l/2])
							{
								bins[k][l]++;
								bins[k][l+1] += (int) UPIXEL(currFrameClone, i, j, k);
								break;
							}
						}
					}
				}

			}
		}
		if (frameCount != 0)
		{
			currFrameInfo.bChannel.push_back(bins[0]);
			currFrameInfo.gChannel.push_back(bins[1]);
			currFrameInfo.rChannel.push_back(bins[2]);
			std::vector <double> temptotalxy;
			temptotalxy.push_back(totalNumOfDetectedPixels);
			temptotalxy.push_back(pos.x);
			temptotalxy.push_back(pos.y);
			currFrameInfo.pos_size.push_back(temptotalxy);
			//            printf("\n%d******************total num of pixels = %d********************************",frameCount,totalNumOfDetectedPixels);
			//            printf("\n ppoooozzzz is :");
			//            for (int j = 0;j<currFrameInfo.pos_size.size();j++)
			//            {
			//                for (int k = 0;k<currFrameInfo.pos_size[j].size();k++)
			//                    printf("%.2lf," ,currFrameInfo.pos_size[j][k]);
			//                printf("\n");
			//            }
			//            printf("\n*********************************************************");
		}
	}
	cvReleaseImage(&currFrameClone);
	cvReleaseImage(&processedArea);
	cvReleaseImage(&notProcessMask);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&processMask);



	//////////////////////////////////
}
void CView::show(void)
{
	char mycaption1[100];
	char mycaption2[100];
	char mycaption3[100];
	char mycaption4[100];
	char mycaption5[100];

	sprintf(mycaption1,"ChangeMask View%d",viewID);
	sprintf(mycaption2,"Denoised changeMask View%d",viewID);
	sprintf(mycaption3,"Input video View%d",viewID);
	sprintf(mycaption4,"Mask View%d",viewID);
	//cvNamedWindow(mycaption1);
	//cvNamedWindow(mycaption2);
	//cvNamedWindow(mycaption3);
	//cvNamedWindow(mycaption4);

	sprintf(mycaption5, "processedArea %d",viewID);




	//if (useStatSilh)
	//	showChangeMask = false;


	if (showChangeMask)
	{
		cvNamedWindow(mycaption1);
		cvShowImage(mycaption1,silhDetect.changeMask);
	}
	else
		cvDestroyWindow(mycaption1);


	if (showDenoisedChangeMask)
	{
		cvNamedWindow(mycaption2);
		if (silhDetect.denoisedChangeMask != NULL /*&& !useStatSilh*/)
			cvShowImage(mycaption2,silhDetect.denoisedChangeMask);

		//if (statSilhDetect.bg_model->foreground != NULL && useStatSilh)
		//      cvShowImage(mycaption2,statSilhDetect.bg_model->foreground);

	}
	else
		cvDestroyWindow(mycaption2);


	if (showInputVideo)
	{
		cvNamedWindow(mycaption3);
		cvShowImage(mycaption3,currFrame);
	}
	else
		cvDestroyWindow(mycaption3);

	sprintf(mycaption4,"Mask View%d",viewID);
	if (showMask)
	{
		cvNamedWindow(mycaption4);
		cvShowImage(mycaption4,myMask);
		cvShowImage(mycaption5, viewProcessedArea);

	}
	else
	{
		cvDestroyWindow(mycaption4);
		cvDestroyWindow(mycaption5);

	}


	cvWaitKey(showMask||showInputVideo||showDenoisedChangeMask||showChangeMask);
}

void CView::setThreshold(void)
{

	///this is for command line version .... without any GUI

	//  cvCreateTrackbar( "threshHold", "toSetThreshold", &pos, 127,&onTrackbarSlide);
	//setThreshDlg myThreshDlg;
	//myThreshDlg.show();

	//justForDefiningThreshold = true;

	//int cnt = 0;
	//while(frameCount<numFrames && !myThreshDlg.toStopDlg)
	//{
	//    printf("\n cnt = %d", cnt);
	//    readNewFrame(cnt);
	//    viewThresh = ((char)myThreshDlg.myDlgResults.sliderValue);
	//    processNewFrame();
	//    printf("\ni= %d, slidervalue is = %d",frameCount,viewThresh);
	//    showChangeMask = (myThreshDlg.myDlgResults.showChangeMask == Qt::Checked)? true:false;
	//    showDenoisedChangeMask = (myThreshDlg.myDlgResults.showDenoisedChangeMask == Qt::Checked)? true:false;
	//    showInputVideo = (myThreshDlg.myDlgResults.showInputVideo == Qt::Checked)? true:false;
	//    showMask = (myThreshDlg.myDlgResults.showMask == Qt::Checked)? true:false;
	//    show();
	//    cvWaitKey(10);
	//    cnt++;
	//}
	//printf("\nthis view tresh will be %d", viewThresh);
	//// here threshold has been already set (viewThresh has the value)
	//cvDestroyAllWindows();
	//// justForDefiningThreshold = false;
	// silhDetect.isInited = false; //put silhDetect back to previous state to make sure the bgmean is being set correctly





}

void CView::printViewSettings()
{

	//printf("\n****these are view setting****\n");
	//char temp[100];
	//sprintf(temp, "view %d:",viewID);
	//viewSettings = temp;
	//viewSettings.append("\ninput file : ");
	//viewSettings.append(inAVIFileName);
	//viewSettings.append("\nThreshold : ");
	//sprintf(temp, "%d",viewThresh);
	//viewSettings.append(temp);
	//printf("\n%s",viewSettings);

}

void CView::calculateInitialBG(int nFramesBG)
{
	if(!isOpen)
		openView();
	cout<<endl<<"in calculateInitialBG";
	cvQueryFrame(inVideoCapture);

	IplImage *tmp_frame = cvQueryFrame(inVideoCapture); // Read the next frame ("temp" points to internal OpenCV memory and should not be freed : See OpenCV doc on cvQueryFrame() )
	int count = 1;


	if (tmp_frame == NULL) {
		cout<<"tmp_frame is null";
	}

	IplImage *currbgMean = cvCloneImage(tmp_frame);
	cvSetZero (currbgMean);

	CvPoint pt1,pt2;
	pt1.x = 0;	pt1.y = 0;
	pt2.x = tmp_frame->width;
	pt2.y = tmp_frame->height;

	IplImage *bgcurFrame = cvCloneImage(tmp_frame);
	cvSetZero (bgcurFrame);
	IplImage  *prevFrame;

	prevFrame = cvCloneImage(bgcurFrame);

	double bgIniThreshold = 10;
	double alpha;

	unsigned long int maxFramesInFile = (unsigned long int)cvGetCaptureProperty(inVideoCapture,CV_CAP_PROP_FRAME_COUNT); // Set the maxFramesInFile
	// I assume maxFramesInFile > 500 ...

	int n = 0;
	char mycaption1[100];
	char mycaption2[100];

	sprintf(mycaption1,"curFrame View%d",viewID);
	sprintf(mycaption2,"currbgMean View%d",viewID);

	for (int myfc = 0;myfc<maxFramesInFile;myfc+=(int)(maxFramesInFile/nFramesBG))
	{
		n++;
		printf("\n%d/%d",n, nFramesBG);

		cvSetCaptureProperty(inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(myfc));
		tmp_frame = cvQueryFrame(inVideoCapture); // Read the next frame ("temp" points to internal OpenCV memory and should not be freed : See OpenCV doc on cvQueryFrame() )

		if(tmp_frame==NULL) // Some error occured in grabbing frame from AVI
		{
			printf("\n an error occured in background modeling part");
			break;
		}

		//	if (!bgcurFrame)
		cvReleaseImage(&bgcurFrame);

		bgcurFrame = cvCloneImage(tmp_frame);

		//cvWaitKey(-1);
		cvNamedWindow("bgcurFrame", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("currbgMean", CV_WINDOW_AUTOSIZE);
		if (myfc ==0)
		{
			cvReleaseImage(&prevFrame);
			prevFrame = cvCloneImage(bgcurFrame);
			continue;
		}
		else
		{

			alpha = 1.0/(n+1);
			////////////////////////for one frame/////////////////////////////////
			int i,j; // Defined above so as to unable parallelization (OpenMP)
#pragma omp parallel shared(bgcurFrame,currbgMean, prevFrame) private(i,j)		//SHOULD BE SHARING BGMEAN TOO. BUT IS THROWING AN ERROR :'( WHY??? Dunnooo. Default access clause is sharing though.
			{
#pragma omp for schedule(dynamic)
				for( j= std::min(pt1.y, pt2.y)+1; j<std::max(pt1.y, pt2.y)-1; ++j) //just look for silhouettes in ROI
				{
					for( i=std::min(pt1.x, pt2.x)+1; i<std::max(pt1.x, pt2.x)-1; ++i)

					{
						// printf("\n i = %d, j = %d, pt1.x = %d, pt2.x = %d",i,j, pt1.x, pt2.x);
						//if( pixelDistance( &UPIXEL(thisFrame, i, j, 0), &UPIXEL(bgMean, i, j, 0), 3 ) < -threshold )
						if( fabs((mypixelDistance( &UPIXEL(bgcurFrame, i, j, 0), &UPIXEL(prevFrame, i, j, 0), 3 ))) < bgIniThreshold )
						{
							for(int k = 0; k < currbgMean->nChannels; ++k)
							{

								UPIXEL(currbgMean, i, j, k) = (unsigned char) (alpha * UPIXEL(bgcurFrame, i, j, k) + (1-alpha) * UPIXEL(currbgMean, i, j, k));
								//PIXEL(thisFrame, i, j, 3) = 0;
								//PIXEL(thisFrameClone, i, j, 3) = 0;
							}
						}
					}
				}
				/////////////////////end of for one frame/////////////////////////////////
			}
		}
		//		cvShowImage("bgcurFrame",bgcurFrame);
		//		cvShowImage("currbgMean",currbgMean);
		cvReleaseImage(&prevFrame);
		prevFrame = cvCloneImage(bgcurFrame);
		//		cvWaitKey(0);
	}

	calculatedInitialBG = cvCloneImage(currbgMean);
	IplImage* grayScaleInitialBg = cvCloneImage(calculatedInitialBG);
	int thresh = 100;
	int max_thresh = 255;
	char* calcBgWindow = "calculatedInitialBG Window";
	cvNamedWindow("calculatedInitialBG", CV_WINDOW_AUTOSIZE);
	cvShowImage(calcBgWindow, calculatedInitialBG);

	cvNamedWindow("calculatedInitialBG");
	cvShowImage("calculatedInitialBG", calculatedInitialBG);


	//cvWaitKey(0);

	cvDestroyWindow(mycaption1);
	cvDestroyWindow(mycaption2);

	cvReleaseImage(&bgcurFrame);
	cvReleaseImage(&prevFrame);
	cvReleaseImage(&currbgMean);
	//cvReleaseCapture(&inVideoCapture); I should not Release the capture because I am using it later ..

}
