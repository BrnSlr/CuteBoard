#ifndef VALUESINGLEDISPLAY_H
#define VALUESINGLEDISPLAY_H

#include "dashboard/elements_base/single_display.h"
#include "dashboard/elements_factory/elementfactory.h"

#define VALUEDISPLAY_NAME "Value - Display"

class QTBValueDisplay : public QTBSingleDisplay
{
    Q_OBJECT
public:
    enum DisplayOrientation {
        doVerticalAlignCenter,
        doVerticalAlignLeftRight
    };

    QTBValueDisplay(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual void buildLayout() Q_DECL_OVERRIDE;
    virtual void edit() Q_DECL_OVERRIDE;
    virtual QString name() Q_DECL_OVERRIDE { return QString(VALUEDISPLAY_NAME);}

    virtual void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;

    virtual void processNewSamples() Q_DECL_OVERRIDE;
    virtual void updateSizeConstraints() Q_DECL_OVERRIDE;
    virtual void checkSizeAndVisibility() Q_DECL_OVERRIDE;

    ValueStringFormat valueFormat() const;
    DisplayOrientation orientation() const;

    void setValueFormat(const ValueStringFormat &valueFormat);
    void setOrientation(const DisplayOrientation &orientation);
protected:
    DisplayOrientation mOrientation;
    ValueStringFormat mValueFormat;
};

static ElementRegister<QTBValueDisplay> valueDisplayRegister(QString(VALUEDISPLAY_NAME), QTBDashboardElement::etSingleParam, ":/elements/icons8_variable_50px.png");

#endif // VALUESINGLEDISPLAY_H
