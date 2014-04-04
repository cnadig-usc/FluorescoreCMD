
#include <stdio.h>
#include "Experiment.h"
#include <string.h>
#include <iostream>
#define CAMERA_OUTPUT_WINDOW_NAME "camera-output"

void printUsage()
{
	printf("\nFluoreScore Usage:");
	printf("\n\nFSCommandLine x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14 x15 x16");
	printf("\n\nx1 : inputAviFile for View 1");
	printf("\n\nx2 : threshold value for View1, should be between 0 and 127");
	printf("\n\nx3 : inputAviFile for View 2");
	printf("\n\nx4 : threshold value for View2, should be between 0 and 127");
	printf("\n\nx5 : showing videos while processing( y or n). not showing videos can make the process faster");
	printf("\n\nx6 : Number of frames to process, put -1 to the whole video");
	printf("\n\nx7 : Mask file name for view 1 (jpg file)");
	printf("\n\nx8 : X-cordination of top-left for view 0 , min = 0, max = video width(640)");
	printf("\n\nx9 : Y-cordination of top-left for view 0 , min = 0, max = video height(480)");
	printf("\n\nx10 : X-cordination of down-right for view 0 , min = 0, max = video width (640)");
	printf("\n\nx11 : Y-cordination of down-right for view 0 , min = 0, max = video height (480)");
	printf("\n\nx12 : Mask file name for view 1 (jpg file)");
	printf("\n\nx13 : X-cordination of top-left for view 1 , min = 0, max = video width(640)");
	printf("\n\nx14 : Y-cordination of top-left for view 1 , min = 0, max = video height(480)");
	printf("\n\nx15 : X-cordination of down-right for view 1 , min = 0, max = video width(640)");
	printf("\n\nx16 : Y-cordination of down-right for view 1 , min = 0, max = video height(480)");
	printf("\n\nx1,x2,x3,x4,x5,x6 are mandatory parameters");
	printf("\n\nif x7-x16 have no input, videos will be processed without mask and ROI");
	printf("\n\nexample1:");
	printf("\n\nFSCommandLine.exe Case_View0.avi 15 Case_View1.avi 15 y -1 mask1.jpg 200 250 400 450 mask2.jpg 100 150 550 500");
	printf("\n\nexample2:");
	printf("\n\nFSCommandLine.exe Case_View0.avi 15 Case_View1.avi 15 y -1\n");
};


int main(int argc, char *argv[])
{

	//printf("\nargc= %d",argc);
	//getchar();
	if (argc<6)
	{
		printUsage();
		getchar();
		return 0;
	}

	CExperiment myExp;
	int nViews = 2;
	for (int i = 0;i<nViews;i++)
	{
		//printf("\n here i = %d ", i);
		myExp.addView(); // I just want to add one view for now ...
		myExp.viewList[i]->viewID = i;
	}

	//myExp.viewList[0]->inAVIFileName = "C:\\Users\\Administrator\\Desktop\\fs and related\\Archive\\Case_View1.avi";
	//input file names ...
	strcpy(myExp.viewList[0]->inAVIFileName ,argv[1]);

	std::string temp0;
	temp0 = argv[1];
	temp0.erase(temp0.end()-4,temp0.end());
	temp0 = temp0 + ".csv";
	strcpy(myExp.viewList[0]->outCSVFileName ,temp0.c_str());
	strcpy(myExp.viewList[1]->inAVIFileName ,argv[3]);

	std::string temp1;
	temp1 = argv[3];
	temp1.erase(temp1.end()-4,temp1.end());
	temp1 = temp1 + ".csv";
	strcpy(myExp.viewList[1]->outCSVFileName ,temp1.c_str());

	///getting threshold values
	int inputThresh = atoi(argv[2]);
	if (inputThresh > 127 || inputThresh<0)
	{
		printf("\ninvalid value for inputThresh");
		printUsage();
		getchar();
	}
	myExp.viewList[0]->viewThresh = inputThresh;
	inputThresh = atoi(argv[4]);
	if (inputThresh > 127 || inputThresh<0)
	{
		printf("\ninvalid value for inputThresh");
		printUsage();
		getchar();
	}
	myExp.viewList[1]->viewThresh = inputThresh;

	///////show window or not /////////////
	bool showWindows;
	if(strcasecmp(argv[5],"y") == 0)
		showWindows = true;
	else
		showWindows = false;
	//////////end of show window or not ///////

	/////////////number of frames to process ////////////
	int lastFramesToProcessIndex = 999999;
	if(atoi(argv[6]) == -1)
		lastFramesToProcessIndex = 9999999;
	else
		lastFramesToProcessIndex = atoi(argv[6]);
	/////////////////////////////////////////////////////
	if (argc == 7)
	{
		printf("\n **********it will process the video without any mask ond ROI************* ");
		myExp.viewList[0]->projROI.pt1.x = 0;
		myExp.viewList[0]->projROI.pt1.y = 0;
		myExp.viewList[0]->projROI.pt2.x = 640; //hardcoded.. should be set after opening the view
		myExp.viewList[0]->projROI.pt2.y = 480;//hardcoded.. should be set after opening the view
		myExp.viewList[1]->projROI.pt1.x = 0;
		myExp.viewList[1]->projROI.pt1.y = 0;
		myExp.viewList[1]->projROI.pt2.x = 640; //hardcoded.. should be set after opening the view
		myExp.viewList[1]->projROI.pt2.y = 480;  //hardcoded.. should be set after opening the view
		myExp.viewList[0]->myMask = cvCreateImage(cvSize(640,480),8,1);
		myExp.viewList[1]->myMask = cvCreateImage(cvSize(640,480),8,1);
		cvZero(myExp.viewList[0]->myMask);
		cvZero(myExp.viewList[1]->myMask);
	}
	else
	{
		if (argc ==17)
		{
			myExp.viewList[0]->myMask = cvLoadImage(argv[7],0);	 // black and white reading
			myExp.viewList[1]->myMask = cvLoadImage(argv[12],0);	 // black and white reading
			//////////first view ROI
			myExp.viewList[0]->projROI.pt1.x = atoi(argv[8]);
			myExp.viewList[0]->projROI.pt1.y = atoi(argv[9]);
			myExp.viewList[0]->projROI.pt2.x = atoi(argv[10]);
			myExp.viewList[0]->projROI.pt2.y = atoi(argv[11]);
			//////////first view ROI
			myExp.viewList[1]->projROI.pt1.x = atoi(argv[13]);
			myExp.viewList[1]->projROI.pt1.y = atoi(argv[14]);
			myExp.viewList[1]->projROI.pt2.x = atoi(argv[15]);
			myExp.viewList[1]->projROI.pt2.y = atoi(argv[16]);
		}
		else
		{
			printf("\nnot complete input..");
			printUsage();
			getchar();
			return 0;
		}
	}

	//printf("\n--------------------------------------------------------------------------");
	//printf("\n view 0 pt1 %d %d",myExp.viewList[0]->projROI.pt1.x ,myExp.viewList[0]->projROI.pt1.y );
	//printf("\n view 0 pt2 %d %d",myExp.viewList[0]->projROI.pt2.x ,myExp.viewList[0]->projROI.pt2.y );
	//printf("\n view 1 pt1 %d %d",myExp.viewList[1]->projROI.pt1.x ,myExp.viewList[1]->projROI.pt1.y );
	//printf("\n view 1 pt2 %d %d",myExp.viewList[1]->projROI.pt2.x ,myExp.viewList[1]->projROI.pt2.y );
	//getchar();

	int numOfFrameToModelBG = 100;
	//int lastFramesToProcessIndex = 999999;
	//
	//	if(atoi(argv[6]) == -1)
	//		lastFramesToProcessIndex = 9999999;
	//	else
	//		lastFramesToProcessIndex = atoi(argv[6]);

	//if (argc<7)
	//{
	//	numOfFrameToModelBG = 30;

	//}
	//else
	//	numOfFrameToModelBG = atoi(argv[7]);

	//int lastFramesToProcessIndex = 9999999;
	//int numOfFrameToModelBG = 30;

	//for (int i = 0;i<2;i++)
	//	printf("\nname of input file = %s",myExp.viewList[i]->inAVIFileName);


	myExp.openNewProject();
	//
	//	printf("\n%f", cvGetCaptureProperty(myExp.viewList[0]->inVideoCapture, CV_CAP_PROP_FOURCC));
	//	printf("\n%d", cvGetCaptureProperty(myExp.viewList[0]->inVideoCapture, CV_CAP_PROP_FOURCC));
	//	printf("\n%f",6);



	//myExp.viewList[i]->openView();
	printf("\ncalculating background");
	myExp.calculateInitialBG(numOfFrameToModelBG);


	//d	cvSetCaptureProperty(myExp.viewList[0]->inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(0.0)); // put it back
	//d	cvSetCaptureProperty(myExp.viewList[1]->inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(0.0)); // put it back



	printf("\n calculating Initial BG is done!\n");

	//cvSaveImage(strcat(pref0,".jpg"),myExp.viewList[0]->calculatedInitialBG);
	//cvSaveImage(strcat(pref1,".jpg"),myExp.viewList[1]->calculatedInitialBG);


	double t = (double)cvGetTickCount();



	//printf( "\n tiiimmeee = in my silh : %.1f\n", t/(cvGetTickFrequency()*1000.) );
	int fn;
	for(fn = 0 ;myExp.readNewFrame((unsigned long)fn)  && fn < lastFramesToProcessIndex ;fn++)
	{
		if((fn % 1000) == 0 )
			printf("fn = %d\n",fn);

		for (int i = 0;i<nViews;i++)
		{
			myExp.viewList[i]->silhDetect.pt1 = cvPoint(0,0);
			//printf("\nh = %d, w = %d",myExp.viewList[0]->currFrame->width,myExp.viewList[0]->currFrame->height);
			myExp.viewList[i]->silhDetect.pt2 = cvPoint(myExp.viewList[i]->currFrame->width,myExp.viewList[i]->currFrame->height);
		}

		//printf("\nbeforeprocess new frame");
		myExp.processNewFrame();
			//printf("\nbefore extract CurrentFrameInfo frame");


		myExp.extractCurrentFrameInfo();
//		cout<<"we're here: "<<fn<<endl;
		//printf("\nbefore write CurrentFrameInfo frame");
		//printf("\n view 0 = %s",myExp.viewList[0]->outCSVFileName);
		//printf("\n view 1 = %s",myExp.viewList[1]->outCSVFileName);

		myExp.writeCurrFrameInfo2CSV();
//		cout<<endl<<" and we're done here"<<endl;
		//printf("\nafter write CurrentFrameInfo frame");

		if (showWindows)
		{
			for (int i = 0;i<nViews;i++)
			{
				myExp.viewList[i]->show();
			}
			char k = cvWaitKey(1);
			if( k == 27 ) break;
		}
		//printf("\nafter showWindows");
		//t = (double)cvGetTickCount() - t;
		//printf( "\n total time is  %.1f seconds\n", t/(cvGetTickFrequency()*1000000.) );

	}

	for(int i = 0;i<nViews;i++)
		myExp.viewList[i]->closeView();

	t = (double)cvGetTickCount() - t;
	//printf( "\n total time is  %.1f seconds\n", t/(cvGetTickFrequency()*1000000.) );
	printf( "\ndone! press Enter");
	//getchar(); // if you want to run in batch this should not be here!
	exit(0);
	return 0;
}
