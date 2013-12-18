#pragma once

#include <opencv2/opencv.hpp>

class ObjFunc
{
public:

    typedef double R;
    
private:
    
    const int width;  // Image width
    const int height; // Image height
    const R lambda;   // Regularisation parameter
    cv::Mat d;        // Delta_i
    cv::Mat w;        // w_i

public:
    
    /// Construct an objective function object.
    ObjFunc (int _width, int _height, R _lambda, const cv::Mat& _d, const cv::Mat& _w)
        : width(_width), height(_height), lambda(_lambda), d(_d), w(_w) {}

    /// Evaluate the objective function for the given image.
    template <class Mat>
    R operator() (Mat mat) const {
        return eval(width, height, lambda, d, w, mat);
    }
    
    template <class Mat>
    static R eval (int width, int height, R lambda,  const cv::Mat& d, const cv::Mat& w, const Mat& mat) {
        R n = 0;
        /*// Inner matrix
        for (int i = 0; i < height-1; ++i) {
            for (int j = 0; j < width-1; ++j) {
                n += d.at<float>(i,j) * mat(i,j); // d_i * x_i
                cv::Vec2f p = w.at<cv::Vec2f>(i,j);
                n += lambda * p[0] * abs(mat(i,j) - mat(i,j+1)); // w_ij * |x_i - x_j| (right)
                n += lambda * p[1] * abs(mat(i,j) - mat(i+1,j)); // w_ij * |x_i - x_j| (down)
            }
        }
        // Last row
        int last_row = height-1;
        for (int j = 0; j < width-1; ++j) {
            n += d.at<float>(last_row,j) * mat(last_row,j); // d_i * x_i
            n += lambda * w.at<cv::Vec2f>(last_row,j)[0] * abs(mat(last_row,j) - mat(last_row,j+1)); // w_ij * |x_i - x_j| (right)
        }
        // Last col
        int last_col = width-1;
        for (int i = 0; i < height-1; ++i) {
            n += d.at<float>(i,last_col) * mat(i,last_col); // d_i * x_i
            n += lambda * w.at<cv::Vec2f>(i,last_col)[1] * abs(mat(i,last_col) - mat(i+1,last_col)); // w_ij * |x_i - x_j| (down)
        }*/
        // Inner matrix
        for (int i = 0; i < height-1; ++i) {
            const float* d_ptr = d.ptr<float>(i);
            const float* w_ptr = w.ptr<float>(i);
            for (int j = 0; j < width-1; ++j) {
                n += (*d_ptr++) * mat(i,j); // d_i * x_i
                n += lambda * *w_ptr++ * abs(mat(i,j) - mat(i,j+1)); // w_ij * |x_i - x_j| (right)
                n += lambda * *w_ptr++ * abs(mat(i,j) - mat(i+1,j)); // w_ij * |x_i - x_j| (down)
            }
        }
        // Last row
        int last_row = height-1;
        const float* d_ptr = d.ptr<float>(last_row);
        const float* w_ptr = w.ptr<float>(last_row);
        for (int j = 0; j < width-1; ++j) {
            n += *d_ptr++ * mat(last_row,j); // d_i * x_i
            n += lambda * *w_ptr * abs(mat(last_row,j) - mat(last_row,j+1)); // w_ij * |x_i - x_j| (right)
            w_ptr += 2;
        }
        // Last col
        int last_col = width-1;
        for (int i = 0; i < height-1; ++i) {
            n += d.at<float>(i,last_col) * mat(i,last_col); // d_i * x_i
            n += lambda * w.at<cv::Vec2f>(i,last_col)[1] * abs(mat(i,last_col) - mat(i+1,last_col)); // w_ij * |x_i - x_j| (down)
        }
        return n;
    }
};
