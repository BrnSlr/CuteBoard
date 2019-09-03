#ifndef BINARYDISPLAY_H
#define BINARYDISPLAY_H

#include "value_display.h"
#include "dashboard/elements_base/axisrect.h"

#define BINTEXT_NAME "Value - Bitfields"

class QTBValueBitfields: public QTBValueDisplay
{
    Q_OBJECT
public:

    QTBValueBitfields(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    virtual void buildLayout() Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;
    QString name() Q_DECL_OVERRIDE { return QString(BINTEXT_NAME);}

    void addDashboardParameter(QSharedPointer<QTBDashboardParameter> dashParameter) Q_DECL_OVERRIDE;

    void saveParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull) Q_DECL_OVERRIDE;
    void loadParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull) Q_DECL_OVERRIDE;

    void processNewSamples() Q_DECL_OVERRIDE;
    void processHistoricalSamples() Q_DECL_OVERRIDE;
    void updateElement() Q_DECL_OVERRIDE;

    void updateSizeConstraints() Q_DECL_OVERRIDE;
    void checkSizeAndVisibility() Q_DECL_OVERRIDE;

protected:
    QTBLayoutGrid *mLayout;
    QTBLayoutGrid *mBitsLayout;
    QTBAxisRect *mAxisRect;
    QList<QTBAdjustTextElement*> mBitLabels;
    QList<QCPGraph*> mGraphs;

    int mBitsSize;
};

static ElementRegister<QTBValueBitfields> bitfieldsRegister(QString(BINTEXT_NAME), ":/elements/icons8_matrix_50px.png");

#endif // BINARYDISPLAY_H
