#pragma once

#include <cstdint>
#include <QStringList>
#include <QDialog>

struct DraftSettings
{
    uint32_t Budget;
    uint32_t HitterCount;
    uint32_t PitcherCount;
    uint32_t RosterSize;
    float HittingSplit;
    float PitchingSplit;
    static const uint32_t OwnerCount = 12;
    QStringList OwnerNames;
    QStringList OwnerAbbreviations;

    static DraftSettings& Get();
};

class DraftSettingsDialog : public QDialog
{
public:
    DraftSettingsDialog();
};