#ifndef SINGLE_DISPLAY_H
#define SINGLE_DISPLAY_H

#include "dashboard/dashboard_element.h"
#include "dashboard/elements_base/adjust_text_element.h"
#include "dashboard/layouts/layout_grid.h"
#include "dashboard/dashboard_parameter.h"

class QTBSingleDisplay : public QTBDashboardElement
{
    Q_OBJECT
public:
    enum DisplayOrientation {
        doVertical,
        doHorizontal
    };

    QTBSingleDisplay(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    virtual void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    virtual void buildLayout();
    virtual void clearElement() Q_DECL_OVERRIDE;
    virtual void edit() Q_DECL_OVERRIDE;
    virtual QString name() Q_DECL_OVERRIDE { return QString();}

    virtual void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;

    virtual void processNewSamples() Q_DECL_OVERRIDE;
    virtual void updateElement() Q_DECL_OVERRIDE;
    virtual void updateSizeConstraints();
    virtual void checkSizeAndVisibility();
    void update(UpdatePhase phase) Q_DECL_OVERRIDE;

    bool headerVisible() const;
    bool valueVisible() const;

    void setHeaderVisible(bool headerVisible);
    void setValueVisible(bool valueVisible);

protected:
    QTBAdjustTextElement *mTextLabel;
    QTBAdjustTextElement *mTextUnit;
    QTBAdjustTextElement *mTextValue;
    QCPLayoutElement *mEmptyElementBottom{};
    QCPLayoutElement *mEmptyElementTop{};

    QTBLayoutGrid *mMainLayout{};
    QTBLayoutGrid *mSubLayout{};

    int mSpacing;

    bool mHeaderVisible;
    bool mValueVisible;
};

#endif // SINGLE_DISPLAY_H
