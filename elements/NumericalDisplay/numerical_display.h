#ifndef NUMERICALDISPLAY_H
#define NUMERICALDISPLAY_H

#include "ui/board/utils/single_display.h"
#include "numerical_display_shared.h"

class NUMERICAL_DISPLAY_EXPORT NumericalDisplay : public SingleDisplay
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum DisplayOrientation {
        doVerticalAlignCenter,
        doVerticalAlignLeftRight
    };

    enum StatMode {
        smNone,
        smMin,
        smMax
    };

    NumericalDisplay(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    virtual void resetElement() Q_DECL_OVERRIDE;
    virtual void buildElement() Q_DECL_OVERRIDE;
    virtual void edit() Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;

    virtual void loadData() Q_DECL_OVERRIDE;
    virtual void displayData() Q_DECL_OVERRIDE;
    virtual void applySizeConstraints() Q_DECL_OVERRIDE;

    ValueStringFormat valueFormat() const;
    DisplayOrientation orientation() const;
    StatMode statMode() const;

    void setValueFormat(const ValueStringFormat &valueFormat);
    void setOrientation(const DisplayOrientation &orientation);
    void setStatMode(const StatMode &statMode);

protected:
    DisplayOrientation mOrientation;
    ValueStringFormat mValueFormat;
    StatMode mStatMode;
    LayoutGrid *mUnitLayout{};
    AdaptiveTextElement *mTextMode;

    double mStatMinValue;
    double mStatMaxValue;
};


#endif // NUMERICALDISPLAY_H
