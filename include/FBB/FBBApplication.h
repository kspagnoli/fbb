#pragma once

#include <QApplication>
#include <QString>

class FBBDraftBoardModel;
class FBBLeaugeSettings;

class QFile;

class FBBApplication : public QApplication
{
    Q_OBJECT

public:
    FBBApplication(int& argc, char** argv);


    // 
    FBBLeaugeSettings* Settings() const { return m_pSettings; } 
    FBBDraftBoardModel* DraftBoardModel() const { return m_pDraftBoardModel; } 

    // I/O
    void Exit();
    void Save();
    void SaveAs();
    void Load(const QString& file);

    // Singleton
    static FBBApplication* Instance() { return s_instance; }
    
signals:
    void PathChanged();

private:
    static FBBApplication* s_instance;
    FBBLeaugeSettings* m_pSettings = nullptr;
    FBBDraftBoardModel* m_pDraftBoardModel = nullptr;

    QString m_file;
 };

#define fbbApp (static_cast<FBBApplication*>(FBBApplication::Instance()))