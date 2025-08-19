#ifndef RCRAFT_GLOBAL_PLANNER_HPP
#define RCRAFT_GLOBAL_PLANNER_HPP

#include <queue>
#include <cmath>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <chrono>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#define A_START_MARGIN 80

namespace rcraft::planner
{
    static constexpr int kCostStraight = 1000;   ///< 수평/수직 이동 비용
    static constexpr int kCostDiagonal = 1414;   ///< 대각선 이동 비용(≈ √2 * 1000)

    struct QN
    {
        int f;
        int i;
    };

    struct Cmp
    {
        bool operator()(const QN &a, const QN &b) const
        {
            return a.f > b.f;
        }
    };

    static inline int
    manhattan_heuristic(const int &x, const int &y, const int &gx, const int &gy)
    {
        return std::abs(x - gx) + std::abs(y - gy);
    }

    /**
     * @brief Octile 휴리스틱(8방향용, 정수 스케일).
     *
     * h = D * (dx + dy) + (D2 - 2*D) * min(dx, dy)
     *
     * @param x   현재 x
     * @param y   현재 y
     * @param gx  목표 x
     * @param gy  목표 y
     * @return int  정수 스케일 휴리스틱 비용
     */
    static inline int
    octile_heuristic(const int &x, const int &y, const int &gx, const int &gy)
    {
        const int &dx = std::abs(x - gx);
        const int &dy = std::abs(y - gy);
        return kCostStraight * (dx + dy) + (kCostDiagonal - 2 * kCostStraight) * std::min(dx, dy);
    }

    /**
     * @brief 대각선 이동 시 corner-cutting(모서리 끼어돌기) 방지 체크.
     *
     * (cx, cy) → (nx, ny)가 대각선 이동일 경우, 수평/수직 인접 칸 둘 다 통로여야 함.
     * 즉, (cx, ny), (nx, cy)가 모두 자유 셀이어야 대각선 이동 허용.
     *
     * @tparam IsFree  bool(int,int)를 호출 가능한 functor/lambda
     * @param cx  현재 x
     * @param cy  현재 y
     * @param nx  다음 x
     * @param ny  다음 y
     * @param is_free  셀 통로 여부 함수 (x,y) -> true if free
     * @return true   이동 허용
     * @return false  이동 불가(코너 충돌 가능)
     */
    template <class IsFree>
    static inline bool
    allow_diagonal_without_cutting(int cx, int cy, int nx, int ny, IsFree is_free)
    {
        // 대각선이 아니면 별도 체크 불필요
        if ((cx == nx) || (cy == ny))
        {
            return true;
        }
        // 대각선일 경우: 수평/수직 인접 두 칸이 모두 비어 있어야 함
        if (!is_free(cx, ny))
        {
            return false;
        }
        if (!is_free(nx, cy))
        {
            return false;
        }
        return true;
    }

    class GlobalPlanner final
    {
    private:
        cv::Mat map_;

    public:
        explicit GlobalPlanner();
        virtual ~GlobalPlanner();

        cv::Mat get_map() const;
        void set_map(const cv::Mat &map);
        std::vector<std::pair<int, int>> plan_by_a_star(int start_x, int start_y, int goal_x, int goal_y);
        std::vector<std::pair<int, int>> plan_by_a_star_8dir(int start_x, int start_y, int goal_x,  int goal_y, int margin = 80);

    public:
        using SharedPtr = std::shared_ptr<GlobalPlanner>;
    };
}

#endif // RCRAFT_GLOBAL_PLANNER_HPP
