#include "page.h"
#include "ui/board/board_layout.h"
#include "ui/board/factory/element_factory_helper.h"

Page::Page(const QString& pageName,
                 const QString& pageDirectory) :
    mPageDirectory(pageDirectory),
    mColumnCount(24),
    mRowCount(24),
    mSingleElementColumnCount(2),
    mSingleElementRowCount(2)
{
    setName(pageName);
}

void Page::loadPageInformation()
{
    QSettings settings(mSettingsPath, QSettings::IniFormat);

    settings.beginGroup("Global");
    mDescription = settings.value("Description").toString();
    auto back = settings.value("Background").toString();
    mBackground = QFileInfo(back).fileName();
    settings.endGroup();

    settings.beginGroup("Layout");
    mColumnCount = settings.value("Columns").toInt();
    mRowCount = settings.value("Rows").toInt();
    mSingleElementColumnCount = settings.value("SingleElementColumns").toInt();
    mSingleElementRowCount = settings.value("SingleElementRows").toInt();
    settings.endGroup();
}

void Page::savePageInformation()
{
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.sync();

    settings.beginGroup("Global");
    settings.setValue("Description", mDescription);
    settings.setValue("Background", mBackground);
    settings.endGroup();

    settings.beginGroup("Layout");
    settings.setValue("Columns", mColumnCount);
    settings.setValue("Rows", mRowCount);
    settings.setValue("SingleElementColumns", mSingleElementColumnCount);
    settings.setValue("SingleElementRows", mSingleElementRowCount);
    settings.endGroup();
}

void Page::loadPageElements(Board *board)
{
    QSettings settings(mSettingsPath, QSettings::IniFormat);
    int elementsCount = settings.beginReadArray("Elements");

    for(int i=0;i<elementsCount;i++) {

        settings.setArrayIndex(i);
        QString elementName = settings.value("name").toString();

        settings.beginGroup("ElementGeometry");
        QRect elementRect = board->dashboardLayout()->loadElementRectSettings(&settings);
        settings.endGroup();

        BoardElement *element = ElementFactory::Factory()->createElement(elementName);
        if(element) {
            element->initialize(board);

            settings.beginGroup("ElementSettings");
            element->loadSettings(&settings);
            settings.endGroup();

            if(board->dashboardLayout()->addElement(element,
                                                 elementRect)) {
                settings.beginGroup("ElementConfigurations");
                element->loadConfigurations(&settings);
                settings.endGroup();
            }
        }
    }

    settings.endArray();
}

void Page::savePageElements(Board *board)
{

    QSettings settings(mSettingsPath, QSettings::IniFormat);
    settings.clear();

    savePageInformation();

    settings.beginWriteArray("Elements");
    for(int i= 0; i< board->dashboardLayout()->elementCount();i++) {
        if (auto *el = qobject_cast<BoardElement*>(board->dashboardLayout()->elementAt(i))) {

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

QString Page::name() const
{
    return mName;
}

void Page::setName(const QString &name)
{
    mName = name;    
    mSettingsPath = mPageDirectory + QDir::separator() + mName + QString(".page");
}

QString Page::description() const
{
    return mDescription;
}

void Page::setDescription(const QString &description)
{
    mDescription = description;
}

int Page::columnCount() const
{
    return mColumnCount;
}

void Page::setColumnCount(int columnCount)
{
    mColumnCount = columnCount;
}

int Page::rowCount() const
{
    return mRowCount;
}

void Page::setRowCount(int rowCount)
{
    mRowCount = rowCount;
}

int Page::singleElementColumnCount() const
{
    return mSingleElementColumnCount;
}

void Page::setSingleElementColumnCount(int singleElementColumnCount)
{
    mSingleElementColumnCount = singleElementColumnCount;
}

int Page::singleElementRowCount() const
{
    return mSingleElementRowCount;
}

void Page::setSingleElementRowCount(int singleElementRowCount)
{
    mSingleElementRowCount = singleElementRowCount;
}

QString Page::background() const
{
    return mBackground;
}

void Page::setBackground(const QString &background)
{
    mBackground = background;
}

QString Page::pageDirectory() const
{
    return mPageDirectory;
}

void Page::setPageDirectory(const QString &pageDirectory)
{
    mPageDirectory = pageDirectory;
}
