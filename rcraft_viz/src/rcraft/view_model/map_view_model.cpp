#include "rcraft/view_model/map_view_model.hpp"

using namespace rcraft::viz;

MapViewModel::MapViewModel(QObject *parent)
        : QObject(parent)
        , model_(std::make_unique<MapModel>())
{
}

MapViewModel::~MapViewModel() = default;

void MapViewModel::handleMapSelect()
{
    QString startPath;
#ifdef Q_OS_WIN
    startPath = MAP_DEFAULT_WIN_PATH;
#else
    startPath = "~";
#endif

    QString filter = "Map Files (*.pgm *.png *.bmp *.jpg *.yaml);;All Files (*)";
    QString path = QFileDialog::getOpenFileName(
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

    bool success = this->model_->loadMapFromPath(path.toStdString());
    if (!success)
    {
        qWarning() << "[ERROR][VM] Failed to load map";
        return;
    }

    this->mapPath_ = path;
    QImage qImage = this->model_->getQImage();

    if (!qImage.isNull())
    {
        emit resolutionChanged(this->model_->mapResolution());
        emit mapUpdated(qImage);
    }
}

