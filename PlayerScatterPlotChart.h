#pragma once

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegendMarker>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include "PlayerTableModel.h"

QT_CHARTS_USE_NAMESPACE

class PlayerScatterPlotChart : public QChartView
{

public:

    PlayerScatterPlotChart(QAbstractItemModel* model, QSortFilterProxyModel* proxyModel, QWidget* parent = nullptr);

    virtual void resizeEvent(QResizeEvent* event) override;

    void Update();

private:

    QAbstractItemModel* m_model;
    QSortFilterProxyModel* m_proxyModel;
};

