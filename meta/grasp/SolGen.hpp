#include <opencv2/opencv.hpp>

using namespace cv;

//Only 15%  noise 
int const DENOISE_RATE = 85;

class SolGen {
public:
	SolGen(){};
	static Mat generate_solution (Mat d, Mat w, Mat s_fg, Mat s_bg) {
		return random_solution (d, w, s_fg, s_bg);
	}

private:
	//Scribbles + random noise
	static Mat random_solution (Mat d, Mat w, Mat s_fg, Mat s_bg) {
		Size s = s_fg.size();
		Mat sol(s.height, s.width, CV_8UC1);
		randu(sol, Scalar(0), Scalar(100));
		
		threshold(sol, sol, DENOISE_RATE, 1,THRESH_BINARY);
		
		sol -= s_bg;
		sol += s_fg;
		threshold(sol, sol, 0, 1,THRESH_BINARY);
		return sol;
	}

	//dilating fg

};