#include <opencv2/opencv.hpp>
#include "../ObjFunc.h"
#include "../SolGen.hpp"
#include <time.h>

using namespace cv;
using namespace std;

int const lambda = 0.1;
int const MAX_IT_G = 9;
int const MAX_IT_H = 100000;
int const BLOCK_CLASS1_SIZE = 1; //0
int const BLOCK_CLASS2_SIZE = 1; //1
int const BLOCK_CLASS3_SIZE = 1; //2

class CVWrap
{
    cv::Mat mat;
public:

    CVWrap (const cv::Mat& _mat) : mat(_mat) {}

    int operator() (int row, int col) const {
        return (int) mat.at<bool>(row,col);
    }
};

class ImageSeg {
public:
	ImageSeg(Mat _img, Mat _sc, Mat _d, Mat _w) : img(_img), dists(_d), w_coef(_w){
		//Random seed global Init
		cv::theRNG().state = time(NULL);

		vector<Mat> layers;
		split(_sc, layers);
		threshold(layers[2], s_fg, 0, 1,THRESH_BINARY);
		threshold(layers[1], s_bg, 0, 1,THRESH_BINARY);
		s_fg.convertTo(s_fg, CV_8UC1);
		s_bg.convertTo(s_bg, CV_8UC1);
		Size s = _img.size();
		obj = new ObjFunc(s.height, s.width, lambda, dists, w_coef);
	}

	vector<Mat> segmentation() {
		Mat sol = grasp();
		vector<Mat> seg (2);
		img.copyTo(seg[0], sol);
		threshold(sol, sol, 0, 1,THRESH_BINARY_INV);
		img.copyTo(seg[1], sol);
		return seg;
	}

private:
	ObjFunc *obj;
	Mat img;
	Mat s_fg, s_bg;

	Mat dists;
	Mat w_coef;

	Mat grasp () {
		//mode = RANDOM_NOISE, DILATING_SCRIBBLE, BIN_DISTANCE
		Mat best_sol = SolGen::generate_solution(dists, w_coef, s_fg, s_bg, DILATING_SCRIBBLE); 
		best_sol = hill_climing(best_sol);
		CVWrap cmat(best_sol);
		float best_fit = (*obj)(cmat);

		cout << "Fit0: " << best_fit << endl;

		#pragma omp parallel for shared(best_fit, best_sol) schedule(dynamic) //num_threads(4)
		for(int i = 1; i < MAX_IT_G; ++i) {
			Mat sol = SolGen::generate_solution(dists, w_coef, s_fg, s_bg, DILATING_SCRIBBLE);
			sol = hill_climing(sol);
			cmat = CVWrap(sol);
			float fit = (*obj)(cmat);
			cout << "Fit" << i << ": " << fit << endl;
			#pragma omp critical (update_best_sol)
        	{
				if(fit < best_fit) {
					best_sol = sol;
					best_fit = fit;
				}
			}
		}
		cout << "Best Fit: " << best_fit << endl;
		return best_sol;
	}

	Mat candidates(const Mat &sol) {
		Mat kernel = (Mat_<bool>(3,3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
		Mat candidates, aux;
		dilate(sol, candidates, kernel);
		erode(sol, aux, kernel);
		candidates = candidates-aux;

		//Removing scribbles from the candidates set
		candidates -= s_fg;
		candidates -= s_bg;

		//remove negative values and type conversion
		threshold(candidates, candidates, 0, 1,THRESH_BINARY);
		candidates.convertTo(candidates, CV_8UC1);

		return candidates;
	}

	double local_fit(const Mat &aux, int i, int j) {
		double local_fit = dists.at<float>(i,j) * (int)aux.at<char>(i,j); // d_i * x_i
    	Vec2f p = w_coef.at<cv::Vec2f>(i,j);
    	if(j < aux.size().width-1)local_fit += lambda * p[0] * abs((int)aux.at<char>(i,j) - (int)aux.at<char>(i,j+1)); // w_ij * |x_i - x_j| (right)
    	if(i < aux.size().height-1)local_fit += lambda * p[1] * abs((int)aux.at<char>(i,j) - (int)aux.at<char>(i+1,j)); // w_ij * |x_i - x_j| (down)
    	return local_fit;
	}

	template <typename T> T CLAMP(T value, T low, T high)
	{
		return (value < low) ? low : ((value > high) ? high : value);
	}

	inline void block_limits(const Mat &aux, int _i, int _j, int block_size, int &left, int &right, int &top, int &bottom) {
		block_size = block_size - 1;
		int side1 = block_size/2;
		int side2 = block_size - side1;

		int min = 0;
		int max_h = aux.size().width;
		int max_v = aux.size().height;

		left = CLAMP(_j - side1, min, max_h);
		right = CLAMP(_j + side2, min, max_h);

		top = CLAMP(_i - side1, min, max_v);
		bottom = CLAMP(_i + side2,  min, max_v);

	}

	inline double block_fit(const Mat &aux, int _i, int _j, int block_size) {
		int left, right, top, bottom;
		block_limits(aux, _i, _j, block_size, left, right, top, bottom);
		double fit = 0;
//cout << "i: " << _i << ", top: " << top << ", bottom: " << bottom << endl;
		if(top > 0 ) top = top - 1;
		if(left > 0 ) left = left - 1;
		for(int i = top; i <= bottom; ++i) {
			for(int j = left; j <= right; ++j) {
				fit += local_fit(aux, i, j);
			}
		}
		return fit; 
	}

	inline void block_change(Mat &aux, int _i, int _j, int block_size, uchar value) {
		int left, right, top, bottom;
		block_limits(aux, _i, _j, block_size, left, right, top, bottom);
		for(int i = top; i <= bottom; ++i) {
			for(int j = left; j <= right; ++j) {
				aux.at<char>(i,j) = value;
			}
		}
	}

	vector<Mat> best_neighbour(const Mat &state) {
		CVWrap mat(state);
		float fit = (*obj)(mat);
		Mat cand = candidates(state);
		vector<Mat> neighbours;
		int height = cand.size().height;
		int width = cand.size().width;

		Mat aux;
		state.copyTo(aux);

		for(int i = 0; i < height; ++i) {
			for(int j = 0; j < width; ++j) {
				if(cand.at<bool>(i,j)) {
					//Mat aux;
					//state.copyTo(aux);
				//cout << "Point " << i << " " << j << endl;
					double orig_fit = block_fit(aux, i, j, BLOCK_CLASS1_SIZE);
				//cout << "Blok: " << orig_fit << ", Point: "<<local_fit(aux, i,j) << endl;

					uchar orig_val = aux.at<uchar>(i,j);
					if(orig_val == 1) block_change(aux, i, j, BLOCK_CLASS1_SIZE, 0);
					if(orig_val == 0) block_change(aux, i, j, BLOCK_CLASS2_SIZE, 1);

					double new_fit = block_fit(aux, i, j, BLOCK_CLASS1_SIZE);
				//	cout << endl;

					if(new_fit < orig_fit) {
					//hill climb and plateaus
					//if(new_fit <= orig_fit) {
						neighbours.push_back(aux);
						return neighbours;
					}
					block_change(aux, i, j, BLOCK_CLASS1_SIZE, orig_val);
				}
			}
		}
		return neighbours;
	}

	Mat hill_climing (const Mat &sol) {

		Mat current_state = sol;
		bool end = false;
		int i = 0;
		while(!end && i <= MAX_IT_H){
			//first improvement strategy
			vector<Mat> neighbours = best_neighbour(current_state);
			if(neighbours.size() == 0) end = true;
			else current_state = neighbours[0];
			++i;
		}
		//imshow("Candidates", candidates(current_state)*255);
		//imshow("Currsol", current_state*255);
		return current_state;
	}
};