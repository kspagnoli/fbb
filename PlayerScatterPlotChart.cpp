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

class Callout : public QGraphicsItem
{
public:

    Callout(QGraphicsItem* parent = 0)
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
        path.addRoundedRect(m_rect, 5, 5);

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

        painter->setBrush(QColor(QRgb(0x2a82da)));
        painter->drawPath(path);
        painter->drawText(m_textRect, m_text);
    }

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        event->setAccepted(true);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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
{
    QChart* myChart = new QChart();
    setChart(myChart);

    chart()->setDropShadowEnabled(false);
    chart()->setAnimationOptions(QChart::AllAnimations);
    chart()->legend()->setAlignment(Qt::AlignBottom);
    chart()->setBackgroundRoundness(0.f);

    setContentsMargins(0, 0, 0, 0);
    setMouseTracking(true);

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

    setRenderHint(QPainter::Antialiasing);
    chart()->addSeries(undraftedSeries);
    chart()->addSeries(draftedSeries);
    chart()->createDefaultAxes();

    chart()->axisX()->setTitleText("Rank");
    chart()->axisY()->setTitleText(CurrentSortName());

    connect(undraftedSeries, &QScatterSeries::hovered, this, &PlayerScatterPlotChart::HoverTooltip);
}

void PlayerScatterPlotChart::HoverTooltip(QPointF point, bool state)
{
    if (m_tooltip == 0) {
        m_tooltip = new Callout(chart());
    }

    if (state) {
    
        // look up name
        auto proxyIndex = m_proxyModel->index(point.x() - 1, 0);
        auto sourceIndex = m_proxyModel->mapToSource(proxyIndex);
        auto nameIndex = m_model->index(sourceIndex.row(), PlayerTableModel::COLUMN_NAME, sourceIndex.parent());
        auto name = m_model->data(nameIndex, PlayerTableModel::RawDataRole).toString();

        m_tooltip->setText(QString("Player: %1\n"
                                   "%2: %3").arg(name).arg(CurrentSortName()).arg(point.y()));
        QXYSeries *series = qobject_cast<QXYSeries *>(sender());
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
