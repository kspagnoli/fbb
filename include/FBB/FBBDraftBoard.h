#pragma once

#include "FBB/FBBPlayer.h"

#include <QWidget>

class FBBDraftBoard : public QWidget
{
public:
    FBBDraftBoard(FBBPlayer::Projection::TypeMask typeMask, QWidget* parent = nullptr);
};