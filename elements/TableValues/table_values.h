#ifndef TABLEVALUES_H
#define TABLEVALUES_H

#include "ui/board/board_element.h"
#include "ui/board/utils/adaptive_text.h"
#include "ui/board/utils/layout_grid.h"
#include "table_values_shared.h"

class TABLEVALUES_EXPORT TableValues : public BoardElement
{
    Q_OBJECT

public:
    TableValues(Board *dashboard = nullptr);
    ~TableValues() Q_DECL_OVERRIDE;

    // reimplemented virtual methods:
    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    virtual void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    void resetElement() Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void buildElement() Q_DECL_OVERRIDE;
    virtual void edit() Q_DECL_OVERRIDE;

    virtual void applySizeConstraints();

    virtual void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;    

    void loadConfigurations(QSettings *settings) Q_DECL_OVERRIDE;
    void checkParameters() Q_DECL_OVERRIDE;

    QSharedPointer<BoardParameter> addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings) Q_DECL_OVERRIDE;
    QSharedPointer<BoardParameter> addParameter(QString paramLabel) Q_DECL_OVERRIDE;
    void addParameter(QSharedPointer<BoardParameter> boardParameter) Q_DECL_OVERRIDE;
    void removeBoardParameter(int index, bool update = true) Q_DECL_OVERRIDE;

    void displayData() Q_DECL_OVERRIDE;

    bool alternatingRowColors() const;
    bool unitVisible() const;
    ValueStringFormat valueFormat() const;
    int precision() const;
    QString title() const;
    bool titleVisible() const;

    void setAlternatingRowColors(bool alternatingRowColors);
    void setUnitVisible(bool unitVisible);
    void setValueFormat(const ValueStringFormat &valueFormat);
    void setPrecision(int precision);
    void setTitle(const QString &title);
    void setTitleVisible(bool titleVisible);

protected:
    QTableWidget *mTableWidget;
    QMargins mTableMargins;

    bool mAlternatingRowColors;
    bool mUnitVisible;
    bool mTitleVisible;
    QString mTitle;
    ValueStringFormat mValueFormat;
    int mPrecision;

    LayoutGrid *mMainLayout{};
    AdaptiveTextElement *mTextLabel;
    QCPLayoutElement *mEmptyElementBottom{};
};

#endif // TABLEVALUES_H
