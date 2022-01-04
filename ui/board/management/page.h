#ifndef PAGE_H
#define PAGE_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QDir>

#include "ui/ui_global.h"
#include "ui/board/board_element.h"

class BOARD_UI_EXPORT Page: public QSharedData
{
public:
    Page(const QString& pageName, const QString& pageDirectory);

    void loadPageInformation();
    void savePageInformation();

    void loadPageElements(Board *board);
    void savePageElements(Board *board);

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    int columnCount() const;
    void setColumnCount(int columnCount);

    int rowCount() const;
    void setRowCount(int rowCount);

    int singleElementColumnCount() const;
    void setSingleElementColumnCount(int singleElementColumnCount);

    int singleElementRowCount() const;
    void setSingleElementRowCount(int singleElementRowCount);

    QString background() const;
    void setBackground(const QString &background);

    QString pageDirectory() const;
    void setPageDirectory(const QString &pageDirectory);

private:
    QString mSettingsPath;

    QString mName;
    QString mPageDirectory;
    QString mDescription;
    QString mBackground;
    int mColumnCount;
    int mRowCount;
    int mSingleElementColumnCount;
    int mSingleElementRowCount;
};

#endif // PAGE_H
