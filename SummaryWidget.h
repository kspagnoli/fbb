#pragma once

#include <QWidget>
#include "SummaryTableModel.h"

//----------------------------------------------------------------------
// SummaryWidget
//----------------------------------------------------------------------

class SummaryWidget : public QWidget
{

public:

    SummaryWidget(PlayerTableModel* playerTableModel, const  std::vector<OwnerSortFilterProxyModel*>& vecOwnerSortFilterProxyModel, QWidget* parent)
        : QWidget(parent)
        , m_sumTableView(new QTableView)
    {

        // Source and filter models
        m_summaryTableModel = new SummaryTableModel(vecOwnerSortFilterProxyModel, playerTableModel, parent);
        SummarySortFitlerProxyModel* summarySortFitlerProxyModel = new SummarySortFitlerProxyModel(m_summaryTableModel);

        // Sum table view
        m_sumTableView->setModel(summarySortFitlerProxyModel);
        m_sumTableView->verticalHeader()->hide();
        m_sumTableView->setAlternatingRowColors(true);
        m_sumTableView->verticalHeader()->setDefaultSectionSize(15);
        m_sumTableView->setSortingEnabled(true);
        m_sumTableView->sortByColumn(SummaryTableModel::TEAM);
        m_sumTableView->setMinimumSize(100, 100);
        m_sumTableView->setMaximumSize(4096, 4096);
        m_sumTableView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        m_sumTableView->resizeColumnsToContents();

        // Use progress bars for each of the rankable stats
        ProgressBarDelegate* progressBarDelegate = new ProgressBarDelegate(playerTableModel, this);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::BA, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::R, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::HR, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::RBI, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::SB, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::W, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::WHIP, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::ERA, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::S, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::K, progressBarDelegate);
        m_sumTableView->setItemDelegateForColumn(SummaryTableModel::SUM, progressBarDelegate);

        for (int i = 0; i < m_sumTableView->horizontalHeader()->count(); ++i) {
            m_sumTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }

        QHBoxLayout* hBoxLayout = new QHBoxLayout(this);
        hBoxLayout->addWidget(m_sumTableView, 1);
        setLayout(hBoxLayout);
    }

public slots:

    void OnDraftedEnd()
    {
        // Forward to table
        m_summaryTableModel->OnDraftedEnd();
    }

private:

    class SummarySortFitlerProxyModel : public QSortFilterProxyModel
    {
    public:

        SummarySortFitlerProxyModel(QAbstractItemModel* sourceModel)
        {
            QSortFilterProxyModel::setSourceModel(sourceModel);
        }

        virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
        {
            return sourceModel()->data(left, SummaryTableModel::RankRole).toInt() < sourceModel()->data(right, SummaryTableModel::RankRole).toInt();
        }

        virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override
        {
            return true;
        }

        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
        {
            return true;
        }
    };

    class ProgressBarDelegate : public QStyledItemDelegate
    {
    public:
        ProgressBarDelegate(PlayerTableModel* playerTableModel, QWidget* parent)
            : QStyledItemDelegate(parent)
            , m_playerTableModel(playerTableModel)
        {
        }

        void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
        {
            int progress = index.data().toInt();
            QStyleOptionProgressBar progressBarOption;
            progressBarOption.rect = option.rect;
            progressBarOption.minimum = 0;
            progressBarOption.maximum = DraftSettings::OwnerCount();
            progressBarOption.progress = index.model()->data(index, SummaryTableModel::RankRole).toInt();
            progressBarOption.text = index.model()->data(index, Qt::DisplayRole).toString();
            progressBarOption.textVisible = true;
            QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
        }

    private:
        PlayerTableModel* m_playerTableModel;
    };
    
    SummaryTableModel* m_summaryTableModel;
    QTableView* m_sumTableView;
};