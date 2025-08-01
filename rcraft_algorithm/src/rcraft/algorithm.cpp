#include "rcraft/algorithm.hpp"

using namespace rcraft::algorithm;

Path::Path() = default;
Path::~Path() = default;

void Path::load_map(const std::string &map_file_path)
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

std::vector<std::pair<int, int>> Path::calculate_path_a_star(int start_x, int start_y, int goal_x, int goal_y)
{
    // [0] Check for empty map
    if (this->map_.empty())
    {
        std::cerr << "A* loaded map is empty" << std::endl;
        return {};
    }

    std::chrono::seconds();

    // [1] Get map dimensions
    int w = this->map_.cols;
    int h = this->map_.rows;

    std::cout << "A* Map width, height (" << w << ", " << h << ")" << '\n';
    std::cout << "A* start x : " << start_x << ", y : " << start_y << '\n';
    std::cout << "A* goal x : " << goal_x << ", y : " << goal_y << '\n';

    // [2] Lambda for boundary and obstacle checking
    auto valid = [&](int x, int y)
    {
        const bool &in = (x >= 0 && y >= 0 && x < w && y < h);

        if (!in)
        {
            std::cout << "A* pixel invalid[OOR] : (" << x << ", " << y << ")" << '\n';
        }

        uchar color = this->map_.at<uchar>(y, x);
        std::cout << "A* color : " << static_cast<int>(color) << '\n';
        std::cout << "A* Goal pixel: " << static_cast<int>(this->map_.at<uchar>(goal_y, goal_x)) << std::endl;

        return in && color >= 200;
    };

    // [3] Closed list to mark visited nodes
    std::vector<std::vector<bool>> closed(h, std::vector<bool>(w, false));

    // [4] Custom comparator for A* open set (priority queue)
    auto cmp = [](Node::SharedPtr a, Node::SharedPtr b) { return a->priority > b->priority; };
    std::priority_queue<Node::SharedPtr, std::vector<Node::SharedPtr>, decltype(cmp)> open(cmp);

    // [5] Create and insert the start node
    Node::SharedPtr start = std::make_shared<Node>(start_x, start_y, 0, heuristic(start_x, start_y, goal_x, goal_y), nullptr);
    open.push(start);

    // [6] For storing goal node pointer (for path tracing)
    Node::SharedPtr last = nullptr;

    // [7] Main A* search loop
    while (!open.empty())
    {
        // [8] Get the node with the lowest priority (cost + heuristic)
        Node::SharedPtr cur = open.top();
        open.pop();

        // [9] Skip if already closed (visited)
        if (closed[cur->y][cur->x])
        {
            continue;
        }
        closed[cur->y][cur->x] = true;

        // [10] Check if goal is reached
        if (cur->x == goal_x && cur->y == goal_y)
        {
            last = cur;
            std::cout << "A* Goal Reached last x, y (" << last->x << ", " << last->y << ")";
            break;
        }

        // [11] Explore 4-connected neighbors
        static constexpr int dx[4] = {1, -1, 0, 0};
        static constexpr int dy[4] = {0, 0, 1, -1};

        for (int d = 0; d < 4; ++d)
        {
            // Vertex (corner) Filtering only
            // (0,0) : only RIGHT and DOWN
            if (cur->x == 0 && cur->y == 0 && (d == 1 || d == 3))
            {
                std::cout << "A* LT Skipping..." << '\n';
                std::cout << "cur x, y (" << cur->x << ", " << cur->y << ")" << '\n';
                continue;
            }
            // (w-1,0) : only LEFT and DOWN
            if (cur->x == w - 1 && cur->y == 0 && (d == 0 || d == 3))
            {
                std::cout << "A* RT Skipping..." << '\n';
                std::cout << "cur x, y (" << cur->x << ", " << cur->y << ")" << '\n';
                continue;
            }
            // (0,h-1) : only RIGHT and UP
            if (cur->x == 0 && cur->y == h - 1 && (d == 1 || d == 2))
            {
                std::cout << "A* LB Skipping..." << '\n';
                std::cout << "cur x, y (" << cur->x << ", " << cur->y << ")" << '\n';
                continue;
            }
            // (w-1,h-1) : only LEFT and UP
            if (cur->x == w - 1 && cur->y == h - 1 && (d == 0 || d == 2))
            {
                std::cout << "A* RB Skipping..." << '\n';
                std::cout << "cur x, y (" << cur->x << ", " << cur->y << ")" << '\n';
                continue;
            }

            const int &nx = cur->x + dx[d];
            const int &ny = cur->y + dy[d];

            std::cout << "==================================================" << '\n';
            std::cout << "A* Explore 4-connected pixels" << '\n';
            std::cout << "current x, y (" << cur->x << ", " << cur->y << ")" << '\n';
            std::cout << "next x, y (" << nx << ", " << ny << ")" << '\n';
            std::cout << "==================================================" << '\n';

            // [12] Skip invalid or already closed neighbors
            if (!valid(nx, ny) || closed[ny][nx])
                continue;

            // [13] Create neighbor node and push to open list
            Node::SharedPtr next = std::make_shared<Node>(nx, ny, cur->cost + 1, cur->cost + 1 + heuristic(nx, ny, goal_x, goal_y), cur);
            open.push(next);

            std::cout << "A* OpenSet size: " << open.size() << std::endl;
        }
    }

    // [14] Reconstruct path from goal to start using parent pointers
    std::vector<std::pair<int, int>> path;
    while (last)
    {
        path.emplace_back(last->x, last->y);
        last = std::shared_ptr<Node>(last->parent);
    }
    std::reverse(path.begin(), path.end());

    // [15] Return the path (empty if not found)
    return path;
}