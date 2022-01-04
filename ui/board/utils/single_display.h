#ifndef SINGLE_DISPLAY_H
#define SINGLE_DISPLAY_H

#include "ui/board/board_element.h"
#include "ui/board/utils/adaptive_text.h"
#include "ui/board/utils/layout_grid.h"
#include "ui/board/board_parameter.h"

#define LABEL_HEIGHT_RATIO 0.3
#define UNIT_HEIGHT_RATIO 0.25
#define VALUE_HEIGHT_RATIO 0.45

class BOARD_UI_EXPORT SingleDisplay : public BoardElement
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    SingleDisplay(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    virtual void applySizeConstraints();

    virtual void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    virtual void resetElement() Q_DECL_OVERRIDE;
    virtual void buildElement() Q_DECL_OVERRIDE;
    virtual void clearElement() Q_DECL_OVERRIDE;

    virtual void edit() Q_DECL_OVERRIDE;
    virtual void checkParameters() Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;

    virtual void loadData() Q_DECL_OVERRIDE;
    virtual void displayData() Q_DECL_OVERRIDE;
    void update(UpdatePhase phase) Q_DECL_OVERRIDE;

    bool headerVisible() const;
    bool valueVisible() const;

    void setHeaderVisible(bool headerVisible);
    void setValueVisible(bool valueVisible);

protected:
    AdaptiveTextElement *mTextLabel;
    AdaptiveTextElement *mTextUnit;
    AdaptiveTextElement *mTextValue;
    QCPLayoutElement *mEmptyElementBottom{};
    QCPLayoutElement *mEmptyElementTop{};

    LayoutGrid *mMainLayout{};
    LayoutGrid *mSubLayout{};

    int mSpacing;

    bool mHeaderVisible;
    bool mValueVisible;
};

#endif // SINGLE_DISPLAY_H
