#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>

double getTheta(double x, double y) {
    double rtn = 0;
    if(y < 0) {
        rtn = -std::atan2(y, x);
    } else {
        rtn = M_PI + (M_PI - std::atan2(y, x));
    }
    return rtn;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Usage: ./program <image_path>\n";
        return -1;
    }

    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
    int inputWidth = image.cols;
    int inputHeight = image.rows;
    double sqr = inputWidth / 4.0;
    int outputWidth = sqr * 3;
    int outputHeight = sqr * 2;

    cv::Mat outputImage(outputHeight, outputWidth, CV_8UC3, cv::Scalar(0,0,0));

    for(int loopY = 0; loopY < outputHeight; loopY++) {
        for(int loopX = 0; loopX < outputWidth; loopX++) {
            double tx = 0, ty = 0, x = 0, y = 0, z = 0;

            if(loopY < sqr) { // top half
                if(loopX < sqr) { // top left box [Y+]
                    tx = loopX;
                    ty = loopY;
                    x = tx - 0.5 * sqr;
                    y = 0.5 * sqr;
                    z = ty - 0.5 * sqr;
                } else if(loopX < 2 * sqr) { // top middle [X+]
                    tx = loopX - sqr;
                    ty = loopY;
                    x = 0.5 * sqr;
                    y = (tx - 0.5 * sqr) * -1;
                    z = ty - 0.5 * sqr;
                } else { // top right [Y-]
                    tx = loopX - sqr * 2;
                    ty = loopY;
                    x = (tx - 0.5 * sqr) * -1;
                    y = -0.5 * sqr;
                    z = ty - 0.5 * sqr;
                }
            } else { // bottom half
                if(loopX < sqr) { // bottom left box [X-]
                    tx = loopX;
                    ty = loopY - sqr;
                    x = -0.5 * sqr;
                    y = tx - 0.5 * sqr;
                    z = ty - 0.5 * sqr;
                } else if(loopX < 2 * sqr) { // bottom middle [Z-]
                    tx = loopX - sqr;
                    ty = loopY - sqr;
                    x = (ty - 0.5 * sqr) * -1;
                    y = (tx - 0.5 * sqr) * -1;
                    z = 0.5 * sqr;
                } else { // bottom right [Z+]
                    tx = loopX - sqr * 2;
                    ty = loopY - sqr;
                    x = ty - 0.5 * sqr;
                    y = (tx - 0.5 * sqr) * -1;
                    z = -0.5 * sqr;
                }
            }

            // now find out the polar coordinates
            double rho = std::sqrt(x*x + y*y + z*z);
            double normTheta = getTheta(x, y) / (2 * M_PI); // /(2*M_PI) normalise theta
            double normPhi = (M_PI - std::acos(z / rho)) / M_PI; // /M_PI normalise phi

            // use this for coordinates
            int iX = normTheta * inputWidth;
            int iY = normPhi * inputHeight;

            // catch possible overflows
            if(iX >= inputWidth) {
                iX = iX - inputWidth;
            }
            if(iY >= inputHeight) {
                iY = iY - inputHeight;
            }

            outputImage.at<cv::Vec3b>(loopY, loopX) = image.at<cv::Vec3b>(iY, iX);
        }
    }

    std::string outputFilename = std::string(argv[1]);
    size_t lastindex = outputFilename.find_last_of(".");
    outputFilename = outputFilename.substr(0, lastindex) + "_To_CUBE.png";
    cv::imwrite(outputFilename, outputImage);

    return 0;
}