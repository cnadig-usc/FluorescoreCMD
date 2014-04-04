#pragma once
#include "view.h"
//#include <QMessageBox>
#include <omp.h>
class CExperiment
{
public:
	CExperiment(void);
	~CExperiment(void);

        std::vector<CView*> viewList;
//      CView view0;
//      CView view1;
//   	public slots:
        bool openNewProject(void);
        void processNewFrame(void);
        std::string expSettings;
        void addView(void);
        void close(void);
        bool readNewFrame(unsigned long frameIndex);
        void show(void);
        bool stopIt;
        void extractCurrentFrameInfo(void);
        void writeCurrFrameInfo2CSV(void);
		bool calculateInitialBG(int);
};


