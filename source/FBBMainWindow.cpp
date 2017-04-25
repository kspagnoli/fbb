#include "FBB/FBBMainWindow.h"
#include "FBB/FBBMainMenuBar.h"
#include "FBB/FBBDraftBoard.h"

#include <QGridLayout>
#include <QTabWidget>

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

    // Add each page...
    pTabWidget->addTab(new FBBDraftBoard(), "Draft Board");
}