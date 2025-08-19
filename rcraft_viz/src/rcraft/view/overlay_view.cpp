#include "rcraft/view/overlay_view.hpp"

using namespace rcraft::viz;

/**
 * @file
 * @brief Implementation of OverlayView, a top-level container that hosts MapView and wires it to MapViewModel.
 */

/**
 * @brief Construct an OverlayView and set up screen sizing, signal wiring, and layout.
 *
 * @param parent Optional Qt parent widget.
 *
 * @details
 * - Allocates a @c MapViewModel (owned via std::unique_ptr, parented to this for Qt ownership).
 * - Allocates a @c MapView bound to the view-model.
 * - Applies initial size hints based on the primary screen via setScreen().
 * - Connects model→view signals via setQConnected().
 * - Installs a simple @c QVBoxLayout and adds the view via setDefaultLayout().
 */
OverlayView::OverlayView(QWidget *parent)
    : QWidget(parent)
    , mapViewModel_(std::make_unique<MapViewModel>(this))
    , mapView_(std::make_unique<MapView>(this->mapViewModel_.get()))
{
    this->setScreen();
    this->setQConnected();
    this->setDefaultLayout();
}

/**
 * @brief Destructor (default).
 *
 * @details
 * The unique_ptr members clean up automatically. Qt parent–child ownership
 * handles any QObject-based sub-objects.
 */
OverlayView::~OverlayView() = default;

/**
 * @brief Apply initial sizing based on the primary screen.
 *
 * @details
 * Uses the primary screen size (or 1920x1080 fallback) and scales a base size
 * of 1600x800 proportionally to the current resolution, then calls setMinimumSize().
 *
 * Scale formula:
 * - width  = screenWidth  * (1600 / 1920)
 * - height = screenHeight * (800 / 1080)
 */
void OverlayView::setScreen()
{
    const QScreen *screen = QGuiApplication::primaryScreen();
    const QSize screenSize = screen ? screen->size() : QSize(1920, 1080);

    constexpr int baseWidth = 1600;
    constexpr int baseHeight = 800;
    const int &scaledWidth = static_cast<int>(screenSize.width() * (static_cast<float>(baseWidth) / 1920));
    const int &scaledHeight = static_cast<int>(screenSize.height() * (static_cast<float>(baseHeight) / 1080));
    setMinimumSize(scaledWidth, scaledHeight);
}

/**
 * @brief Wire up signals between the view-model and the view.
 *
 * @details
 * - MapViewModel::mapUpdated      → MapView::onMapUpdated
 * - MapViewModel::resolutionChanged → MapView::onResolutionChanged
 *
 * These are direct connections (same thread), suitable for GUI usage.
 */
void OverlayView::setQConnected()
{
    connect(this->mapViewModel_.get(), &MapViewModel::mapUpdated, this->mapView_.get(), &MapView::onMapUpdated);
    connect(this->mapViewModel_.get(), &MapViewModel::resolutionChanged, this->mapView_.get(), &MapView::onResolutionChanged);
}

/**
 * @brief Install a simple vertical box layout and insert the map view.
 *
 * @details
 * The layout owns the inserted widget. Margins are set to zero so the MapView
 * fills the available space.
 */
void OverlayView::setDefaultLayout()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->mapView_.get());
}

/**
 * @brief Handle widget resize events.
 *
 * @param event Qt resize event.
 *
 * @details
 * Currently just forwards to the base class. The embedded MapView itself
 * handles fit-to-scene behavior on its own resizeEvent.
 */
void OverlayView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}
