#include "ImageSeg.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
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

	/*Mat sol = SolGen::generate_solution(d, w, scribble);
	imshow("Sol", sol*255);
	*/

	ImageSeg seg(image, scribble, d, w);

	vector<Mat> segmentated = seg.segmentation();
	imshow("Foreground", segmentated[0]);
	imshow("Background", segmentated[1]);

	waitKey(0);
}
