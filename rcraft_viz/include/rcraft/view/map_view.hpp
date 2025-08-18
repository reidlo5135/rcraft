#ifndef RCRAFT_MAP_VIEW_HPP
#define RCRAFT_MAP_VIEW_HPP

#pragma once

/**
 * @file
 * @brief Interactive Qt view for visualizing a map, robot/goal markers, and a planned path.
 *
 * The class owns a @c QGraphicsScene (map image as a pixmap item) and overlays runtime
 * items such as the robot pose, the goal marker (set by right-click), and the global path.
 * Heavy planning work is delegated to a background @c QThread hosting @c PlannerWorker.
 *
 * Typical usage:
 * - Construct with a valid @c MapViewModel pointer.
 * - Connect @c MapViewModel::mapUpdated / resolutionChanged to update the scene.
 * - Right-click to set goal; the view emits @ref requestPlan which the worker handles.
 *
 * @note Z-order defaults (typical): map=0, path>map, goal>path, robot>path.
 */

#include <memory>
#include <vector>
#include <thread>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QPen>
#include <QBrush>
#include <QImage>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QPixmap>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QThread>

#include "rcraft/worker/planner_worker.hpp"
#include "rcraft/view_model/map_view_model.hpp"
#include "rcraft/unit/robot_unit.hpp"
#include "rcraft/unit/goal_unit.hpp"

namespace rcraft::viz
{
    /**
     * @class MapView
     * @brief Graphics-based widget that displays a map and orchestrates path planning interactions.
     *
     * @details
     * Responsibilities:
     * - Owns and renders the map pixmap inside a @c QGraphicsScene
     * - Renders robot (@c RobotUnit) and goal (@c GoalUnit) items
     * - Captures mouse events to update goal and trigger planning
     * - Manages a background planner thread (@c PlannerWorker in a @c QThread)
     * - Draws the resulting path as a @c QGraphicsPathItem
     *
     * Threading model:
     * - @c MapView lives on the GUI thread.
     * - Planning is executed in a dedicated @c QThread via queued signal/slot connections.
     *
     * Coordinate model:
     * - Mouse position is mapped to scene coordinates and converted to grid/pixel indices
     *   using @c mapResolution_ and @c mapSize_.
     */
    class MapView final : public QGraphicsView
    {
        Q_OBJECT

    private:
        /** @brief Initialize the default @c QGraphicsScene and basic view settings. */
        void setDefaultScene();

        /** @brief Create and position the "Load Map" button in the view. */
        void setMapLoadButton();

        /** @brief Create and add the default @c RobotUnit to the scene (initial pose/Z). */
        void setDefaultRobotUnit();

        /** @brief Ensure @c GoalUnit exists when needed and set its defaults (no-op until first use). */
        void setDefaultGoalUnit();

        /**
         * @brief Wire up Qt signal/slot connections between the view and its model/worker.
         *
         * @details
         * - Connects @c MapViewModel updates to @ref onMapUpdated and @ref onResolutionChanged.
         * - Connects planner signals to @ref onPlanReady / @ref onPlanError.
         * - Connects the "Load Map" button to the model's map selection handler.
         */
        void setQConnected();

        /**
         * @brief Create the planner worker and move it to a dedicated @c QThread.
         *
         * @note Only constructs and attaches; thread is started via @ref startPlannerThread().
         */
        void setPlannerThread();

        /** @brief Reposition the "Load Map" button based on the current widget size. */
        void updateButtonPosition() const;

        /** @brief Start the planner thread's event loop (safe to call once after setup). */
        void startPlannerThread() const;

        /** @brief Stop the planner thread gracefully and wait for completion. */
        void stopPlannerThread() const;

        // ====== Data members (lifecycle managed by MapView unless noted) ======

        /** @brief View-model providing map path, resolution, and updates (non-owning). */
        MapViewModel *viewModel_ = nullptr;

        /** @brief Scene that owns and renders all graphics items. */
        QGraphicsScene *scene_ = nullptr;

        /** @brief Map pixmap holder (nullptr until a map is loaded). */
        QGraphicsPixmapItem *pixmapItem_ = nullptr;

        /** @brief "Load Map" UI button (child of the view). */
        QPushButton *mapLoadButton_ = nullptr;

        /** @brief Map resolution (pixels-per-cell or scaling factor depending on usage). */
        float mapResolution_ = 1.0f;

        /** @brief Whether a valid map pixmap has been loaded into the scene. */
        bool mapLoaded_ = false;

        /** @brief Map dimensions in scene units (post-scaling). */
        QSize mapSize_;

        /** @brief Robot item (shared ownership through @c std::shared_ptr). */
        RobotUnit::SharedPtr robotUnit_;

        /** @brief Goal item (created on demand; shared ownership). */
        GoalUnit::SharedPtr goalUnit_;

        /** @brief Last mouse-mapped goal position in grid/pixel indices. */
        QPoint goalItemPos_{0, 0};

        /** @brief Dedicated thread that hosts the planner worker. */
        QThread* plannerThread_ = nullptr;

        /** @brief Planning worker that runs in @ref plannerThread_. */
        PlannerWorker *plannerWorker_ = nullptr;

        /** @brief Current path graphics item (owned by the scene). */
        QGraphicsPathItem *pathItem_ = nullptr;

        /** @brief Guard to prevent re-entrant planning requests. */
        bool isPathPlanning_ = false;

    private slots:
        /**
         * @brief Slot invoked when the planner returns a path.
         * @param path Sequence of (x,y) grid/pixel coordinates from start to goal.
         *
         * @details
         * Clears any previous path item and draws the new @c QPainterPath with cosmetic pen,
         * then resets the busy cursor / planning guard.
         */
        void onPlanReady(const std::vector<std::pair<int, int>> &path);

        /**
         * @brief Slot invoked when planning fails.
         * @param what Human-readable error message.
         *
         * @details
         * Resets the busy cursor / planning guard, and logs the error.
         */
        void onPlanError(const QString &what);

    signals:
        /**
         * @brief Signal requesting a planning operation in the worker thread.
         *
         * @param mapPath Absolute/relative map path to load or reuse.
         * @param sx Start X in grid/pixel coordinates (scene space → grid).
         * @param sy Start Y in grid/pixel coordinates (scene space → grid).
         * @param gx Goal  X in grid/pixel coordinates.
         * @param gy Goal  Y in grid/pixel coordinates.
         *
         * @note Intended to be connected with @c Qt::QueuedConnection to @c PlannerWorker::plan().
         */
        void requestPlan(const QString &mapPath, double sx, double sy, double gx, double gy);

    protected:
        /**
         * @brief Keep the whole map visible when the view is resized.
         * @param event Qt resize event.
         *
         * @details
         * Calls @c fitInView on the scene rect when a map is loaded, and repositions UI widgets.
         */
        void resizeEvent(QResizeEvent *event) override;

        /**
         * @brief Track mouse movement to update the tentative goal position.
         * @param event Qt mouse move event.
         *
         * @details
         * Maps viewport coordinates to scene coordinates, converts to grid indices using
         * @c mapResolution_, and stores in @ref goalItemPos_.
         */
        void mouseMoveEvent(QMouseEvent *event) override;

        /**
         * @brief Handle mouse clicks to set a goal and trigger planning.
         * @param event Qt mouse press event.
         *
         * @details
         * On right-click:
         * - Clamp goal within map bounds and (lazily) create/update @c GoalUnit
         * - Emit @ref requestPlan with current robot and goal positions
         * - Set a busy cursor and block re-entrant requests via @ref isPathPlanning_
         * Otherwise, forwards the event to the base class.
         */
        void mousePressEvent(QMouseEvent *event) override;

    public:
        /**
         * @brief Construct a new MapView.
         * @param viewModel Non-owning pointer to the associated view-model.
         * @param parent Optional Qt parent widget.
         *
         * @details
         * Initializes the scene, UI controls, default items, and the planner thread/worker.
         */
        explicit MapView(MapViewModel *viewModel, QWidget *parent = nullptr);

        /**
         * @brief Destructor. Stops the planner thread and releases scene-owned items.
         */
        ~MapView() override;

        /**
         * @brief Update internal resolution used for grid/scene conversions.
         * @param resolution New map resolution/scaling factor.
         */
        void onResolutionChanged(float resolution);

        /**
         * @brief Update the scene with a new @c QImage of the map.
         * @param qImage New map image (RGB888 expected).
         *
         * @details
         * Clears the scene, inserts the pixmap item, draws a border, fits the view, and
         * re-adds the robot/goal/path items with proper Z-order.
         */
        void onMapUpdated(const QImage &qImage);

    public:
        /// @brief Shared pointer alias.
        using SharedPtr = std::shared_ptr<MapView>;
        /// @brief Unique pointer alias.
        using UniquePtr = std::unique_ptr<MapView>;
    };
}

#endif // RCRAFT_MAP_VIEW_HPP
