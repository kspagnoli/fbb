#pragma once

#include <QtCharts/QChartView>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include "PlayerTableModel.h"

QT_CHARTS_USE_NAMESPACE

class Callout;

class PlayerScatterPlotChart : public QChartView
{

public:

    PlayerScatterPlotChart(QAbstractItemModel* model, QSortFilterProxyModel* proxyModel, QWidget* parent = nullptr);

    virtual void resizeEvent(QResizeEvent* event) override;

    void Update();

private:

    void HoverTooltip(QPointF point, bool state);
    QString CurrentSortName() const;

    QAbstractItemModel* m_model = nullptr;
    QSortFilterProxyModel* m_proxyModel = nullptr;
    Callout* m_tooltip = nullptr;
};

