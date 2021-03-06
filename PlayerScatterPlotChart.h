#pragma once

#include <QtCharts/QChartView>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include "PlayerTableModel.h"

// Forwards
QT_CHARTS_BEGIN_NAMESPACE
class QScatterSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class PlayerChartCallout;

class PlayerScatterPlotChart : public QChartView
{
    Q_OBJECT

public:

    PlayerScatterPlotChart(QAbstractItemModel* model, QSortFilterProxyModel* proxyModel, QWidget* parent = nullptr);

    virtual void resizeEvent(QResizeEvent* event) override;

    void SetProxyModel(QSortFilterProxyModel* proxyModel);
    void Update();

signals:

    void PlayerClicked(const QModelIndex& index);

private:

    static const int MARKER_SIZE = 8;

    void HoverTooltip(QPointF point, bool state);
    void OnReleased(QPointF point);
    QString CurrentSortName() const;

    QAbstractItemModel* m_model = nullptr;
    QSortFilterProxyModel* m_proxyModel = nullptr;
    PlayerChartCallout* m_tooltip = nullptr;
    QValueAxis* m_yAxis = nullptr;
    QValueAxis* m_xAxis = nullptr;
    QScatterSeries* m_undraftedSeries = nullptr;
    QScatterSeries* m_draftedSeries = nullptr;
};

