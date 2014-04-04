#ifndef VIEW_H
#define VIEW_H


#include <cv.h>
#include "SilhDetector.h"
#include "statSilhDetector.h"
#include <highgui.h>
#include <string>

using namespace std;

//#include "setthreshdlg.h"

struct twoPointsROI{

        CvPoint pt1;
        CvPoint pt2;

//        twoPointsROI() : pt1(cvPoint(0,0)),
//                         pt2(cvPoint(800,600)
//        {
//        }

};




struct FrameIntensityInfo
{
    std::vector<std::vector<int> > rChannel; // this keeps red channel info for all components detected in current frame ..
    std::vector<std::vector<int> > bChannel;// this keeps blue channel info for all components detected in current frame ..
    std::vector<std::vector<int> > gChannel;// this keeps green channel info for all components detected in current frame ..
    std::vector<std::vector<double> > pos_size; // keeps x,y, and total num of pixels for each detected component
};

class CView
{
public:
        IplImage* currFrame;
        //IplImage* changeMask;
        FrameIntensityInfo currFrameInfo;

        int viewID;

     //   static IplImage* myMask; // this is for processing mask //IMPORTANT CHANGE... SINCE I DON'T NEED TO DEFINE A MASK USING MOUSE THIS VARIABLE DOESN'T NEED TO BE STATIC AND ACTUALLY FOR MORE THAN 1 VIEW IT SHOULDN'T BE AT ALL, SINCE i WILL HAVE TWO INSTANCES OF THIS CLASS BUT BOTH WILL HAVE THE SAME MASK, I DON'T WANT THAT
		IplImage* myMask;
        IplImage* viewMask;
		IplImage* viewProcessedArea;

        int frameWidth;
        int frameHeight;
        int numFrames;
        bool isMaskSet;
        bool toStop, askAgain;
        bool doDenoising;
        bool saveSeparateSilh;
        bool showInputVideo;
        bool showChangeMask;
        bool showDenoisedChangeMask;
        bool showProcessedArea;
        bool showMask;
		
		bool showBGModelingProcess;
        bool isOpen;

        bool saveCSVoutput; // its gonna save green channel as default
        bool saveBlue;
        bool saveRed;
        bool saveGreen;
        int frameCount;

//        static twoPointsROI projROI; // I changed this from static since I have two views now, similar to My mask variable
        twoPointsROI projROI; 
        //static void roiMouseHandler(int events, int x, int y, int flags, void* param);
        //static void maskMouseHandler(int events, int x, int y, int flags, void* param);
		static void onTrackbarSlide(int thresh);
        CView(void);
        ~CView(void);
//	CString ProjInputVideo;
//	CString ProjOutputFile;


        //std::string inAVIFileName;
        char inAVIFileName[10000];

//        std::string outCSVFileName;
        char outCSVFileName[10000];

        std::string viewSettings;
        cSilhDetector silhDetect;
		statSilhDetector statSilhDetect;


        double viewThresh;
//        std::fstream projOFile;
//        std::fstream projPosOfile; //output file for position of the blobs ...(for activity measurement ..)
//        CvCapture* projInputVideoCapture;


        CvCapture* inVideoCapture;
        FILE *fp_outputCSV;
         //FILE *fp_toTestView;


//        void ProcessTheVideo(void);
        void processNewFrame(void);
        bool readNewFrame(unsigned long frameIndex);
//bool View::readNewFrame(unsigned long frameIndex);

        void closeView(void);
     //   void setViewROI(void);
        // To set Mask for taking out glares
  //      void setMask(void);
       // void ProcessTheVideo(void);

        bool openView(void);
        bool initProcessing(void);
        void writeCurrFrameInfo2CSV(void);
        void extractCurrFrameInfo(void);
        int numBins;
        void show(void);
        void setThreshold(void);

        void printViewSettings(void);
        bool readFrameResult;
        void writeHeaderinCSV(void);
        bool justForDefiningThreshold; // to check if we are in the state of defining thresholds
		void processNewFrame2(void);
		bool useStatSilh;
		IplImage *calculatedInitialBG;
		void calculateInitialBG(int);
};




#endif // VIEW_H
