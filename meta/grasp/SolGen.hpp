#include <opencv2/opencv.hpp>

using namespace cv;
int const NOISE_RATE = 85;

class SolGen {
public:
	SolGen(){};
	//Scribbles + random noise
	static Mat generate_solution (Mat d, Mat w, Mat s_fg, Mat s_bg) {

		Size s = s_fg.size();
		Mat sol(s.height-1, s.width-1, CV_8UC1);
		randu(sol, Scalar(0), Scalar(100));

		Mat col = Mat::zeros(s.height-1, 1, CV_8UC1);
		cv::hconcat(sol, col, sol);
		Mat row = Mat::zeros(1, s.width, CV_8UC1);
		sol.push_back(row);

		threshold(sol, sol, NOISE_RATE, 1,THRESH_BINARY);
		
		sol -= s_bg;
		sol += s_fg;
		threshold(sol, sol, 0, 1,THRESH_BINARY);
		return sol;
	}
};