#include "ImageSeg.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "../ParseDatFile.hpp"

using namespace std;

static void help() {
	std::cout << "Usage:" << std::endl << "program <image_name>" << std::endl;
}

int main(int argc, const char** argv ) {
	if(argc < 2) {
		help();
		exit(0);
	}
	string name = argv[1];
	Mat image, scribble, w, d;
	image = imread(name+".png");
	//imshow("Original image", image);

	scribble = imread(name+"-scribble.png");
	//imshow("Original scribble", scribble);

	ParseDatFile parse(name+".dat");

	parse.parseDandZ(d, w);

	//imshow("Original D", d);
	//imshow("Original W", w);
	/*
	Mat s_fg, s_bg;
	vector<Mat> layers;
	split(scribble, layers);
	threshold(layers[2], s_fg, 0, 1,THRESH_BINARY);
	threshold(layers[1], s_bg, 0, 1,THRESH_BINARY);
	s_fg.convertTo(s_fg, CV_8UC1);
	s_bg.convertTo(s_bg, CV_8UC1); 
	Mat sol = SolGen::generate_solution(d, w, s_fg, s_bg, DILATING_SCRIBBLE); //enum {RANDOM_NOISE, DILATING_SCRIBBLE, BIN_DISTANCE};
	imshow("Sol", sol*255);
	*/
	
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
    time_t s  = spec.tv_sec;
    long ms = round(spec.tv_nsec / 1.0e6);

	ImageSeg seg(image, scribble, d, w);
	vector<Mat> segmentated = seg.segmentation();

	clock_gettime(CLOCK_REALTIME, &spec);
    s  = spec.tv_sec - s;
    ms = round(spec.tv_nsec / 1.0e6) - ms;

	cout << "segmentated in " << ms+s*1000 << " ms" << endl;


	namedWindow("Foreground", CV_WINDOW_NORMAL|CV_WINDOW_KEEPRATIO|CV_GUI_EXPANDED);
	imshow("Foreground", segmentated[0]);
	resizeWindow("Foreground", 512, 512);

	namedWindow("Background", CV_WINDOW_NORMAL|CV_WINDOW_KEEPRATIO|CV_GUI_EXPANDED);
	imshow("Background", segmentated[1]);
	resizeWindow("Background", 512, 512);

	waitKey(0);
}
