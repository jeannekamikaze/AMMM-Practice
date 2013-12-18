#include <opencv2/opencv.hpp>

using namespace cv;

//Only 15%  noise 
int const DENOISE_RATE = 85;
enum {RANDOM_NOISE, DILATING_SCRIBBLE, BIN_DISTANCE};

class SolGen {
public:
	SolGen(){};
	static Mat generate_solution (const Mat &d, const Mat &w, const Mat &s_fg, const Mat &s_bg, int mode = RANDOM_NOISE) {
		Mat sol;
		switch(mode) {
			case RANDOM_NOISE:
				sol = random_solution (d, w, s_fg, s_bg);
			break;
			case DILATING_SCRIBBLE:
				sol = dilating_scrible (d, w, s_fg, s_bg);
			break;
			case BIN_DISTANCE:
				sol = binarize_distance (d, w, s_fg, s_bg);
			break;
			default:
			//Error
			exit(0);
		}
		return sol;
	}

private:
	//random noise
	static Mat random_solution (const Mat &d, const Mat &w, const Mat &s_fg, const Mat &s_bg) {
		Size s = s_fg.size();
		Mat sol(s.height, s.width, CV_8UC1);
		RNG(time(NULL));
		randu(sol, Scalar(0), Scalar(100));
		
		threshold(sol, sol, DENOISE_RATE, 1,THRESH_BINARY);
		
		include_scribbles(sol, s_fg, s_bg);
		return sol;
	}

	//dilating fg
	static Mat dilating_scrible (const Mat &d, const Mat &w, const Mat &s_fg, const Mat &s_bg) {
		Size s = s_fg.size();
		Mat sol(s.height, s.width, CV_8UC1);

		Mat kernel = (Mat_<bool>(3,3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
		Mat candidates, aux;
		srand (time(NULL));
		int steps = RNG(time(NULL)).uniform(0,s.width/2);
		dilate(s_fg, sol, kernel, Point(-1,-1), steps);
		
		include_scribbles(sol, s_fg, s_bg);
		return sol;
	}

	//bin distances
	static Mat binarize_distance (const Mat &d, const Mat &w, const Mat &s_fg, const Mat &s_bg) {
		Size s = s_fg.size();
		Mat sol(s.height, s.width, CV_8UC1);
		
		double min, max; 
        Point minLoc, maxLoc;
        minMaxLoc( d, &min, &max, &minLoc, &maxLoc );

        double thresh = RNG(time(NULL)).uniform(min,max);
		threshold(s_fg, sol, thresh , 1,THRESH_BINARY_INV);
		
		include_scribbles(sol, s_fg, s_bg);
		return sol;
	}

	//includeing scribbles
	static inline void include_scribbles(Mat &sol, const Mat &s_fg, const Mat &s_bg) {
		sol -= s_bg;
		sol += s_fg;
		threshold(sol, sol, 0, 1,THRESH_BINARY);
	}

};