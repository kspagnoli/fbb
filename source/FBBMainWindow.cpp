#include "FBB/FBBMainWindow.h"
#include "FBB/FBBMainMenuBar.h"
#include "FBB/FBBDraftBoard.h"

#include <QGridLayout>
#include <QTabWidget>
#include <QDockWidget>

FBBMainWindow::FBBMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    // Add menu bar
    FBBMainMenuBar* mainMenuBar = new FBBMainMenuBar(this);
    QMainWindow::setMenuBar(mainMenuBar);

    QDockWidget* pFirstWidget = nullptr;

    auto InstallWidget = [&](const QString& title,  QWidget* widget) 
    {
        QDockWidget* pDockWidget = new QDockWidget(title, this);
        pDockWidget->setWidget(widget);
        pDockWidget->setFeatures(QDockWidget::DockWidgetMovable);
        addDockWidget(Qt::LeftDockWidgetArea, pDockWidget);

        if (!pFirstWidget) {
            pFirstWidget = pDockWidget;
        } else {
            tabifyDockWidget(pFirstWidget, pDockWidget);
        }
    };

    InstallWidget("Hitting", new FBBDraftBoard(FBBPlayer::Projection::PROJECTION_TYPE_HITTING, this));
    InstallWidget("Pitching", new FBBDraftBoard(FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, this));

    /*
    // Create main widget
    QWidget* pCentralWidget = new QWidget(this);
    QGridLayout* pCentralLayout = new QGridLayout(pCentralWidget);
    QMainWindow::setCentralWidget(pCentralWidget);

    // Main tab widget
    QTabWidget* pTabWidget = new QTabWidget();
    pCentralLayout->addWidget(pTabWidget);

    // Add each page...
    pTabWidget->addTab(new FBBHittingDraftBoard(), "Hitters");
    */

    // Create main widget
    // QWidget* pCentralWidget = new QWidget(this);
    // QGridLayout* pCentralLayout = new QGridLayout(pCentralWidget);
    // QMainWindow::setCentralWidget(pCentralWidget);

    // Main tab widget
    //QTabWidget* pTabWidget = new QTabWidget();
    //pCentralLayout->addWidget(new FBBHittingDraftBoard(this));

    // addDockWidget(Qt::LeftDockWidgetArea, new QDockWidget("test_0", this));
    // addDockWidget(Qt::LeftDockWidgetArea, new QDockWidget("test_1", this));
    // addDockWidget(Qt::LeftDockWidgetArea, new QDockWidget("test_2", this));
    // addDockWidget(Qt::LeftDockWidgetArea, new QDockWidget("test_3", this));

    // Add each page...
    // pTabWidget->addTab, "Hitters");
}

