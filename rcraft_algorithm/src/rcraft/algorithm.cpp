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
    if (this->map_.empty())
    {
        std::cerr << "Path::calculate_path_a_star: map is empty" << std::endl;
        return {};
    }

    int h = this->map_.rows;
    int w = this->map_.cols;

    std::cout << "A* height : " << h << ", w : " << w << '\n';

    auto valid = [&](int x, int y)
    {
        bool in = (x >= 0 && y >= 0 && x < w && y < h);
        if (!in)
            std::cout << "OutOfRange: (" << x << ", " << y << ")" << std::endl;
        return in && this->map_.at<uchar>(y, x) < 128;
    };

    std::vector<std::vector<bool>> closed(h, std::vector<bool>(w, false));
    auto cmp = [](Node *a, Node *b) { return a->priority > b->priority; };
    std::priority_queue<Node *, std::vector<Node *>, decltype(cmp)> open(cmp);

    std::vector<Node*> node_gc;

    Node *start = new Node(start_x, start_y, 0, heuristic(start_x, start_y, goal_x, goal_y), nullptr);
    open.push(start);
    node_gc.push_back(start);

    Node *last = nullptr;

    while (!open.empty())
    {
        Node *cur = open.top();
        open.pop();

        if (closed[cur->y][cur->x])
        {
            delete cur;
            continue;
        }
        closed[cur->y][cur->x] = true;

        if (cur->x == goal_x && cur->y == goal_y)
        {
            last = cur;
            break;
        }
        static constexpr int dx[4] = {1, -1, 0, 0};
        static constexpr int dy[4] = {0, 0, 1, -1};
        for (int d = 0; d < 4; ++d)
        {
            const int nx = cur->x + dx[d];
            const int ny = cur->y + dy[d];

            if (!valid(nx, ny) || closed[ny][nx])
                continue;

            Node* next = new Node(nx, ny, cur->cost + 1, cur->cost + 1 + heuristic(nx, ny, goal_x, goal_y), cur);
            open.push(next);
            node_gc.push_back(next);
        }
    }

    std::vector<std::pair<int, int>> path;
    while (last)
    {
        path.emplace_back(last->x, last->y);
        last = last->parent;
    }
    std::reverse(path.begin(), path.end());

    for (Node *n : node_gc) delete n;

    return path;
}
