#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "AvgBackground.h"
#include "cv_yuv_codebook.h"
#include "datastructure.h"
#include <cvblob.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctype.h>
#include <string.h>
#include "opencv2/opencv.hpp"
#include "BackgroundSubtract.h"
#include "Detector.h"
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <opencv2/highgui/highgui_c.h>
#include "Ctracker.h"
using namespace cv;
using namespace cvb;
using namespace std;
#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)  //接收超时
#define TIMEOUT_USEC 0
//VARIABLES for CODEBOOK METHOD:
codeBook *cB;   //This will be our linear model of the image, a vector 
				//of lengh = height*width
int maxMod[CHANNELS];	//Add these (possibly negative) number onto max 
						// level when code_element determining if new pixel is foreground
int minMod[CHANNELS]; 	//Subract these (possible negative) number from min 
						//level code_element when determining if pixel is foreground
unsigned cbBounds[CHANNELS]; //Code Book bounds for learning
bool ch[CHANNELS];		//This sets what channels should be adjusted for background bounds
int nChannels = CHANNELS;
int imageLen = 0;
uchar *pColor; //YUV pointer
/*parameters for erosion and dilation*/
Mat src, erosion_dst, dilation_dst;

int erosion_elem = 0;
int erosion_size = 0;  //originally 0
int dilation_elem = 0;
int dilation_size = 15; //originally 15
int const max_elem = 2;
int const max_kernel_size = 21;

int in = 0;
int out = 0;
const char* In;
const char* Out;
/** @function People Counter*/

/** @function Erosion */
void Erosion( IplImage* src, IplImage* output )
{
  int erosion_type;
  if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
  else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
  else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

  Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  /// Apply the erosion operation
  erode( Mat(src), Mat(output), element );
  //imshow( "Erosion Demo", erosion_dst );
}

/** @function Dilation */
void Dilation( IplImage* src, IplImage* output )
{
  int dilation_type;
  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

  Mat element = getStructuringElement( dilation_type,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
  /// Apply the dilation operation
  dilate( Mat(src), Mat(output), element );
  //imshow( "Dilation Demo", dilation_dst );
}
/*****************************************/

void FindBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs)
{
    blobs.clear();

    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground

    cv::Mat label_image;
    binary.convertTo(label_image, CV_32SC1);

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < label_image.rows; y++) {
        int *row = (int*)label_image.ptr(y);
        for(int x=0; x < label_image.cols; x++) {
            if(row[x] != 1) {
                continue;
            }

            cv::Rect rect;
            cv::floodFill(label_image, cv::Point(x,y), label_count, &rect, 0, 0, 4);

            std::vector <cv::Point2i> blob;

            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                int *row2 = (int*)label_image.ptr(i);
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if(row2[j] != label_count) {
                        continue;
                    }

                    blob.push_back(cv::Point2i(j,i));
                }
            }

            blobs.push_back(blob);

            label_count++;
        }
    }
}
void help() {
	printf("\nLearn background and find foreground using simple average and average difference learning method:\n"
		"\nUSAGE:\n  ch9_background startFrameCollection# endFrameCollection# [movie filename, else from camera]\n"
		"If from AVI, then optionally add HighAvg, LowAvg, HighCB_Y LowCB_Y HighCB_U LowCB_U HighCB_V LowCB_V\n\n"
		"***Keep the focus on the video windows, NOT the consol***\n\n"
		"INTERACTIVE PARAMETERS:\n"
		"\tESC,q,Q  - quit the program\n"
		"\th	- print this help\n"
		"\tp	- pause toggle\n"
		"\ts	- single step\n"
		"\tr	- run mode (single step off)\n"
		"=== AVG PARAMS ===\n"
		"\t-    - bump high threshold UP by 0.25\n"
		"\t=    - bump high threshold DOWN by 0.25\n"
		"\t[    - bump low threshold UP by 0.25\n"
		"\t]    - bump low threshold DOWN by 0.25\n"
		"=== CODEBOOK PARAMS ===\n"
		"\ty,u,v- only adjust channel 0(y) or 1(u) or 2(v) respectively\n"
		"\ta	- adjust all 3 channels at once\n"
		"\tb	- adjust both 2 and 3 at once\n"
		"\ti,o	- bump upper threshold up,down by 1\n"
		"\tk,l	- bump lower threshold up,down by 1\n"
		);
}

//
//USAGE:  background startFrameCollection# endFrameCollection# [movie filename, else from camera]
//If from AVI, then optionally add HighAvg, LowAvg, HighCB_Y LowCB_Y HighCB_U LowCB_U HighCB_V LowCB_V
//
int main(int argc, char** argv)
{
    CTracker tracker(0.2,0.5,60.0,10,10);  //for Kalman filter analysis
    Scalar Colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255),Scalar(255,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,127,255),Scalar(127,0,255),Scalar(127,0,127)};
    IplImage* rawImage = 0, *yuvImage = 0; //yuvImage is for codebook method
    IplImage *ImaskAVG = 0,*ImaskAVGCC = 0;
    IplImage *ImaskCodeBook = 0,*ImaskCodeBookCC = 0, *kfdisp =0;
    Mat display;
    cv::Mat output;
    CvCapture* capture = 0;
    CvTracks tracks;
	int startcapture = 1;
	int endcapture = 30;
    int c;
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 2, 8);
    vector<TrackPoint> prePoint, curPoint; //previous Point and Current Point
    int middle; //half of the height of the video
    //std::vector < std::vector<cv::Point2i > > blobs;
    //sprintf(in, "%d", In);
    vector<Point2d> centers; //for Kalman filter
    /** *********convert int to string*********************/
  /*  std::string In1,Out1;       //std string
    std::stringstream Inout,Outout;
    Inout << in;
    Outout << out;
    In1 = Inout.str();
    Out1 = Outout.str();
    In = In1.c_str();
    Out = Out1.c_str();*/
    /** **********************************************8  */
    maxMod[0] = 74;  //Set color thresholds to default values
    minMod[0] = 32;
    maxMod[1] = 72;
    minMod[1] = 23;
    maxMod[2] = 72;
    minMod[2] = 23;
	float scalehigh = HIGH_SCALE_NUM;
	float scalelow = LOW_SCALE_NUM;
	
	if(argc < 3) {
		printf("ERROR: Too few parameters\n");
		help();
	}else{
		if(argc == 3){
			printf("Capture from Camera\n");
			capture = cvCaptureFromCAM( 0 );
		}
		else {
			printf("Capture from file %s\n",argv[3]);
	//		capture = cvCaptureFromFile( argv[3] );
			capture = cvCreateFileCapture( argv[3] );
			if(!capture) { printf("Couldn't open %s\n",argv[3]); return -1;}
		}
		if(isdigit(argv[1][0])) { //Start from of background capture
			startcapture = atoi(argv[1]);
			printf("startcapture = %d\n",startcapture);
		}
		if(isdigit(argv[2][0])) { //End frame of background capture
			endcapture = atoi(argv[2]);
            printf("endcapture = %d\n", endcapture);
		}
		if(argc > 4){ //See if parameters are set from command line
			//FOR AVG MODEL
			if(argc >= 5){
				if(isdigit(argv[4][0])){
					scalehigh = (float)atoi(argv[4]);
				}
			}
			if(argc >= 6){
				if(isdigit(argv[5][0])){
					scalelow = (float)atoi(argv[5]);
				}
			}
			//FOR CODEBOOK MODEL, CHANNEL 0
			if(argc >= 7){
				if(isdigit(argv[6][0])){
					maxMod[0] = atoi(argv[6]);
				}
			}
			if(argc >= 8){
				if(isdigit(argv[7][0])){
					minMod[0] = atoi(argv[7]);
				}
			}
			//Channel 1
			if(argc >= 9){
				if(isdigit(argv[8][0])){
					maxMod[1] = atoi(argv[8]);
				}
			}
			if(argc >= 10){
				if(isdigit(argv[9][0])){
					minMod[1] = atoi(argv[9]);
				}
			}
			//Channel 2
			if(argc >= 11){
				if(isdigit(argv[10][0])){
					maxMod[2] = atoi(argv[10]);
				}
			}
			if(argc >= 12){
				if(isdigit(argv[11][0])){
					minMod[2] = atoi(argv[11]);
				}
			}
		}
	}

	//MAIN PROCESSING LOOP:
	bool pause = false;
	bool singlestep = false;


    /* Serial Port Part*/
    int readlen, fs_sel;
        fd_set	fs_read;
        struct timeval tv_timeout;

        FD_ZERO(&fs_read);

        tv_timeout.tv_sec = TIMEOUT_SEC(256, 9600);
        tv_timeout.tv_usec = TIMEOUT_USEC;
        int n;

    /* Open File Descriptor */
    int USB = open( "/dev/ttyUSB0", O_RDWR| O_NONBLOCK | O_NDELAY );

    /* Error Handling */
     if ( USB < 0 )
     {
      cout << "Error " << errno << " opening " << "/dev/ttyUSB0" << ": " << strerror (errno) << endl;
     }

    /* *** Configure Port *** */
    struct termios tty;
    memset (&tty, 0, sizeof tty);

    /* Error Handling */
    if ( tcgetattr ( USB, &tty ) != 0 )
    {
    cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
    }

    /* Set Baud Rate */
    cfsetospeed (&tty, B9600);
    cfsetispeed (&tty, B9600);

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;        // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cflag     &=  ~CRTSCTS;       // no flow control
    tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
    tty.c_oflag     =   0;                  // no remapping, no delays
    tty.c_cc[VMIN]      =   0;                  // read doesn't block
    tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout

    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
    tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
    tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    tty.c_oflag     &=  ~OPOST;              // make raw

    /* Flush Port, then applies attributes */
    tcflush( USB, TCIFLUSH );

    if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
    {
    cout << "Error " << errno << " from tcsetattr" << endl;
    }
    char messtowrite[3];
 /* ********************************************************** */


    if( capture )
    {
        cvNamedWindow( "Raw", 1 );
        //cvNamedWindow( "AVG_ConnectComp",1);
        cvNamedWindow( "ForegroundCodeBook",1);
		cvNamedWindow( "CodeBook_ConnectComp",1);
        namedWindow("Trajetory_Analysis");
        //cvNamedWindow( "ForegroundAVG",1);
        //cvNamedWindow( "blobresults",1);
        int i = -1;
        /*add trackbar for tuning erosion and dilation*/
        createTrackbar( "EElement:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Raw",
                          &erosion_elem, max_elem,
                          0 );

        createTrackbar( "EKernel size:\n 2n +1", "AVG_ConnectComp",
                          &erosion_size, max_kernel_size,
                          0 );

          /// Create Dilation Trackbar
        createTrackbar( "DElement:\n 0: Rect \n 1: Cross \n 2: Ellipse", "CodeBook_ConnectComp",
                          &dilation_elem, max_elem,
                          0 );

        createTrackbar( "DKernel size:\n 2n +1", "ForegroundAVG",
                          &dilation_size, max_kernel_size,
                          0 );
        /************************************************************/
        for(;;)
        {
    			if(!pause){
//        		if( !cvGrabFrame( capture ))
//                	break;
//            	rawImage = cvRetrieveFrame( capture );
				rawImage = cvQueryFrame( capture );
                middle = (rawImage->height)/2;
				++i;//count it
//				printf("%d\n",i);
                if(!rawImage)
					break;

				//REMOVE THIS FOR GENERAL OPERATION, JUST A CONVIENIENCE WHEN RUNNING WITH THE SMALL tree.avi file
				if(i == 56){
					pause = 1;
					printf("\n\nVideo paused for your convienience at frame 50 to work with demo\n"
					"You may adjust parameters, single step or continue running\n\n");
					help();
				}
			}
			if(singlestep){
				pause = true;
			}
			//First time:
			if(0 == i) {
				printf("\n . . . wait for it . . .\n"); //Just in case you wonder why the image is white at first
				//AVG METHOD ALLOCATION
				AllocateImages(rawImage);
				scaleHigh(scalehigh);
				scaleLow(scalelow);
                //ImaskAVG = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
                //ImaskAVGCC = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
                //cvSet(ImaskAVG,cvScalar(255));
				//CODEBOOK METHOD ALLOCATION:
				yuvImage = cvCloneImage(rawImage);
				ImaskCodeBook = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
				ImaskCodeBookCC = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
                //kfdisp = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 3 );
                //display = Mat(kfdisp);
				cvSet(ImaskCodeBook,cvScalar(255));
				imageLen = rawImage->width*rawImage->height;
				cB = new codeBook [imageLen];
				for(int f = 0; f<imageLen; f++)
				{
 					cB[f].numEntries = 0;
				}
				for(int nc=0; nc<nChannels;nc++)
				{
					cbBounds[nc] = 10; //Learning bounds factor
				}
				ch[0] = true; //Allow threshold setting simultaneously for all channels
				ch[1] = true;
				ch[2] = true;
			}
			//If we've got an rawImage and are good to go:                
        	if( rawImage )
        	{
                messtowrite[0] = in;
                messtowrite[1] = '\r';
                messtowrite[2] = '\0';

                kfdisp = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 3 );
                display = Mat(kfdisp);
                cvCvtColor( rawImage, yuvImage, CV_BGR2YCrCb );//YUV For codebook method
				//This is where we build our background model
				if( !pause && i >= startcapture && i < endcapture  ){
					//LEARNING THE AVERAGE AND AVG DIFF BACKGROUND
					accumulateBackground(rawImage);
					//LEARNING THE CODEBOOK BACKGROUND
					pColor = (uchar *)((yuvImage)->imageData);
					for(int c=0; c<imageLen; c++)
					{
						cvupdateCodeBook(pColor, cB[c], cbBounds, nChannels);
						pColor += 3;
					}
				}
				//When done, create the background model
				if(i == endcapture){
					createModelsfromStats();
				}
				//Find the foreground if any
				if(i >= endcapture) {
					//FIND FOREGROUND BY AVG METHOD:
                    //backgroundDiff(rawImage,ImaskAVG);
                    //cvCopy(ImaskAVG,ImaskAVGCC);
                    //cvconnectedComponents(ImaskAVGCC);
					//FIND FOREGROUND BY CODEBOOK METHOD
                    curPoint.clear();
					uchar maskPixelCodeBook;
					pColor = (uchar *)((yuvImage)->imageData); //3 channel yuv image
					uchar *pMask = (uchar *)((ImaskCodeBook)->imageData); //1 channel image
					for(int c=0; c<imageLen; c++)
					{
						 maskPixelCodeBook = cvbackgroundDiff(pColor, cB[c], nChannels, minMod, maxMod);
						*pMask++ = maskPixelCodeBook;
						pColor += 3;
					}
					//This part just to visualize bounding boxes and centers if desired
					cvCopy(ImaskCodeBook,ImaskCodeBookCC);	
					cvconnectedComponents(ImaskCodeBookCC);
                    Erosion(ImaskCodeBookCC,ImaskCodeBookCC);
                    Dilation(ImaskCodeBookCC,ImaskCodeBookCC);
                    //cvSaveImage("test.jpg",ImaskCodeBookCC);
                    //cv::Mat img = cv::imread("test.jpg", 0);
                    IplImage *labelImg = cvCreateImage(cvGetSize(ImaskCodeBookCC), IPL_DEPTH_LABEL, 1);

                    CvBlobs blobs;
                    unsigned int result = cvLabel(ImaskCodeBookCC, labelImg, blobs);

                    cvFilterByArea(blobs, 0, 100000);

                    cvUpdateTracks(blobs, tracks, 5., 10);
                    //cvUpdateTracks(blobs, tracks, 10., 5);
                    for (CvTracks::const_iterator it=tracks.begin(); it!=tracks.end(); ++it)
                    {
                        //if (mode&CV_TRACK_RENDER_ID)
                          if (!it->second->inactive)
                          {
                            curPoint.push_back(trackPoint(it->second->id,(int)it->second->centroid.x,(int)it->second->centroid.y));
                            centers.push_back(Point2d(it->second->centroid.x,it->second->centroid.y));
                          }
                    }


                    if(prePoint.size()==curPoint.size())
                    {
                        for(int kk=0; kk<prePoint.size(); kk++)
                        {
                          //line(display,prePoint[klm],curPoint[klm],);
                          if(curPoint[kk].id==prePoint[kk].id)
                          {
                              if(curPoint[kk].y<middle&&prePoint[kk].y>middle) //moving up
                              {
                                  out++;
                              }
                              if(curPoint[kk].y>middle&&prePoint[kk].y<middle) //moving down
                              {
                                  in++;
                                  int n_written = write( USB, messtowrite, sizeof(messtowrite)-1 );
                              }
                          }
                        }
                    }
                    if(centers.size()>0)
                    {
                        tracker.Update(centers);

                        cout << tracker.tracks.size()  << endl;
                        if(prePoint.size()==curPoint.size())
                        {
                            for(int j=0;j<centers.size();j++)
                            {
                             line(display,Point(prePoint[j].x,prePoint[j].y),Point(curPoint[j].x,curPoint[j].y),Scalar( 255, 0, 0 ),1,8,0);
                             cvCircle(kfdisp,Point(curPoint[j].x,curPoint[j].y),2,Scalar(255,255,0),1,8,0);
                            }
                        }
                        for(int i=0;i<tracker.tracks.size();i++)
                        {
                            if(tracker.tracks[i]->trace.size()>1)
                            {
                                for(int j=0;j<tracker.tracks[i]->trace.size()-1;j++)
                                {
                                    line(display,tracker.tracks[i]->trace[j],tracker.tracks[i]->trace[j+1],Colors[tracker.tracks[i]->track_id%9],1,CV_AA);
                                    cvCircle(kfdisp,centers[i],2,Scalar(255,0,255),1,8,0);
                                }
                            }
                        }
                    }
                    prePoint.clear();
                    for(int blobid=0; blobid<curPoint.size(); blobid++)
                    {
                        prePoint.push_back(curPoint[blobid]);
                    }

                    cvRenderBlobs(labelImg, blobs, ImaskCodeBookCC, rawImage, CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_BOUNDING_BOX);
                    cvRenderTracks(tracks, ImaskCodeBookCC, rawImage, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX|CV_TRACK_RENDER_TO_LOG);

                    /* Finding Blobs Part*/
                    /*FindBlobs(img, blobs);
                    output = cv::Mat::zeros(cvGetSize(ImaskCodeBookCC), CV_8UC3);

                    if(blobs.size()>0)
                    {
                        printf("dasdass");
                    }
                    // Randomy color the blobs
                    for(size_t i=0; i < blobs.size(); i++) {
                        unsigned char r = 255 * (rand()/(1.0 + RAND_MAX));
                        unsigned char g = 255 * (rand()/(1.0 + RAND_MAX));
                        unsigned char b = 255 * (rand()/(1.0 + RAND_MAX));

                        for(size_t j=0; j < blobs[i].size(); j++) {
                           if(blobs[i].size()>250)
                            {
                            int x = blobs[i][j].x;
                            int y = blobs[i][j].y;

                            output.at<cv::Vec3b>(y,x)[0] = b;
                            output.at<cv::Vec3b>(y,x)[1] = g;
                            output.at<cv::Vec3b>(y,x)[2] = r;
                           }
                        }
                    }*/
                    //cv::imshow("blobresults", output);
                    //cvErode(ImaskCodeBookCC,ImaskCodeBookCC,0,2);
                    //cvDilate(ImaskCodeBookCC,ImaskCodeBookCC,0,5);
				}
                //Display


                centers.clear();
                cvLine(rawImage, cvPoint(0,(rawImage->height)/2), cvPoint(rawImage->width,(rawImage->height)/2), CV_RGB(0,0,255), 1,8,0);
                stringstream ssin;
                stringstream ssout;
                ssin << in; ssout<<out;
                string innumber = ssin.str();
                string outnumber = ssout.str();
                //strcat(innumber.c_str(),"In");
                //frameNumberString.c_str()

                cvPutText(rawImage, innumber.c_str(), cvPoint(60,30), &font, CV_RGB(255,0,0));
                cvPutText(rawImage, "In:", cvPoint(20,30), &font, CV_RGB(255,0,0));
                cvPutText(rawImage, "Out:", cvPoint(180,30), &font, CV_RGB(255,0,0));
                cvPutText(rawImage, outnumber.c_str(), cvPoint(220,30), &font, CV_RGB(255,0,0));
                cvShowImage( "Raw", rawImage );
                //cvShowImage( "AVG_ConnectComp",ImaskAVGCC);
                //cvShowImage( "ForegroundAVG",ImaskAVG);
 				cvShowImage( "ForegroundCodeBook",ImaskCodeBook);
                cvShowImage( "CodeBook_ConnectComp",ImaskCodeBookCC);
                imshow( "Trajetory_Analysis",display);
                //display.release();
                //USER INPUT:
                c = cvWaitKey(30)&0xFF;
				//End processing on ESC, q or Q
				if(c == 27 || c == 'q' | c == 'Q')
					break;
				//Else check for user input
				switch(c)
				{
					case 'h':
						help();
						break;
					case 'p':
						pause ^= 1;
						break;
					case 's':
						singlestep = 1;
						pause = false;
						break;
					case 'r':
						pause = false;
						singlestep = false;
						break;
					//AVG BACKROUND PARAMS
					case '-':
						if(i > endcapture){
							scalehigh += 0.25;
							printf("AVG scalehigh=%f\n",scalehigh);
							scaleHigh(scalehigh);
						}
						break;
					case '=':
						if(i > endcapture){
							scalehigh -= 0.25;
							printf("AVG scalehigh=%f\n",scalehigh);
							scaleHigh(scalehigh);
						}
						break;
					case '[':
						if(i > endcapture){
							scalelow += 0.25;
							printf("AVG scalelow=%f\n",scalelow);
							scaleLow(scalelow);
						}
						break;
					case ']':
						if(i > endcapture){
							scalelow -= 0.25;
							printf("AVG scalelow=%f\n",scalelow);
							scaleLow(scalelow);
						}
						break;
				//CODEBOOK PARAMS
                case 'y':
                case '0':
                        ch[0] = 1;
                        ch[1] = 0;
                        ch[2] = 0;
                        printf("CodeBook YUV Channels active: ");
                        for(n=0; n<nChannels; n++)
                                printf("%d, ",ch[n]);
                        printf("\n");
                        break;
                case 'u':
                case '1':
                        ch[0] = 0;
                        ch[1] = 1;
                        ch[2] = 0;
                        printf("CodeBook YUV Channels active: ");
                        for(n=0; n<nChannels; n++)
                                printf("%d, ",ch[n]);
                        printf("\n");
                        break;
                case 'v':
                case '2':
                        ch[0] = 0;
                        ch[1] = 0;
                        ch[2] = 1;
                        printf("CodeBook YUV Channels active: ");
                        for(n=0; n<nChannels; n++)
                                printf("%d, ",ch[n]);
                        printf("\n");
                        break;
                case 'a': //All
                case '3':
                        ch[0] = 1;
                        ch[1] = 1;
                        ch[2] = 1;
                        printf("CodeBook YUV Channels active: ");
                        for(n=0; n<nChannels; n++)
                                printf("%d, ",ch[n]);
                        printf("\n");
                        break;
                case 'b':  //both u and v together
                        ch[0] = 0;
                        ch[1] = 1;
                        ch[2] = 1;
                        printf("CodeBook YUV Channels active: ");
                        for(n=0; n<nChannels; n++)
                                printf("%d, ",ch[n]);
                        printf("\n");
                        break;
				case 'i': //modify max classification bounds (max bound goes higher)
					for(n=0; n<nChannels; n++){
						if(ch[n])
							maxMod[n] += 1;
						printf("%.4d,",maxMod[n]);
					}
					printf(" CodeBook High Side\n");
					break;
				case 'o': //modify max classification bounds (max bound goes lower)
					for(n=0; n<nChannels; n++){
						if(ch[n])
							maxMod[n] -= 1;
						printf("%.4d,",maxMod[n]);
					}
					printf(" CodeBook High Side\n");
					break;
				case 'k': //modify min classification bounds (min bound goes lower)
					for(n=0; n<nChannels; n++){
						if(ch[n])
							minMod[n] += 1;
						printf("%.4d,",minMod[n]);
					}
					printf(" CodeBook Low Side\n");
					break;
				case 'l': //modify min classification bounds (min bound goes higher)
					for(n=0; n<nChannels; n++){
						if(ch[n])
							minMod[n] -= 1;
						printf("%.4d,",minMod[n]);
					}
					printf(" CodeBook Low Side\n");
					break;
				}
				
            }
		}		
        cvReleaseCapture( &capture );
        cvDestroyWindow( "Raw" );
		cvDestroyWindow( "ForegroundAVG" );
		cvDestroyWindow( "AVG_ConnectComp");
		cvDestroyWindow( "ForegroundCodeBook");
		cvDestroyWindow( "CodeBook_ConnectComp");
		DeallocateImages();
		if(yuvImage) cvReleaseImage(&yuvImage);
		if(ImaskAVG) cvReleaseImage(&ImaskAVG);
		if(ImaskAVGCC) cvReleaseImage(&ImaskAVGCC);
		if(ImaskCodeBook) cvReleaseImage(&ImaskCodeBook);
		if(ImaskCodeBookCC) cvReleaseImage(&ImaskCodeBookCC);

		delete [] cB;
    }
	else{ printf("\n\nDarn, Something wrong with the parameters\n\n"); help();
	}
    return 0;
}



