#include "rcraft/global_planner.hpp"

using namespace rcraft::planner;

GlobalPlanner::GlobalPlanner() = default;
GlobalPlanner::~GlobalPlanner() = default;

void GlobalPlanner::load_map(const std::string &map_file_path)
{
    std::cout << "Path LoadMap : " << map_file_path << '\n';

    std::string ext;
    size_t dot = map_file_path.find_last_of('.');

    if (dot != std::string::npos)
    {
        ext = map_file_path.substr(dot + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    else
    {
        ext = "";
    }

    std::cout << "Path LoadMap ext : " << ext << '\n';

    if (ext == "pgm")
    {
        this->map_ = cv::imread(map_file_path, cv::IMREAD_GRAYSCALE);
    }
    else
    {
        cv::Mat color_map = cv::imread(map_file_path, cv::IMREAD_COLOR);

        if (color_map.empty())
        {
            std::cerr << "Path Image Map is Empty" << '\n';
            this->map_ = cv::Mat();
            return;
        }

        cv::cvtColor(color_map, this->map_, cv::COLOR_BGR2GRAY);
    }
}

/**
 * @brief Plan a path on the loaded map using the A* algorithm.
 *
 * This version uses:
 * - Bounding Box reduction (search area limited to start–goal rectangle + margin)
 * - Manhattan heuristic (4-connected grid)
 * - gScore array for pruning worse paths
 * - 1D index representation for performance
 *
 * @param start_x Start cell x-coordinate (pixel index).
 * @param start_y Start cell y-coordinate (pixel index).
 * @param goal_x  Goal cell x-coordinate (pixel index).
 * @param goal_y  Goal cell y-coordinate (pixel index).
 * @return std::vector<std::pair<int,int>> The computed path from start to goal (empty if no path).
 */
std::vector<std::pair<int, int>> GlobalPlanner::plan_by_a_star(int start_x, int start_y, int goal_x, int goal_y)
{
    /// [0] Validate map
    if (this->map_.empty())
    {
        std::cerr << "A* loaded map is empty\n";
        return {};
    }

    int w = this->map_.cols;
    int h = this->map_.rows;

    /// [1] Utility: boundary check
    auto inside = [&](int x, int y)
    {
        return static_cast<unsigned>(x) < static_cast<unsigned>(w) &&
            static_cast<unsigned>(y) < static_cast<unsigned>(h);
    };

    /// [2] Utility: obstacle check (free if gray >= 200)
    auto is_free = [&](int x, int y) -> bool
    {
        if (!inside(x, y))
        {
            return false;
        }
        const uchar *row = this->map_.ptr<uchar>(y);
        return row[x] >= 200;
    };

    /// [3] Validate start/goal positions
    if (!is_free(start_x, start_y) || !is_free(goal_x, goal_y))
    {
        return {};
    }

    /// [4] Bounding Box reduction
    constexpr int MARGIN = 80;
    int minx = std::max(0, std::min(start_x, goal_x) - MARGIN);
    int miny = std::max(0, std::min(start_y, goal_y) - MARGIN);
    int maxx = std::min(w - 1, std::max(start_x, goal_x) + MARGIN);
    int maxy = std::min(h - 1, std::max(start_y, goal_y) + MARGIN);
    int bw = maxx - minx + 1;
    int bh = maxy - miny + 1;
    int N = bw * bh;

    /// [5] Index conversion helpers
    auto idx = [&](int x, int y)
    {
        return (y - miny) * bw + (x - minx);
    };

    auto to_xy = [&](int i)
    {
        int yy = i / bw + miny;
        int xx = i % bw + minx;
        return std::pair<int, int>(xx, yy);
    };

    /// [6] Heuristic function (Manhattan distance)
    auto hfun = [&](int x, int y)
    {
        return std::abs(x - goal_x) + std::abs(y - goal_y);
    };

    /// [7] A* state arrays
    std::vector<int> g(N, INT_MAX);
    std::vector<int> parent(N, -1);
    std::vector<uint8_t> closed(N, 0);

    /// [8] Open list with (f-score, index)
    std::priority_queue<QN, std::vector<QN>, Cmp> open;

    /// [9] Initialize start node
    int s_i = idx(start_x, start_y);
    int g_i = idx(goal_x, goal_y);

    g[s_i] = 0;
    open.push({hfun(start_x, start_y), s_i});

    static constexpr int dx[4] = {1, -1, 0, 0};
    static constexpr int dy[4] = {0, 0, 1, -1};

    /// [10] Main A* loop
    while (!open.empty())
    {
        QN cur = open.top();
        open.pop();
        int i = cur.i;

        if (closed[i])
        {
            continue;
        }
        closed[i] = 1;

        /// [11] Goal reached
        if (i == g_i)
        {
            break;
        }

        auto [cx, cy] = to_xy(i);
        int cg = g[i];

        /// [12] Explore neighbors (4-connected)
        for (int d = 0; d < 4; ++d)
        {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx < minx || ny < miny || nx > maxx || ny > maxy)
            {
                continue;
            }
            if (!is_free(nx, ny))
            {
                continue;
            }

            int ni = idx(nx, ny);
            if (closed[ni])
            {
                continue;
            }

            int ng = cg + 1;
            if (ng < g[ni])
            {
                g[ni] = ng;
                parent[ni] = i;
                int f = ng + hfun(nx, ny);
                open.push({f, ni});
            }
        }
    }

    /// [13] Path reconstruction
    std::vector<std::pair<int, int>> path;
    if (g[g_i] == INT_MAX)
    {
        return path; // not found
    }

    for (int i = g_i; i != -1; i = parent[i])
    {
        path.push_back(to_xy(i));
        if (i == s_i)
        {
            break;
        }
    }

    std::reverse(path.begin(), path.end());

    /// [14] Return path
    return path;
}

std::vector<std::pair<int, int>>
GlobalPlanner::plan_by_a_star_8dir(int start_x, int start_y, int goal_x, int goal_y, int margin)
{
    // [0] 입력 맵 검증
    if (this->map_.empty() || this->map_.type() != CV_8UC1)
    {
        return {};
    }

    int w = this->map_.cols;
    int h = this->map_.rows;

    // [1] 경계/통로 유틸
    auto inside = [&](int x, int y)
    {
        return (unsigned)x < (unsigned)w && (unsigned)y < (unsigned)h;
    };
    auto is_free = [&](int x, int y) -> bool
    {
        if (!inside(x, y))
        {
            return false;
        }
        const uchar *row = this->map_.ptr<uchar>(y);
        return row[x] >= 200;
    };

    // [2] 시작/목표 유효성
    if (!is_free(start_x, start_y) || !is_free(goal_x, goal_y))
    {
        return {};
    }

    // [3] Bounding Box 축소
    int minx = std::max(0, std::min(start_x, goal_x) - margin);
    int miny = std::max(0, std::min(start_y, goal_y) - margin);
    int maxx = std::min(w - 1, std::max(start_x, goal_x) + margin);
    int maxy = std::min(h - 1, std::max(start_y, goal_y) + margin);

    int bw = maxx - minx + 1;
    int bh = maxy - miny + 1;
    int N  = bw * bh;

    auto idx = [&](int x, int y)
    {
        return (y - miny) * bw + (x - minx);
    };
    auto to_xy = [&](int i)
    {
        int yy = i / bw + miny;
        int xx = i % bw + minx;
        return std::pair<int, int>(xx, yy);
    };

    // [4] 상태 배열
    std::vector<int> g(N, std::numeric_limits<int>::max());
    std::vector<int> parent(N, -1);
    std::vector<uint8_t> closed(N, 0);

    std::priority_queue<QN, std::vector<QN>, Cmp> open;

    int s_i = idx(start_x, start_y);
    int g_i = idx(goal_x,  goal_y);

    g[s_i] = 0;
    open.push({octile_heuristic(start_x, start_y, goal_x, goal_y), s_i});

    // [5] 8방향 이웃
    static constexpr int dx[8] = { 1, -1,  0,  0,  1,  1, -1, -1 };
    static constexpr int dy[8] = { 0,  0,  1, -1,  1, -1,  1, -1 };

    // [6] 메인 루프
    while (!open.empty())
    {
        QN cur = open.top();
        open.pop();

        int i = cur.i;
        if (closed[i])
        {
            continue;
        }
        closed[i] = 1;

        if (i == g_i)
        {
            break;
        }

        auto [cx, cy] = to_xy(i);
        int cg = g[i];

        for (int d = 0; d < 8; ++d)
        {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            // 축소 영역 밖
            if (nx < minx || ny < miny || nx > maxx || ny > maxy)
            {
                continue;
            }
            // 장애물
            if (!is_free(nx, ny))
            {
                continue;
            }
            // corner-cutting 방지 (대각선일 때만 체크)
            if (!allow_diagonal_without_cutting(cx, cy, nx, ny, is_free))
            {
                continue;
            }

            int ni = idx(nx, ny);
            if (closed[ni])
            {
                continue;
            }

            // 이동 비용: 직선/대각선 구분
            int step = ((cx == nx) || (cy == ny)) ? kCostStraight : kCostDiagonal;
            int ng = cg + step;

            if (ng < g[ni])
            {
                g[ni] = ng;
                parent[ni] = i;
                int f = ng + octile_heuristic(nx, ny, goal_x, goal_y);
                open.push({f, ni});
            }
        }
    }

    // [7] 경로 복원
    std::vector<std::pair<int, int>> path;
    if (g[g_i] == std::numeric_limits<int>::max())
    {
        return path; // 경로 없음
    }
    for (int i = g_i; i != -1; i = parent[i])
    {
        path.push_back(to_xy(i));
        if (i == s_i)
        {
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    return path;
}
