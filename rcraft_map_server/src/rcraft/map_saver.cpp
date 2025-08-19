#include "rcraft/map_saver.hpp"

using namespace rcraft::map;

MapSaver::MapSaver() = default;
MapSaver::~MapSaver() = default;

namespace
{
    static bool
    write_image(const cv::Mat &map, const std::filesystem::path &out)
    {
        try
        {
            std::filesystem::create_directories(out.parent_path());
        }
        catch (const std::exception &e)
        {
            std::cerr << "[MapSaver] create_directories failed: " << e.what() << '\n';
        }
        
        const std::string out_str = out.string();
        
        if (!cv::imwrite(out_str, map))
        {
            std::cerr << "[MapSaver] imwrite failed: " << out_str << '\n';
            return false;
        }
        std::cout << "[MapSaver] wrote: " << out_str << '\n';
        return true;
    }
}

void
MapSaver::save(const cv::Mat &map, const std::string &path)
{
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    if (!cv::imwrite(path, map))
    {
        std::cerr << "[MapServer] imwrite failed: " << path << '\n';
    }
    else
    {
        std::cout << "[MapServer] wrote: " << path << '\n';
    }
    // fs::path out = path.is_absolute() ? path : (fs::current_path() / path);
    // out = out.lexically_normal();
    //
    // std::cout << "[INFO][MapSaver] out " << out << '\n';
    // std::cout << "[INFO][MapSaver] out parent path " << out.parent_path() << '\n';
    //
    // std::error_code ec;
    // fs::create_directories(out.parent_path(), ec);
    //
    // if (!cv::imwrite(out.string(), map))
    // {
    //     std::cerr << "[MapSaver] imwrite failed: " << out << '\n';
    // }
    // else
    // {
    //     std::cout << "[MapSaver] wrote: " << out << '\n';
    // }
}