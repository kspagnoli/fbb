#include "FBB/FBBApplication.h"
#include "FBB/FBBMainWindow.h"

int main(int argc, char *argv[])
{
    // Application
    FBBApplication app(argc, argv);

    // Main window
    FBBMainWindow mainWin;
    mainWin.resize(1000, 800);
    mainWin.show();

    return app.exec();
}

