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
    R operator() (const Mat& mat) {
        R n = 0;
        for (int i = 0; i < height-1; ++i) {
            for (int j = 0; j < width-1; ++j) {
                n += d.at<float>(i,j) * mat(i,j); // d_i * x_i
                cv::Vec2f p = w.at<cv::Vec2f>(i,j);
                n += lambda * p[0] * abs(mat(i,j) - mat(i,j+1)); // w_ij * |x_i - x_j| (right)
                n += lambda * p[1] * abs(mat(i,j) - mat(i+1,j)); // w_ij * |x_i - x_j| (down)
            }
        }
        return n;
    }
};
