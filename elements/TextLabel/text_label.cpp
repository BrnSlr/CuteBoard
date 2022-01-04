#include "text_label.h"
#include "ui/board/management/project.h"
#include "text_label_editor.h"

TextLabel::TextLabel(Board *dashboard) :
    BoardElement (dashboard),
    mText("Empty text")
{
    setType(BoardElement::etOther);
    setParametersMaxCount(0);
}

int TextLabel::defaultWidth()
{
    return 4*mBoard->dashboardLayout()->singleElementColumnCount();
}

int TextLabel::defaultHeight()
{
    return 1*mBoard->dashboardLayout()->singleElementRowCount();
}

void TextLabel::initialize(Board *dashboard)
{
    if(dashboard) {

        BoardElement::initialize(dashboard);

        auto textColor = QApplication::palette().toolTipText().color();
        mTextElement = new AdaptiveTextElement(dashboard);
        mTextElement->setTextFlags(Qt::AlignCenter);
        mTextElement->setMaxPointSize(16);
        mTextElement->setMinPointSize(1);
        mTextElement->setTextColor(textColor);
        mTextElement->setText(mText);
        mTextElement->setAdjustStrategy(AdaptiveTextElement::asAdjustPointSize);
        mTextElement->setCachedPixmap(true);

        mMainLayout = new LayoutGrid();
        mMainLayout->initializeParentPlot(dashboard);
        mMainLayout->setLayer(QLatin1String("main"));
        mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
        mMainLayout->setRowSpacing(0);
        mMainLayout->setMargins(QMarginsF(0,0,0,0));
        mMainLayout->setMinimumMargins(QMarginsF(0,0,0,0));
    }
}

void TextLabel::resetElement()
{    
    BoardElement::resetElement();

    for(int i=0; i<mMainLayout->elementCount(); i++)
        mMainLayout->takeAt(i);
    mMainLayout->simplify();
}

void TextLabel::buildElement()
{
    mTextElement->setText(mText);
    mTextElement->setBackgroundBrush(mColorSettings.backgroundBrush());
    mTextElement->setTextColor(mColorSettings.foregroundColor());
    mTextElement->needUpdate(true);

    mMainLayout->addElement(mTextElement);
    mMainLayout->needUpdate(true);
}

void TextLabel::clearElement()
{
    delete mTextElement;
    delete mMainLayout;
}

void TextLabel::edit()
{    
    TextLabelEditor editor(this);
    editor.exec();
}

void TextLabel::update(QCPLayoutElement::UpdatePhase phase)
{
    BoardElement::update(phase);

    if(phase == upLayout)
        mMainLayout->setOuterRect(rect());

    mMainLayout->update(phase);
}

void TextLabel::saveSettings(QSettings *settings)
{
    settings->beginGroup("TextLabelSettings");
    settings->setValue("Text", mText);
    settings->setValue("Color", mColorSettings.color().name(QColor::HexArgb));
    settings->setValue("ColorMode", mColorSettings.mode());
    settings->setValue("BackgroundMode", mBackgroundMode);
    settings->endGroup();
}

void TextLabel::loadSettings(QSettings *settings)
{
    settings->beginGroup("TextLabelSettings");
    mText = settings->value("Text", "Empty Text").toString();
    mColorSettings.setColor(QColor(settings->value("Color").toString()));
    mColorSettings.setMode(ColorSettings::ColorMode(settings->value("ColorMode").toInt()));
    if(settings->contains("BackgroundMode")) {
        auto maxVal = QMetaEnum::fromType<BackgroundMode>().keyCount();
        auto val = settings->value("BackgroundMode").toInt();
        if(val <   maxVal)
            setBackgroundMode(BackgroundMode(val));
    }
    settings->endGroup();
}

QString TextLabel::text() const
{
    return mText;
}

void TextLabel::setText(const QString &text)
{
    mText = text;
}

ColorSettings &TextLabel::colorSettingsRef()
{
    return mColorSettings;
}
