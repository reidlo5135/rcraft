#include "rcraft/map_converter.hpp"

using namespace rcraft::map;

MapConverter::MapConverter()
    : map_(cv::Mat())
{
}

MapConverter::~MapConverter() = default;

cv::Mat MapConverter::color2gray(const cv::Mat &map)
{
    CV_Assert(!map.empty());
    std::cout << "[INFO][MapConverter] Color2Gray map size : " << map.size << '\n';

    if (map.channels() == 1)
    {
        return map.clone();
    }

    cv::Mat bgr;
    if (map.channels() == 4)
    {
        cv::cvtColor(map, bgr, cv::COLOR_BGRA2BGR);
    }
    else
    {
        bgr = map;
    }

    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    cv::Mat gray;
    cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Mat> ch;
    cv::split(hsv, ch); // ch[1] = S

    const int S_MIN = 90, V_MIN = 60;
    if (cv::countNonZero(ch[1] >= S_MIN) == 0)
    {
        return gray;
    }

    auto in_range_hsv = [&](const cv::Scalar &lo, const cv::Scalar &hi) -> cv::Mat
    {
        cv::Mat m;
        cv::inRange(hsv, lo, hi, m);
        return m;
    };

    cv::Mat m_green  = in_range_hsv({40,  S_MIN, V_MIN}, {85, 255, 255});  // 초록
    cv::Mat m_yellow = in_range_hsv({20,  S_MIN, V_MIN}, {34, 255, 255});  // 노랑 (상한 34)
    cv::Mat m_magenta= in_range_hsv({140, S_MIN, V_MIN}, {165,255, 255});  // 마젠타
    cv::Mat m_red1   = in_range_hsv({0,   S_MIN, V_MIN}, {10, 255, 255});
    cv::Mat m_red2   = in_range_hsv({170, S_MIN, V_MIN}, {179,255, 255});
    cv::Mat m_red; cv::bitwise_or(m_red1, m_red2, m_red);

    cv::Mat k = cv::getStructuringElement(cv::MORPH_ELLIPSE, {3,3});
    cv::morphologyEx(m_green,  m_green,  cv::MORPH_OPEN, k);
    cv::morphologyEx(m_yellow, m_yellow, cv::MORPH_OPEN, k);
    cv::morphologyEx(m_magenta,m_magenta,cv::MORPH_OPEN, k);
    cv::morphologyEx(m_red,    m_red,    cv::MORPH_OPEN, k);

    cv::Mat out = cv::Mat::zeros(hsv.size(), CV_8UC1);

    cv::Mat m_my;
    cv::bitwise_or(m_magenta, m_yellow, m_my);
    out.setTo(128, m_my);

    out.setTo(255, m_green);

    cv::Mat handled;
    cv::bitwise_or(m_green, m_my, handled);
    cv::bitwise_or(handled, m_red, handled);
    cv::Mat inv; cv::bitwise_not(handled, inv);
    gray.copyTo(out, inv);

    return out;
}