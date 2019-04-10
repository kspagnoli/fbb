#include "FBB/FBBMainWindow.h"
#include "FBB/FBBMainMenuBar.h"
#include "FBB/FBBDraftBoard.h"
#include "FBB/FBBDraftLog.h"
#include "FBB/FBBDraftBoardModel.h"

#include <QGridLayout>
#include <QTabWidget>
#include <QDockWidget>
#include <QTableView>
#include <QFile>

FBBMainWindow::FBBMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    // Add menu bar
    FBBMainMenuBar* mainMenuBar = new FBBMainMenuBar(this);
    QMainWindow::setMenuBar(mainMenuBar);

    // Create main widget
    QWidget* pCentralWidget = new QWidget(this);
    QGridLayout* pCentralLayout = new QGridLayout(pCentralWidget);
    QMainWindow::setCentralWidget(pCentralWidget);

    // Main tab widget
    QTabWidget* pTabWidget = new QTabWidget();
    pCentralLayout->addWidget(pTabWidget);

    // Add tabs
    pTabWidget->addTab(new FBBDraftBoard(this), "Draft");
    pTabWidget->addTab(new FBBDraftLog(this), "Log");
}

