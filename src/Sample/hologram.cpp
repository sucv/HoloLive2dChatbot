#include "hologram.hpp"

cv::Mat HoloGram::holoTransform(cv::Mat original, double scale, double scaleR, int distance, QSize screenSize) {
    int height = int((scale * original.rows));
    int width = int((scale * original.cols));
    cv::Mat image;
    cv::Mat mask;
    cv::resize(original, image, cv::Size(fmax(height, width), fmax(height, width)), cv::INTER_CUBIC);
    cv::Mat up = image.clone();
    cv::Mat down = rotateBound(image.clone(), 180);
    cv::Mat right = rotateBound(image.clone(), 90);
    cv::Mat left = rotateBound(image.clone(), 270);
    cv::Mat hologram = cv::Mat::zeros(cv::Size(fmin(fmax(image.rows, image.cols) * scaleR + distance, screenSize.height()), fmin(fmax(image.rows, image.cols) * scaleR + distance, screenSize.height())), image.type());
    cv::Mat padding = cv::Mat::zeros(cv::Size(screenSize.width(), screenSize.height()), image.type());


    double center_x = (hologram.rows) / 2;
    double center_y = (hologram.cols) / 2;
    double vert_x = (up.rows) / 2;
    double vert_y = (up.cols) / 2;

    up.copyTo(hologram(cv::Rect(center_x - vert_x, 0, up.cols, up.rows)));
    mask = (down != 0);
    down.copyTo(hologram(cv::Rect(center_x - vert_x, hologram.cols - down.cols, down.cols, down.rows)),mask);
    double hori_x = (right.rows) / 2;
    double hori_y = (right.cols) / 2;
    mask = (right != 0);
    right.copyTo(hologram(cv::Rect(hologram.rows - right.rows , center_y - hori_y, right.cols, right.rows)), mask);
    mask = (left != 0);
    left.copyTo(hologram(cv::Rect(0, center_y - hori_y, left.cols, left.rows)), mask);

    int centerX = (padding.cols - hologram.cols) / 2;
    int centerY = (padding.rows - hologram.rows) / 2;

    // Copy the smaller Mat to the center of the larger Mat
    cv::Mat roi = padding(cv::Rect(centerX, centerY, hologram.cols, hologram.rows));
    hologram.copyTo(roi);

    return padding;
}

cv::Mat HoloGram::rotateBound(cv::Mat image, double angle) {
    // grab the dimensions of the image and then determine the center
    int h = image.rows;
    int w = image.cols;
    int cX = w / 2;
    int cY = h / 2;

    // grab the rotation matrix (applying the negative of the angle to rotate clockwise), then grab the sine and cosine
    // (i.e., the rotation components of the matrix)
    cv::Mat M = cv::getRotationMatrix2D(cv::Point2f(cX, cY), -angle, 1.0);
    double cos = std::abs(M.at<double>(0, 0));
    double sin = std::abs(M.at<double>(0, 1));

    // compute the new bounding dimensions of the image
    int nW = static_cast<int>((h * sin) + (w * cos));
    int nH = static_cast<int>((h * cos) + (w * sin));

    // adjust the rotation matrix to take into account translation
    M.at<double>(0, 2) += (nW / 2) - cX;
    M.at<double>(1, 2) += (nH / 2) - cY;

    // perform the actual rotation and return the image
    cv::Mat rotated;
    cv::warpAffine(image, rotated, M, cv::Size(nW, nH));
    return rotated;
}
