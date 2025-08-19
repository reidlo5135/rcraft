#include "rcraft/worker/planner_worker.hpp"

using namespace rcraft::viz;

PlannerWorker::PlannerWorker(QObject *parent)
    : QObject(parent)
    , globalPlanner_(std::make_shared<planner::GlobalPlanner>())
{
}

PlannerWorker::~PlannerWorker() = default;

/**
 * @brief Executes a planning request in the worker thread.
 *
 * Loads the map (from @p mapPath) and computes a global path from (sx, sy) to (gx, gy)
 * using the 8-directional A* with octile heuristic and corner-cutting prevention.
 *
 * This slot is designed to be invoked via a queued connection from the UI thread.
 * On success, it emits #planReady; on error, it emits #planError.
 *
 * @param driveMap Map for Driving.
 * @param sx Start X in pixel/grid coordinates (will be cast to int).
 * @param sy Start Y in pixel/grid coordinates (will be cast to int).
 * @param gx Goal  X in pixel/grid coordinates (will be cast to int).
 * @param gy Goal  Y in pixel/grid coordinates (will be cast to int).
 *
 * @emit planReady Emitted with the computed path.
 * @emit planError Emitted with an error message if planning fails.
 *
 * @note If repeated calls use the same map, consider caching the last loaded path
 *       inside this worker to avoid reloading from disk on every request.
 */
void PlannerWorker::plan(const cv::Mat &driveMap, double sx, double sy, double gx, double gy)
{
    using Clock = std::chrono::steady_clock;

    try
    {
        const std::chrono::time_point<Clock> start = Clock::now();

        this->globalPlanner_->set_map(driveMap);
        const std::vector<std::pair<int, int>> &path = this->globalPlanner_->plan_by_a_star(
            static_cast<int>(sx), static_cast<int>(sy),
            static_cast<int>(gx), static_cast<int>(gy));

        const std::chrono::time_point<Clock> end = Clock::now();
        const long long msDiffer = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        qDebug() << "[INFO][W] Plan Path Size:" << path.size() << "| Time:" << msDiffer << "ms";

        emit planReady(path);
    }
    catch (const std::exception &e)
    {
        emit planError(QString::fromUtf8(e.what()));
    }
}