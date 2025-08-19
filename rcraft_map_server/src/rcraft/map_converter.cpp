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

    // 0) 이미 그레이면 그대로
    if (map.channels() == 1)
    {
        return map.clone();
    }

    // 1) BGR 정리
    cv::Mat bgr;
    if (map.channels() == 4)
    {
        cv::cvtColor(map, bgr, cv::COLOR_BGRA2BGR);
    }
    else
    {
        bgr = map;
    }

    // 2) HSV + 그레이 준비
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    cv::Mat gray;
    cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);

    // 2-1) 색이 사실상 없는(그레이스케일) 이미지면 바로 반환
    std::vector<cv::Mat> ch;
    cv::split(hsv, ch); // ch[1] = S

    const int S_MIN = 90, V_MIN = 60;
    if (cv::countNonZero(ch[1] >= S_MIN) == 0)
    {
        return gray; // 전체가 저채도 → 원본 그레이 유지
    }

    auto in_range_hsv = [&](const cv::Scalar &lo, const cv::Scalar &hi) -> cv::Mat
    {
        cv::Mat m;
        cv::inRange(hsv, lo, hi, m);
        return m;
    };

    // 3) 우리 색만 마스크 (경계 안겹치게)
    cv::Mat m_green  = in_range_hsv({40,  S_MIN, V_MIN}, {85, 255, 255});  // 초록
    cv::Mat m_yellow = in_range_hsv({20,  S_MIN, V_MIN}, {34, 255, 255});  // 노랑 (상한 34)
    cv::Mat m_magenta= in_range_hsv({140, S_MIN, V_MIN}, {165,255, 255});  // 마젠타
    cv::Mat m_red1   = in_range_hsv({0,   S_MIN, V_MIN}, {10, 255, 255});
    cv::Mat m_red2   = in_range_hsv({170, S_MIN, V_MIN}, {179,255, 255});
    cv::Mat m_red; cv::bitwise_or(m_red1, m_red2, m_red);

    // (선택) 잡음 정리
    cv::Mat k = cv::getStructuringElement(cv::MORPH_ELLIPSE, {3,3});
    cv::morphologyEx(m_green,  m_green,  cv::MORPH_OPEN, k);
    cv::morphologyEx(m_yellow, m_yellow, cv::MORPH_OPEN, k);
    cv::morphologyEx(m_magenta,m_magenta,cv::MORPH_OPEN, k);
    cv::morphologyEx(m_red,    m_red,    cv::MORPH_OPEN, k);

    // 4) 매핑: 기본 0(검정)에서 시작
    cv::Mat out = cv::Mat::zeros(hsv.size(), CV_8UC1);

    // 회색(128): 노랑 ∪ 마젠타
    cv::Mat m_my;
    cv::bitwise_or(m_magenta, m_yellow, m_my);
    out.setTo(128, m_my);

    // 흰색(255): 초록
    out.setTo(255, m_green);

    // 5) 나머지 픽셀은 **원본 그레이 유지** (빨강은 0 검정 유지)
    cv::Mat handled; // 처리된(또는 빨강) 픽셀
    cv::bitwise_or(m_green, m_my, handled);
    cv::bitwise_or(handled, m_red, handled); // 빨강을 handled에 포함해야 회색으로 덮지 않음
    cv::Mat inv; cv::bitwise_not(handled, inv);
    gray.copyTo(out, inv);

    return out;
}