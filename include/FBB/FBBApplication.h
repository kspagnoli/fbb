#pragma once

#include <QApplication>
#include <QString>

#include <memory>

class FBBDraftBoardModel;
class QFile;

class FBBApplication : public QApplication
{
    Q_OBJECT

public:
    FBBApplication(int& argc, char** argv);
    FBBDraftBoardModel* DraftBoardModel() const { return m_pDraftBoardModel; } 
    static FBBApplication* Instance() { return s_instance; }

    void Exit();
    void Save();
    void SaveAs();
    void Load(const QString& file);

signals:
    void PathChanged();

private:
    static FBBApplication* s_instance;
    FBBDraftBoardModel* m_pDraftBoardModel = nullptr;

    QString m_file;
 };

#define fbbApp (static_cast<FBBApplication*>(FBBApplication::Instance()))