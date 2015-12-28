#include "PlayerScatterPlotChart.h"

PlayerScatterPlotChart::PlayerScatterPlotChart(QAbstractItemModel* model, QSortFilterProxyModel* proxyModel, QWidget* parent)
    : m_model(model)
    , m_proxyModel(proxyModel)
{
    QChart* myChart = new QChart();
    setChart(myChart);

    chart()->setDropShadowEnabled(false);

    Update();
}

void PlayerScatterPlotChart::resizeEvent(QResizeEvent* event)
{
    Update();
    QChartView::resizeEvent(event);
}

void PlayerScatterPlotChart::Update()
{
    const int MARKER_SIZE = 7;

    chart()->removeAllSeries();

    QScatterSeries* undraftedSeries = new QScatterSeries();
    undraftedSeries->setName("Undrafted");
    undraftedSeries->setMarkerSize(MARKER_SIZE);

    QScatterSeries* draftedSeries = new QScatterSeries();
    draftedSeries->setName("Drafted");
    draftedSeries->setMarkerSize(MARKER_SIZE);

    // Scale to widget width
    int maxSize = width() / (MARKER_SIZE+1);

    [&]
    {
        int rowCount = m_proxyModel->rowCount();
        for (auto row = 0; row < std::min(rowCount, maxSize); row++) {

            auto col = m_proxyModel->sortColumn();
            auto proxyIndex = m_proxyModel->index(row, col);
            auto value = m_proxyModel->data(proxyIndex, PlayerTableModel::RawDataRole).toFloat();

            auto sourceIndex = m_proxyModel->mapToSource(proxyIndex);
            auto ownerIndex = m_model->index(sourceIndex.row(), PlayerTableModel::COLUMN_OWNER, sourceIndex.parent());
            auto ownerId = m_model->data(ownerIndex, PlayerTableModel::RawDataRole).toInt();

            if (ownerId == 0) {
                undraftedSeries->append(row + 1, value);
            } else {
                draftedSeries->append(row + 1, value);
            }
        }
    }();

    QString sortColumnName = m_proxyModel->headerData(m_proxyModel->sortColumn(), Qt::Horizontal, Qt::DisplayRole).toString();
    chart()->setTitle("Data: " + sortColumnName);

    // connect(series0, &QScatterSeries::hovered, this, [=](const QPointF& point, bool state) {
    //     QToolTip::showText(point.toPoint(), QString("!!"));
    // });

    setRenderHint(QPainter::Antialiasing);
    chart()->addSeries(undraftedSeries);
    chart()->addSeries(draftedSeries);
    chart()->createDefaultAxes();
}

