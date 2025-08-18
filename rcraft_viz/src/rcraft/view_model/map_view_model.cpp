#include "rcraft/view_model/map_view_model.hpp"

using namespace rcraft::viz;

/**
 * @brief Construct a new MapViewModel and allocate the underlying MapModel.
 * @param parent Optional QObject parent.
 */
MapViewModel::MapViewModel(QObject *parent)
        : QObject(parent)
        , model_(std::make_unique<MapModel>())
{
}

/**
 * @brief Destroy the MapViewModel.
 */
MapViewModel::~MapViewModel() = default;

/**
 * @brief Get the currently selected map file path.
 * @return QString Current map path (may be empty if none selected).
 */
QString MapViewModel::getMapPath() const
{
    return this->mapPath_;
}

/**
 * @brief Set the current map file path (no I/O performed).
 * @param mapPath New map path to store.
 * @note This does not load or emit updates; see handleMapSelect() for loading.
 */
void MapViewModel::setMapPath(const QString &mapPath)
{
    this->mapPath_ = mapPath;
}

/**
 * @brief Open a file dialog to select a map image/YAML, load it into the model, and emit updates.
 *
 * @details
 * Workflow:
 *  - Show @c QFileDialog starting from a platform-specific default folder.
 *  - If the user cancels, log and return.
 *  - Ask @c MapModel to load the map from the chosen path.
 *  - On failure, log an error and return.
 *  - On success:
 *      - Update internal @ref mapPath_
 *      - Convert the loaded OpenCV image to @c QImage
 *      - Emit @ref resolutionChanged() and @ref mapUpdated()
 *
 * Supported filters (by dialog): @c *.pgm *.png *.bmp *.jpg *.yaml
 * Actual loading behavior is implemented inside @c MapModel::loadMapFromPath().
 *
 * @note This method is intended to be called from the GUI thread.
 */
void MapViewModel::handleMapSelect()
{
    QString startPath;
#ifdef Q_OS_WIN
    startPath = MAP_DEFAULT_WIN_PATH;
#else
    startPath = "~";
#endif

    const QString filter = "Map Files (*.pgm *.png *.bmp *.jpg *.yaml);;All Files (*)";
    const QString path = QFileDialog::getOpenFileName(
        nullptr,
        tr("Select Map Image or YAML"),
        startPath,
        filter
    );

    if (path.isEmpty())
    {
        qInfo() << "[INFO][VM] No map file selected";
        return;
    }
    qInfo() << "[INFO][VM] Selected Map Path:" << path;

    const bool success = this->model_->loadMapFromPath(path.toStdString());
    if (!success)
    {
        qWarning() << "[ERROR][VM] Failed to load map";
        return;
    }

    this->setMapPath(path);

    const QImage qImage = this->model_->copyMapImage2QImage();
    if (!qImage.isNull())
    {
        emit resolutionChanged(this->model_->getMapResolution());
        emit mapUpdated(qImage);
    }
}
