#include "FBB/FBBApplication.h"

#include <QStyleFactory>

FBBApplication::FBBApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Dark theme!
    // QPalette darkPalette;
    // darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    // darkPalette.setColor(QPalette::WindowText, Qt::white);
    // darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    // darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    // darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    // darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    // darkPalette.setColor(QPalette::Text, Qt::white);
    // darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    // darkPalette.setColor(QPalette::ButtonText, Qt::white);
    // darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    // darkPalette.setColor(QPalette::BrightText, Qt::red);
    // darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    // darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    // darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    // app.setPalette(darkPalette);
    // QToolTip {
    // color: #ffffff;
    //     background - color: #2a82da;
    // border: 1px solid white;
    // }

    setStyleSheet(R"""(
        QLabel, QMenu, QTableView, QHeaderView, QToolTip, QPlainTextEdit {
            font-family: "Consolas";
            font-size: 11px;
        })""");
}

