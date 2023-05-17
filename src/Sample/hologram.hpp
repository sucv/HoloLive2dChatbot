#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSize>

class HoloGram {
public:
    static cv::Mat holoTransform(cv::Mat original, double scale, double scaleR, int distance, QSize screenSize);
    static cv::Mat rotateBound(cv::Mat image, double angle);
};
