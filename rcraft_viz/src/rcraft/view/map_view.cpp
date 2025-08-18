#include "rcraft/view/map_view.hpp"

using namespace rcraft::viz;

/**
 * @brief Construct a MapView, set up scene/UI/items, and start the planner thread.
 *
 * @param viewModel Non-owning pointer to the associated MapViewModel.
 * @param parent Optional parent widget.
 *
 * @details
 * Initializes:
 * - Default scene and basic view hints via setDefaultScene()
 * - "Load Map" button via setMapLoadButton()
 * - Default robot/goal items via setDefaultRobotUnit()/setDefaultGoalUnit()
 * - Planner worker and thread via setPlannerThread()
 * - Qt signal/slot connections via setQConnected()
 * Finally, starts the planner thread event loop.
 */
MapView::MapView(MapViewModel *viewModel, QWidget *parent)
    : QGraphicsView(parent)
    , viewModel_(viewModel)
    , scene_(nullptr)
    , pixmapItem_(nullptr)
    , mapLoadButton_(nullptr)
    , mapResolution_(1.0f)
    , mapLoaded_(false)
    , mapSize_(QSize(0, 0))
    , goalUnit_(nullptr)
    , goalItemPos_(QPoint(0, 0))
    , plannerThread_(nullptr)
    , plannerWorker_(nullptr)
    , pathItem_(nullptr)
    , isPathPlanning_(false)
{
    this->setDefaultScene();
    this->setMapLoadButton();
    this->setDefaultRobotUnit();
    this->setDefaultGoalUnit();
    this->setPlannerThread();
    this->setQConnected();

    this->startPlannerThread();
}

/**
 * @brief Destructor; stops the planner thread gracefully.
 *
 * @details
 * Calls stopPlannerThread() to quit and join the worker thread.
 * Scene-owned items are released by Qt's parent–child ownership model.
 */
MapView::~MapView()
{
    this->stopPlannerThread();
}

/**
 * @brief Initialize the QGraphicsScene and base view properties.
 *
 * @details
 * - Creates a new scene owned by this view
 * - Sets render hints (antialiasing, smooth pixmap transform)
 * - Sets a minimum widget size and a black background
 * - Resets internal map state and enables mouse tracking
 */
void MapView::setDefaultScene()
{
    this->scene_ = new QGraphicsScene(this);
    setScene(this->scene_);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setMinimumSize(1600, 800);
    setBackgroundBrush(Qt::black);

    this->mapResolution_ = 1.0f;
    this->mapLoaded_ = false;
    this->mapSize_ = QSize(0, 0);
    setMouseTracking(true);
}

/**
 * @brief Create and style the "Load Map" button.
 *
 * @details
 * The button is a child of the view so it floats on top.
 * Position is managed by updateButtonPosition() in resizeEvent().
 */
void MapView::setMapLoadButton()
{
    this->mapLoadButton_ = new QPushButton("Load Map", this);
    this->mapLoadButton_->setFixedSize(120, 30);
    this->mapLoadButton_->setStyleSheet("background-color: red; color: white;");
}

/**
 * @brief Create and insert the default RobotUnit into the scene.
 *
 * @details
 * - The robot is constructed with the current mapResolution_
 * - Initial rotation set to 270 degrees (facing up on typical map imagery)
 * - Initial position set to (0,0) and Z-value to 1 (above the map pixmap)
 */
void MapView::setDefaultRobotUnit()
{
    this->robotUnit_ = std::make_shared<RobotUnit>(static_cast<double>(this->mapResolution_));
    this->robotUnit_->setRotation(270);
    this->robotUnit_->setPos(0, 0);
    this->robotUnit_->setZValue(1);

    if (this->scene_ != nullptr)
    {
        this->scene_->addItem(this->robotUnit_.get());
    }
    else
    {
        qDebug() << "[INFO][V] setRobotUnit Scene is null" << '\n';
        return;
    }
}

/**
 * @brief Wire up Qt signal/slot connections between the view, model, and planner worker.
 *
 * @details
 * - mapLoadButton_ -> MapViewModel::handleMapSelect()
 * - MapViewModel::mapUpdated -> MapView::onMapUpdated()
 * - MapViewModel::resolutionChanged -> MapView::onResolutionChanged()
 * - MapView::requestPlan -> PlannerWorker::plan() (queued, cross-thread)
 * - PlannerWorker::planReady -> MapView::onPlanReady() (queued)
 * - PlannerWorker::planError -> MapView::onPlanError() (queued)
 */
void MapView::setQConnected()
{
    connect(this->mapLoadButton_, &QPushButton::clicked, this->viewModel_, &MapViewModel::handleMapSelect);
    connect(this->viewModel_, &MapViewModel::mapUpdated, this, &MapView::onMapUpdated);
    connect(this->viewModel_, &MapViewModel::resolutionChanged, this, &MapView::onResolutionChanged);
    connect(this, &MapView::requestPlan, this->plannerWorker_, &PlannerWorker::plan, Qt::QueuedConnection);
    connect(this->plannerWorker_, &PlannerWorker::planReady, this, &MapView::onPlanReady, Qt::QueuedConnection);
    connect(this->plannerWorker_, &PlannerWorker::planError, this, &MapView::onPlanError, Qt::QueuedConnection);
}

/**
 * @brief Reset goal-related state; goal item is created on first use.
 */
void MapView::setDefaultGoalUnit()
{
    this->goalUnit_ = nullptr;
    this->goalItemPos_ = QPoint(0, 0);
}

/**
 * @brief Create the planner worker and move it to a dedicated QThread.
 *
 * @details
 * The thread is started separately in startPlannerThread().
 */
void MapView::setPlannerThread()
{
    this->plannerThread_ = new QThread(this);
    this->plannerWorker_ = new PlannerWorker();
    this->plannerWorker_->moveToThread(this->plannerThread_);
}

/**
 * @brief Start the planner thread event loop.
 *
 * @note Safe to call once after setPlannerThread().
 */
void MapView::startPlannerThread() const
{
    if (this->plannerThread_)
    {
        this->plannerThread_->start();
    }
}

/**
 * @brief Stop the planner thread and wait for it to finish.
 *
 * @details
 * Calls quit() then wait() to ensure a clean shutdown of the worker's event loop.
 */
void MapView::stopPlannerThread() const
{
    if (this->plannerThread_)
    {
        this->plannerThread_->quit();
        this->plannerThread_->wait();
    }
}

/**
 * @brief Update the map resolution scaling factor.
 *
 * @param resolution New scaling factor used to convert scene to grid/pixel coordinates.
 */
void MapView::onResolutionChanged(float resolution)
{
    this->mapResolution_ = resolution;
}

/**
 * @brief Handle widget resize; keeps the whole map visible and repositions UI.
 *
 * @param event Qt resize event.
 *
 * @details
 * If a map is loaded, calls fitInView() on the scene rect with KeepAspectRatio.
 * Updates the "Load Map" button placement, then forwards the event to the base class.
 */
void MapView::resizeEvent(QResizeEvent* event)
{
    if (this->mapLoaded_)
    {
        fitInView(this->scene_->sceneRect(), Qt::KeepAspectRatio);
    }
    this->updateButtonPosition();
    QGraphicsView::resizeEvent(event);
}

/**
 * @brief Position the "Load Map" button at the top-right corner with margins.
 */
void MapView::updateButtonPosition() const
{
    if (this->mapLoadButton_)
    {
        const int marginX = 20;
        const int marginY = 20;
        this->mapLoadButton_->move(width() - this->mapLoadButton_->width() - marginX, marginY);
    }
}

/**
 * @brief Update the scene with the provided map image.
 *
 * @param qImage Map image (expected RGB888).
 *
 * @details
 * - Removes robot/goal/path items from the scene (they will be re-added)
 * - Clears the scene and inserts the map pixmap with Z=0
 * - Updates scene rect and internal map size (scaled by mapResolution_)
 * - Draws a border and fits the view to the scene rect
 * - Re-adds the robot item at a default position and Z=1
 */
void MapView::onMapUpdated(const QImage &qImage)
{
    if (qImage.isNull())
    {
        qDebug() << "[ERROR][V] Map Image is Null" << '\n';
        return;
    }

    if (this->robotUnit_)
    {
        this->scene_->removeItem(this->robotUnit_.get());
    }

    if (this->goalUnit_)
    {
        this->scene_->removeItem(this->goalUnit_.get());
    }

    if (this->pathItem_)
    {
        this->scene_->removeItem(this->pathItem_);
    }

    QPixmap pixmap = QPixmap::fromImage(qImage);
    this->scene_->clear();

    this->pixmapItem_ = new QGraphicsPixmapItem(pixmap);
    this->pixmapItem_->setScale(this->mapResolution_);
    this->pixmapItem_->setZValue(0);
    this->scene_->addItem(this->pixmapItem_);

    int mapWidth = static_cast<int>(qImage.width() * this->mapResolution_);
    int mapHeight = static_cast<int>(qImage.height() * this->mapResolution_);
    this->mapSize_ = QSize(mapWidth, mapHeight);
    this->scene_->setSceneRect(0, 0, mapWidth, mapHeight);
    this->mapLoaded_ = true;
    qDebug() << "[INFO][V] OnMapUpdated:" << qImage.size() << qImage.format() << mapWidth << mapHeight << this->mapResolution_;

    QPen borderPen(Qt::gray);
    borderPen.setWidth(2);
    QRectF rect = this->scene_->sceneRect();
    this->scene_->addRect(rect, borderPen, QBrush(Qt::NoBrush));

    fitInView(this->scene_->sceneRect(), Qt::KeepAspectRatioByExpanding);

    this->robotUnit_->setPos(10, std::abs(10 - mapHeight));
    this->robotUnit_->setZValue(1);
    this->scene_->addItem(this->robotUnit_.get());
}

/**
 * @brief Track mouse movement to update the tentative goal grid position.
 *
 * @param event Qt mouse move event.
 *
 * @details
 * - Converts viewport coordinates to scene coordinates
 * - Divides by mapResolution_ to get grid/pixel indices
 * - Stores clamped non-negative position in goalItemPos_
 */
void MapView::mouseMoveEvent(QMouseEvent *event)
{
    if (!this->mapLoaded_)
    {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    const QPointF scenePos = mapToScene(event->pos());
    const int x = static_cast<int>(scenePos.x() / this->mapResolution_);
    const int y = static_cast<int>(scenePos.y() / this->mapResolution_);

    if (x >= 0 && y >= 0)
    {
        this->goalItemPos_ = QPoint(x, y);
    }

    QGraphicsView::mouseMoveEvent(event);
}

/**
 * @brief Handle mouse clicks: right-click places/updates the goal and triggers planning.
 *
 * @param event Qt mouse press event.
 *
 * @details
 * On right mouse button:
 * - Clamps the goal position inside map bounds
 * - Lazily creates GoalUnit if it does not exist and adds to the scene
 * - Ensures the goal has a higher Z so it remains visible
 * - If no planning is in progress, sets busy cursor, logs state, and emits requestPlan
 * For other buttons, forwards to the base class handler.
 */
void MapView::mousePressEvent(QMouseEvent *event)
{
    if (!this->mapLoaded_)
    {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    int x = this->goalItemPos_.x();
    int y = this->goalItemPos_.y();
    const int width = this->mapSize_.width();
    const int height = this->mapSize_.height();

    if (event->button() == Qt::RightButton)
    {
        x = std::max(0, std::min(x, width));
        y = std::max(0, std::min(y, height));

        if (!this->goalUnit_)
        {
            this->goalUnit_ = std::make_shared<GoalUnit>(static_cast<double>(this->mapResolution_));
        }
        this->goalUnit_->setPos(x, y);
        this->goalUnit_->setZValue(10);
        this->scene_->addItem(this->goalUnit_.get());

        if (this->isPathPlanning_)
        {
            qDebug() << "[INFO][V] planning in progress, skip new request" << '\n';
            return;
        }
        this->isPathPlanning_ = true;
        setCursor(Qt::BusyCursor);

        qDebug() << "[INFO][V] ready for path finding\n\t"
                << "map path : " << this->viewModel_->getMapPath() << "\n\t"
                << "robot (x, y) (" << this->robotUnit_->x() << ", " << this->robotUnit_->y() << ")"
                << "goal (x, y) (" << this->goalUnit_->x() << ", " <<  this->goalUnit_->y() << ")" << '\n';

        emit requestPlan(this->viewModel_->getMapPath(),
            this->robotUnit_->x(), this->robotUnit_->y(),
            this->goalUnit_->x(), this->goalUnit_->y());
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}

/**
 * @brief Slot called when a planned path is ready; draws the polyline on the scene.
 *
 * @param path Sequence of (x, y) grid points from start to goal.
 *
 * @details
 * - Clears any existing path item from the scene
 * - Builds a QPainterPath connecting all points (scaled by mapResolution_)
 * - Uses a cosmetic, round-capped/joined pen for consistent on-screen thickness
 * - Sets Z to 2 (above map; ensure goal/robot Z if necessary)
 * - Resets planning guard and cursor
 *
 * @note If Z-order requires the path above goal/robot, increase setZValue() accordingly.
 */
void MapView::onPlanReady(const std::vector<std::pair<int,int>> &path)
{
    this->isPathPlanning_ = false;
    unsetCursor();

    if (this->pathItem_)
    {
        this->scene_->removeItem(this->pathItem_);
        delete this->pathItem_;
        this->pathItem_ = nullptr;
    }

    if (path.empty())
    {
        qDebug() << "[INFO][V] empty path";
        return;
    }

    QPainterPath painterPath;
    painterPath.moveTo(path.front().first * this->mapResolution_,
                       path.front().second * this->mapResolution_);

    for (size_t i = 1; i < path.size(); ++i)
    {
        painterPath.lineTo(path[i].first * this->mapResolution_,
                           path[i].second * this->mapResolution_);
    }

    this->pathItem_ = new QGraphicsPathItem(painterPath);
    QPen pen(Qt::yellow);
    pen.setWidthF(3.0f);
    pen.setCosmetic(true);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    this->pathItem_->setPen(pen);
    this->pathItem_->setZValue(2);
    this->scene_->addItem(this->pathItem_);
}

/**
 * @brief Slot called when planning fails; logs the error and clears the busy state.
 *
 * @param what Human-readable error message emitted by the planner worker.
 */
void MapView::onPlanError(const QString &what)
{
    this->isPathPlanning_ = false;
    unsetCursor();
    qDebug() << "[ERROR][V] path finding error:" << what;
}
