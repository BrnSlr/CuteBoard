#include "project/page.h"
#include "dashboard/layouts/layout_reactive.h"
#include "dashboard/elements_factory/elementfactory.h"

QTBPage::QTBPage(const QString& pageName,
                 const QString& pageDirectory,
                 QObject *parent) :
    QObject(parent),
    mPageDirectory(pageDirectory),
    mColumnCount(24),
    mRowCount(24),
    mSingleElementColumnCount(2),
    mSingleElementRowCount(2),
    mCustomGridSize(false)
{
    mSettingsPath = pageDirectory + QDir::separator() + pageName + QString(".page");
    setName(pageName);
}

QTBPage::~QTBPage()
= default;

void QTBPage::loadPageInformation()
{
    QSettings settings(mSettingsPath, QSettings::IniFormat);

    settings.beginGroup("Global");
    mName = settings.value("Name").toString();
    mDescription = settings.value("Description").toString();
    mBackground = settings.value("Background").toString();
    settings.endGroup();

    settings.beginGroup("Layout");
    mColumnCount = settings.value("Columns").toInt();
    mRowCount = settings.value("Rows").toInt();
    mSingleElementColumnCount = settings.value("SingleElementColumns").toInt();
    mSingleElementRowCount = settings.value("SingleElementRows").toInt();
    mCustomGridSize = settings.value("CustomSize").toBool();
    settings.endGroup();
}

void QTBPage::savePageInformation()
{
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    //    settings.clear();
    settings.sync();

    settings.beginGroup("Global");
    settings.setValue("Name", mName);
    settings.setValue("Description", mDescription);
    settings.setValue("Background", mBackground);
    settings.endGroup();

    settings.beginGroup("Layout");
    settings.setValue("Columns", mColumnCount);
    settings.setValue("Rows", mRowCount);
    settings.setValue("SingleElementColumns", mSingleElementColumnCount);
    settings.setValue("SingleElementRows", mSingleElementRowCount);
    settings.setValue("CustomSize", mCustomGridSize);
    settings.endGroup();
}

void QTBPage::loadPageElements(QTBoard *board)
{
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    int elementsCount = settings.beginReadArray("Elements");

    for(int i=0;i<elementsCount;i++) {

        settings.setArrayIndex(i);
        QString elementName = settings.value("name").toString();

        settings.beginGroup("ElementGeometry");
        QRect elementRect = board->dashboardLayout()->loadElementRectSettings(&settings);
        settings.endGroup();

        QTBDashboardElement *element = ElementFactory::Instance()->Create(elementName);
        if(element) {
            element->initializeElement(board);

            settings.beginGroup("ElementSettings");
            element->loadSettings(&settings);
            settings.endGroup();

            board->dashboardLayout()->addElement(element,
                                                 elementRect);

            if(element) {
            settings.beginGroup("ElementConfigurations");
            element->loadConfigurations(&settings);
            settings.endGroup();
        }
    }
    }

    settings.endArray();
}

void QTBPage::savePageElements(QTBoard *board)
{
    savePageInformation();

    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.beginWriteArray("Elements");
    for(int i= 0; i< board->dashboardLayout()->elementCount();i++) {
        if (auto *el = qobject_cast<QTBDashboardElement*>(board->dashboardLayout()->elementAt(i))) {

            settings.setArrayIndex(i);
            settings.setValue("name", el->name());

            settings.beginGroup("ElementGeometry");
            board->dashboardLayout()->saveElementRectSettings(i, &settings);
            settings.endGroup();

            settings.beginGroup("ElementSettings");
            el->saveSettings(&settings);
            settings.endGroup();

            settings.beginGroup("ElementConfigurations");
            el->saveConfigurations(&settings);
            settings.endGroup();
        }
    }
    settings.endArray();
}

QString QTBPage::name() const
{
    return mName;
}

void QTBPage::setName(const QString &name)
{
    mName = name;
}

QString QTBPage::description() const
{
    return mDescription;
}

void QTBPage::setDescription(const QString &description)
{
    mDescription = description;
}

int QTBPage::columnCount() const
{
    return mColumnCount;
}

void QTBPage::setColumnCount(int columnCount)
{
    mColumnCount = columnCount;
}

int QTBPage::rowCount() const
{
    return mRowCount;
}

void QTBPage::setRowCount(int rowCount)
{
    mRowCount = rowCount;
}

bool QTBPage::customGridSize() const
{
    return mCustomGridSize;
}

void QTBPage::setCustomGridSize(bool customGridSize)
{
    mCustomGridSize = customGridSize;
}

int QTBPage::singleElementColumnCount() const
{
    return mSingleElementColumnCount;
}

void QTBPage::setSingleElementColumnCount(int singleElementColumnCount)
{
    mSingleElementColumnCount = singleElementColumnCount;
}

int QTBPage::singleElementRowCount() const
{
    return mSingleElementRowCount;
}

void QTBPage::setSingleElementRowCount(int singleElementRowCount)
{
    mSingleElementRowCount = singleElementRowCount;
}

QString QTBPage::backgroundPath()
{
    return mPageDirectory + QDir::separator() + mBackground;
}

QString QTBPage::background() const
{
    return mBackground;
}

void QTBPage::setBackground(const QString &background)
{
    mBackground = background;
}

QString QTBPage::pageDirectory() const
{
    return mPageDirectory;
}

void QTBPage::setPageDirectory(const QString &pageDirectory)
{
    mPageDirectory = pageDirectory;
}
