#pragma once

#include <QApplication>

#include <memory>

class FBBDraftBoardModel;

class FBBApplication : public QApplication
{
public:
    FBBApplication(int& argc, char** argv);
    FBBDraftBoardModel* DraftBoardModel() const { return m_pDraftBoardModel; } 

private:
    FBBDraftBoardModel* m_pDraftBoardModel;
};

static FBBApplication* s_app = nullptr;
#define fbbApp s_app;