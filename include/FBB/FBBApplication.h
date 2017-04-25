#pragma once

#include <QApplication>

#include <memory>

class FBBApplication : public QApplication
{
public:
    FBBApplication(int& argc, char** argv);
};