#include "FBB/FBBApplication.h"
#include "FBB/FBBDraftBoardModel.h"

#include <QStyleFactory>
#include <QIcon>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>

static const char* k_AppName = "Fantasy Baseball";

FBBApplication* FBBApplication::s_instance = nullptr;

FBBApplication::FBBApplication(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_pDraftBoardModel(new FBBDraftBoardModel(this))
{
    s_instance = this;

    Q_INIT_RESOURCE(Resources);

    setStyle(QStyleFactory::create("Fusion"));

    setStyleSheet(R"""(
        QTableView, QHeaderView {
            font-family: "Consolas";
            font-size: 11px;
        }

        QDockWidget::title {
            border-top: 1px solid grey;
            border-bottom: 1px solid grey;
            font-size: 14px;
            text-align: left;
            padding-left: 5px;
        }

        )""");

    setWindowIcon(QIcon(":/icons/baseball.png"));

    auto SetApplicationName = [=]() {
        if (m_file.isEmpty()) { 
            setApplicationName(k_AppName);
        } else {
            const QFileInfo fileInfo(m_file);
            const QString name = QString("%1 - %2").arg(fileInfo.baseName()).arg(k_AppName);
            setApplicationName(name);
        }
    };
    
    connect(this, &FBBApplication::PathChanged, [=](){
        SetApplicationName();
    });

    SetApplicationName();
}

void FBBApplication::Exit()
{
    exit();
}

void FBBApplication::Save()
{
    QJsonObject json = m_pDraftBoardModel->ToJson();
    QJsonDocument doc(json);

    QFile jsonFile(m_file);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(doc.toJson(QJsonDocument::Indented));
}

void FBBApplication::SaveAs()
{
    const QString docFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_file = QFileDialog::getSaveFileName(nullptr, tr("Open File"), docFolder, tr("JSON Files (*.json)"));
    Save();
    emit PathChanged();
}

void FBBApplication::Load(const QString& fileName)
{
}

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

