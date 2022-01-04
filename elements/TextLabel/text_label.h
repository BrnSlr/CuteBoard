#ifndef TEXT_LABEL_H
#define TEXT_LABEL_H

#include "ui/board/board_element.h"
#include "ui/board/utils/adaptive_text.h"
#include "ui/board/utils/layout_grid.h"
#include "text_label_shared.h"

class TEXTLABEL_EXPORT TextLabel : public BoardElement
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    TextLabel(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    virtual void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    virtual void resetElement() Q_DECL_OVERRIDE;
    virtual void buildElement() Q_DECL_OVERRIDE;

    virtual void clearElement() Q_DECL_OVERRIDE;
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    virtual void edit() Q_DECL_OVERRIDE;

    void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;
    void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;

    QString text() const;
    void setText(const QString &text);

    ColorSettings &colorSettingsRef();

protected:
    QString mText;
    AdaptiveTextElement *mTextElement;
    ColorSettings mColorSettings;
    LayoutGrid *mMainLayout{};

};

#endif // ALARM_PANEL_H
