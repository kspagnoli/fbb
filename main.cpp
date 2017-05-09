#include "FBB/FBBApplication.h"
#include "FBB/FBBMainWindow.h"

#include <QSplashScreen>

int main(int argc, char *argv[])
{
    // Application
    FBBApplication app(argc, argv);

    // 
    QPixmap pixmap(":/icons/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    // Main window
    FBBMainWindow mainWin;
    mainWin.resize(1000, 800);
    mainWin.show();
    splash.finish(&mainWin);

    return app.exec();
}

