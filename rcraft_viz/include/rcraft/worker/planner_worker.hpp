#ifndef RCRAFT_PLANNER_WORKER_HPP
#define RCRAFT_PLANNER_WORKER_HPP

#pragma once

/**
 * @file
 * @brief Asynchronous path-planning worker that runs in a QThread and bridges
 *        the UI (Qt) world with the planning backend (rcraft::planner::GlobalPlanner).
 *
 * Typical usage:
 * @code
 * auto thread = new QThread(this);
 * auto worker = new rcraft::viz::PlannerWorker();
 * worker->moveToThread(thread);
 *
 * // Connect request/response (queued) across threads
 * connect(this,   &MyView::requestPlan,
 *         worker, &rcraft::viz::PlannerWorker::plan,
 *         Qt::QueuedConnection);
 *
 * connect(worker, &rcraft::viz::PlannerWorker::planReady,
 *         this,   &MyView::onPlanReady,
 *         Qt::QueuedConnection);
 *
 * connect(worker, &rcraft::viz::PlannerWorker::planError,
 *         this,   &MyView::onPlanError,
 *         Qt::QueuedConnection);
 *
 * thread->start();
 * @endcode
 *
 * @note When using queued connections across threads with custom types, make sure the
 *       type is registered with Qt's meta-type system (e.g., `qRegisterMetaType<StdPath>()`).
 *       For STL containers like `std::vector<std::pair<int,int>>`, either register them or
 *       switch to Qt containers (e.g., `QVector<QPoint>`).
 */

#include <utility>
#include <memory>
#include <chrono>

#include <QObject>
#include <QString>
#include <vector>
#include <utility>
#include <memory>
#include <QDebug>
#include <QMetaType>

#include <rcraft/global_planner.hpp>

namespace rcraft::viz
{

/**
 * @class PlannerWorker
 * @brief Thin QObject wrapper around @c rcraft::planner::GlobalPlanner intended to live
 *        in its own @c QThread. Heavy planning work is executed inside the @ref plan slot.
 *
 * @details
 * - Move this object to a dedicated QThread via @c moveToThread().
 * - Call @ref plan via a queued connection from the UI thread.
 * - Results are delivered through @ref planReady (success) or @ref planError (failure).
 * - The worker owns a shared instance of @c GlobalPlanner and reuses it across calls.
 *
 * @thread_safety
 * The class is thread-affine (inherits QObject). All member functions must be called
 * from the object's thread unless documented otherwise. Use queued connections from other threads.
 */
class PlannerWorker final : public QObject
{
    Q_OBJECT

    private:
        /**
         * @brief Planning backend reused across invocations.
         *
         * @note The planner performs disk I/O on @ref plan() (map loading) unless the
         *       underlying implementation caches the last map. Consider adding a simple
         *       path-based cache in the worker if repeated calls use the same map.
         */
        planner::GlobalPlanner::SharedPtr global_planner_;

    public:
        /**
         * @brief Construct a new PlannerWorker.
         * @param parent Optional QObject parent. Ownership follows Qt's parent–child model.
         */
        explicit PlannerWorker(QObject *parent = nullptr);

        /**
         * @brief Destroy the PlannerWorker.
         *
         * @note No special action is required; Qt will handle destruction via the
         *       parent–child relationship or @c deleteLater() when the thread stops.
         */
        ~PlannerWorker() override;

    public slots:
        /**
         * @brief Execute a planning request (blocking in the worker thread).
         *
         * @details
         * Loads the map from @p mapPath (if required) and computes a path from (sx, sy) to (gx, gy)
         * in pixel/grid coordinates. On success, emits @ref planReady with the full path.
         * On failure (I/O error, no path, exceptions), emits @ref planError with a message.
         *
         * @param mapPath Absolute or relative path to the map image file.
         * @param sx Start X in pixels (grid column).
         * @param sy Start Y in pixels (grid row).
         * @param gx Goal  X in pixels (grid column).
         * @param gy Goal  Y in pixels (grid row).
         *
         * @emit planReady
         * @emit planError
         *
         * @note This slot is intended to be invoked via a queued connection from another thread.
         */
        void plan(const QString &mapPath, double sx, double sy, double gx, double gy);

    signals:
        /**
         * @brief Emitted when a path is successfully computed.
         *
         * @param path Sequence of (x, y) grid coordinates in pixel units forming the path
         *             from start to goal. The vector is non-empty on success.
         *
         * @note If you transmit this type across threads via queued connections, ensure it is
         *       registered with Qt's meta-type system (e.g., @c qRegisterMetaType<std::vector<std::pair<int,int>>>()),
         *       or consider using @c QVector<QPoint> instead.
         */
        void planReady(const std::vector<std::pair<int, int>> &path);

        /**
         * @brief Emitted when an error occurs during planning.
         *
         * @param what Human-readable error message (e.g., map I/O failure, planner exception).
         */
        void planError(const QString &what);

    public:
        /// Shared pointer alias for convenience.
        using SharedPtr = std::shared_ptr<PlannerWorker>;
};

} // namespace rcraft::viz

#endif // RCRAFT_PLANNER_WORKER_HPP
