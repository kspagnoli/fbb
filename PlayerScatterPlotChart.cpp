#include "PlayerScatterPlotChart.h"

#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QValueAxis>

#include <QLayout>

////////////////////////////////////////////////////////////////////////////

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QMouseEvent>

class PlayerChartCallout : public QGraphicsItem
{
public:

    PlayerChartCallout(QGraphicsItem* parent = 0)
        : QGraphicsItem(parent)
    {
    }

    void setText(const QString &text)
    {
        m_text = text;
        QFontMetrics metrics(m_font);
        m_textRect = metrics.boundingRect(QRect(0, 0, 150, 150), Qt::AlignLeft, m_text);
        m_textRect.translate(5, 5);
        prepareGeometryChange();
        m_rect = m_textRect.adjusted(-5, -5, 5, 5);
    }

    void setAnchor(QPointF point)
    {
        m_anchor = point;
    }

    QRectF boundingRect() const
    {
        QPointF anchor = mapFromParent(m_anchor);
        QRectF rect;
        rect.setLeft(qMin(m_rect.left(), anchor.x()));
        rect.setRight(qMax(m_rect.right(), anchor.x()));
        rect.setTop(qMin(m_rect.top(), anchor.y()));
        rect.setBottom(qMax(m_rect.bottom(), anchor.y()));
        return rect;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        QPainterPath path;
        path.addRect(m_rect);

        QPointF anchor = mapFromParent(m_anchor);

        if (!m_rect.contains(anchor)) {

            QPointF point1, point2;

            // establish the position of the anchor point in relation to m_rect
            bool above = anchor.y() <= m_rect.top();
            bool aboveCenter = anchor.y() > m_rect.top() && anchor.y() <= m_rect.center().y();
            bool belowCenter = anchor.y() > m_rect.center().y() && anchor.y() <= m_rect.bottom();
            bool below = anchor.y() > m_rect.bottom();

            bool onLeft = anchor.x() <= m_rect.left();
            bool leftOfCenter = anchor.x() > m_rect.left() && anchor.x() <= m_rect.center().x();
            bool rightOfCenter = anchor.x() > m_rect.center().x() && anchor.x() <= m_rect.right();
            bool onRight = anchor.x() > m_rect.right();

            // get the nearest m_rect corner.
            qreal x = (onRight + rightOfCenter) * m_rect.width();
            qreal y = (below + belowCenter) * m_rect.height();
            bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
            bool vertical = qAbs(anchor.x() - x) > qAbs(anchor.y() - y);

            qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight * 20);
            qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20);;
            point1.setX(x1);
            point1.setY(y1);

            qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);;
            qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * (above * 20 - below * 10);;
            point2.setX(x2);
            point2.setY(y2);

            path.moveTo(point1);
            path.lineTo(mapFromParent(m_anchor));
            path.lineTo(point2);
            path = path.simplified();
        }

        painter->setPen(QColor(QRgb(0xffffff)));
        painter->setBrush(QColor(QRgb(0x2a82da)));
        painter->drawPath(path);
        painter->drawText(m_textRect, m_text);
    }

protected:

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event)
    {
        if (event->buttons() & Qt::LeftButton) {
            setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
            event->setAccepted(true);
        } else {
            event->setAccepted(false);
        }
    }

private:
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor;
    QFont m_font;
};

//////////////////////////////////////////////////////////////////////////

PlayerScatterPlotChart::PlayerScatterPlotChart(QAbstractItemModel* model, QSortFilterProxyModel* proxyModel, QWidget* parent)
    : m_model(model)
    , m_proxyModel(proxyModel)
    , m_yAxis(new QValueAxis(this))
    , m_xAxis(new QValueAxis(this))
    , m_undraftedSeries(new QScatterSeries(this))
    , m_draftedSeries(new QScatterSeries(this))
{
    // Create the main chart
    QChart* myChart = new QChart();
    setChart(myChart);

    // Chart configuration
    chart()->setDropShadowEnabled(false);
    chart()->legend()->setAlignment(Qt::AlignBottom);
    chart()->setBackgroundRoundness(0.f);
    chart()->setMargins(QMargins(5, 10, 10, 5));

    // View configuration
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);

    // Drafted player series
    m_undraftedSeries->setName("Undrafted");
    m_undraftedSeries->setMarkerSize(MARKER_SIZE);

    // Undrafted player series
    m_draftedSeries->setName("Drafted");
    m_draftedSeries->setMarkerSize(MARKER_SIZE);

    // Setup hovering
    connect(m_undraftedSeries, &QScatterSeries::hovered, this, &PlayerScatterPlotChart::HoverTooltip);
    connect(m_draftedSeries, &QScatterSeries::hovered, this, &PlayerScatterPlotChart::HoverTooltip);

    // Set initial values
    Update();
}

void PlayerScatterPlotChart::resizeEvent(QResizeEvent* event)
{
    Update();
    QChartView::resizeEvent(event);
}

void PlayerScatterPlotChart::Update()
{
    // Reset the series values
    m_undraftedSeries->clear();
    m_draftedSeries->clear();

    // Scale to widget width
    int maxSize = chart()->plotArea().width() / (MARKER_SIZE+1);
    
    // Calculate entries to show
    int rowCount = m_proxyModel->rowCount();
    auto entriesToShow = std::min(rowCount, maxSize);

    // Calculate number of tick marks to show
    auto xTicks = chart()->plotArea().width() / ((MARKER_SIZE + 1) * 4);
    auto yTicks = chart()->plotArea().height() / ((MARKER_SIZE + 1) * 4);

    // Value range
    float minValue = std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::min();

    // Value format
    QString valueFormat = m_proxyModel->headerData(m_proxyModel->sortColumn(), Qt::Horizontal, Qt::ToolTipRole).toString();

    [&]
    {
        for (auto row = 0; row < entriesToShow; row++) {

            auto col = m_proxyModel->sortColumn();
            auto proxyIndex = m_proxyModel->index(row, col);
            auto value = m_proxyModel->data(proxyIndex, PlayerTableModel::RawDataRole).toFloat();

            auto sourceIndex = m_proxyModel->mapToSource(proxyIndex);
            auto ownerIndex = m_model->index(sourceIndex.row(), PlayerTableModel::COLUMN_OWNER, sourceIndex.parent());
            auto ownerId = m_model->data(ownerIndex, PlayerTableModel::RawDataRole).toInt();

            if (ownerId == 0) {
                m_undraftedSeries->append(row+1, value);
            } else {
                m_draftedSeries->append(row+1, value);
            }

            minValue = std::min(minValue, value);
            maxValue = std::max(maxValue, value);
        }
    }();

    // yRange padding
    auto yRange = maxValue - minValue;
    auto yRangePadding = yRange * 0.07f;

    // Add to chart
    chart()->addSeries(m_undraftedSeries);
    chart()->addSeries(m_draftedSeries);

    // Configure x-axis
    m_xAxis->setTitleText("Rank");
    m_xAxis->setTickCount(xTicks);
    m_xAxis->setLabelFormat("%i");
    m_xAxis->setRange(0, entriesToShow+1);
    m_draftedSeries->attachAxis(m_xAxis);
    m_undraftedSeries->attachAxis(m_xAxis);
    chart()->addAxis(m_xAxis, Qt::AlignBottom);

    // Configure y-axis
    m_yAxis->setTitleText(CurrentSortName());
    m_yAxis->setTickCount(yTicks);
    m_yAxis->setLabelFormat(valueFormat);
    m_yAxis->setRange(minValue - yRangePadding, maxValue + yRangePadding);
    m_draftedSeries->attachAxis(m_yAxis);
    m_undraftedSeries->attachAxis(m_yAxis);
    chart()->addAxis(m_yAxis, Qt::AlignLeft);
}

void PlayerScatterPlotChart::HoverTooltip(QPointF point, bool state)
{
    if (m_tooltip == 0) {
        m_tooltip = new PlayerChartCallout(chart());
    }

    if (state) {
    
        // look up name
        auto proxyIndex = m_proxyModel->index(point.x() - 1, 0);
        auto sourceIndex = m_proxyModel->mapToSource(proxyIndex);
        auto nameIndex = m_model->index(sourceIndex.row(), PlayerTableModel::COLUMN_NAME, sourceIndex.parent());
        auto name = m_model->data(nameIndex, PlayerTableModel::RawDataRole).toString();

        m_tooltip->setText(QString("Player: %1\n"
                                   "%2: %3").arg(name).arg(CurrentSortName()).arg(point.y()));
        QXYSeries* series = qobject_cast<QXYSeries*>(sender());
        m_tooltip->setAnchor(chart()->mapToPosition(point, series));
        m_tooltip->setPos(chart()->mapToPosition(point, series) + QPoint(10, -50));
        m_tooltip->setZValue(11);
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

QString PlayerScatterPlotChart::CurrentSortName() const
{
    return m_proxyModel->headerData(m_proxyModel->sortColumn(), Qt::Horizontal, Qt::DisplayRole).toString();
}
