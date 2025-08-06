#include "rcraft/view/map_view.hpp"

using namespace rcraft::viz;

MapView::MapView(MapViewModel *viewModel, QWidget *parent)
    : QGraphicsView(parent)
    , viewModel_(viewModel)
{
    SetScene();

    this->mapLoadButton_ = new QPushButton("Load Map", this);
    this->mapLoadButton_->setFixedSize(120, 30);
    this->mapLoadButton_->setStyleSheet("background-color: red; color: white;");
    connect(this->mapLoadButton_, &QPushButton::clicked, this->viewModel_, &MapViewModel::handleMapSelect);

    this->mapResolution_ = 1.0f;
    this->mapLoaded_ = false;
    this->mapSize_ = QSize(0, 0);
    
    // robotItem_ = new RobotItem(this->mapResolution_);
    // robotItem_->setRotation(270);
    // robotItem_->setPos(0, 0);
    // robotItem_->setZValue(1);
    // this->scene_->addItem(robotItem_);

    // goalItem_ = nullptr;
    // goalItemPos_ = QPoint(0, 0);

    setMouseTracking(true);
    
    connect(this->viewModel_, &MapViewModel::mapUpdated, this, &MapView::OnMapUpdated);
    connect(this->viewModel_, &MapViewModel::resolutionChanged, this, &MapView::OnResolutionChanged);
}

MapView::~MapView()
{
}

void MapView::SetScene()
{
    this->scene_ = new QGraphicsScene(this);
    setScene(this->scene_);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setMinimumSize(1600, 800);
    setBackgroundBrush(Qt::black);
}

void MapView::OnResolutionChanged(float resolution)
{
    this->mapResolution_ = resolution;
}

void MapView::resizeEvent(QResizeEvent* event)
{
    if (this->mapLoaded_)
        fitInView(this->scene_->sceneRect(), Qt::KeepAspectRatio);
    UpdateButtonPosition();
    QGraphicsView::resizeEvent(event);
}

void MapView::UpdateButtonPosition()
{
    int marginX = 20, marginY = 20;
    if (this->mapLoadButton_)
        this->mapLoadButton_->move(width() - this->mapLoadButton_->width() - marginX, marginY);
}

void MapView::OnMapUpdated(const QImage& qImage)
{
    if (qImage.isNull())
        return;

    // if (robotItem_)
    //     this->scene_->removeItem(robotItem_);

    QPixmap pixmap = QPixmap::fromImage(qImage);
    this->scene_->clear();

    this->pixmapItem_ = new QGraphicsPixmapItem(pixmap);
    this->pixmapItem_->setScale(this->mapResolution_);
    this->pixmapItem_->setZValue(0);
    this->scene_->addItem(this->pixmapItem_);

    int widthM = static_cast<int>(qImage.width() * this->mapResolution_);
    int heightM = static_cast<int>(qImage.height() * this->mapResolution_);
    this->mapSize_ = QSize(widthM, heightM);
    this->scene_->setSceneRect(0, 0, widthM, heightM);
    qDebug() << "[INFO][V] OnMapUpdated:" << qImage.size() << qImage.format() << widthM << heightM << this->mapResolution_;

    this->mapLoaded_ = true;

    QPen borderPen(Qt::gray);
    borderPen.setWidth(2);
    QRectF rect = this->scene_->sceneRect();
    this->scene_->addRect(rect, borderPen, QBrush(Qt::NoBrush));

    fitInView(this->scene_->sceneRect(), Qt::KeepAspectRatioByExpanding);

    // robotItem_->setPos(10, abs(10 - heightM));
    // robotItem_->setZValue(1);
    // this->scene_->addItem(robotItem_);
}

void MapView::mouseMoveEvent(QMouseEvent* event)
{
    if (!this->mapLoaded_)
    {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }
    QPointF scenePos = mapToScene(event->pos());
    int x = static_cast<int>(scenePos.x() / this->mapResolution_);
    int y = static_cast<int>(scenePos.y() / this->mapResolution_);

    // if (x >= 0 && y >= 0)
    //     goalItemPos_ = QPoint(x, y);

    QGraphicsView::mouseMoveEvent(event);
}

void MapView::mousePressEvent(QMouseEvent* event)
{
    if (!this->mapLoaded_)
    {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    int x = goalItemPos_.x();
    int y = goalItemPos_.y();
    int width = this->mapSize_.width();
    int height = this->mapSize_.height();

    if (event->button() == Qt::RightButton)
    {
        x = std::max(0, std::min(x, width));
        y = std::max(0, std::min(y, height));

        // if (!goalItem_) {
        //     goalItem_ = new GoalItem(this->mapResolution_);
        //     this->scene_->addItem(goalItem_);
        // }
        // goalItem_->setPos(x, y);
        // goalItem_->setZValue(1);
        // goalItem_->update();
        // this->scene_->update();
        //
        // qDebug() << "[INFO][V] ready for path finding\n\t"
        //          << "robot x, y:" << robotItem_->x() << robotItem_->y()
        //          << "goal x, y:" << goalItem_->x() << goalItem_->y();
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}