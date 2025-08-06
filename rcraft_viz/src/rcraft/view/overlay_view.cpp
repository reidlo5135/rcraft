#include "rcraft/view/overlay_view.hpp"

using namespace rcraft::viz;

OverlayView::OverlayView(QWidget *parent)
    : QWidget(parent)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen ? screen->size() : QSize(1920, 1080);

    int baseWidth = 1600;
    int baseHeight = 800;
    int scaledWidth = static_cast<int>(screenSize.width() * (static_cast<float>(baseWidth) / 1920));
    int scaledHeight = static_cast<int>(screenSize.height() * (static_cast<float>(baseHeight) / 1080));
    setMinimumSize(scaledWidth, scaledHeight);

    this->mapViewModel_ = std::make_unique<MapViewModel>(this);
    this->mapView_ = std::make_unique<MapView>(this->mapViewModel_.get());

    connect(this->mapViewModel_.get(), &MapViewModel::mapUpdated, this->mapView_.get(), &MapView::OnMapUpdated);
    connect(this->mapViewModel_.get(), &MapViewModel::resolutionChanged, this->mapView_.get(), &MapView::OnResolutionChanged);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->mapView_.get());
}

OverlayView::~OverlayView() = default;

void OverlayView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}