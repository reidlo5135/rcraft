#ifndef RCRAFT_MAP_VIEW_MODEL_HPP
#define RCRAFT_MAP_VIEW_MODEL_HPP

#pragma once

#include <memory>
#include <QString>
#include <QObject>
#include <QImage>
#include <QFileDialog>
#include <QApplication>
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "rcraft/model/map_model.hpp"

#define MAP_DEFAULT_WIN_PATH "C:/mn_ws/rcraft/rcraft_map_server/maps"

namespace rcraft::viz
{
    class MapViewModel : public QObject
    {
        Q_OBJECT

    private:
        MapModel::UniquePtr model_;
        QString mapPath_;

    public:
        explicit MapViewModel(QObject *parent = nullptr);
        virtual ~MapViewModel() override;
        QString mapPath() const { return this->mapPath_; }
        void handleMapSelect();

    signals:
        void mapUpdated(const QImage&);
        void resolutionChanged(float);

    public:
        using SharedPtr = std::shared_ptr<MapViewModel>;
        using UniquePtr = std::unique_ptr<MapViewModel>;

    };
}

#endif // RCRAFT_MAP_VIEW_MODEL_HPP
