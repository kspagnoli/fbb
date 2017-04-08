#include "FBB/FBBMainWindow.h"
#include "FBB/FBBMainMenuBar.h"

FBBMainWindow::FBBMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    // Add menu bar
    FBBMainMenuBar* mainMenuBar = new FBBMainMenuBar(this);
    QMainWindow::setMenuBar(mainMenuBar);
}