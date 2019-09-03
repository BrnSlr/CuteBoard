#ifndef PAGE_H
#define PAGE_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QDir>
#include "dashboard/dashboard_element.h"

class QTBPage: public QObject
{
    Q_OBJECT
public:
    QTBPage(const QString& pageName, const QString& pageDirectory,
            QObject *parent = nullptr);
    ~QTBPage();

    void loadPageInformation();
    void savePageInformation();

    void loadPageElements(QTBoard *board);
    void savePageElements(QTBoard *board);

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    int columnCount() const;
    void setColumnCount(int columnCount);

    int rowCount() const;
    void setRowCount(int rowCount);

    bool customGridSize() const;
    void setCustomGridSize(bool customGridSize);

    int singleElementColumnCount() const;
    void setSingleElementColumnCount(int singleElementColumnCount);

    int singleElementRowCount() const;
    void setSingleElementRowCount(int singleElementRowCount);

    QString background() const;
    void setBackground(const QString &background);

private:
    QString mSettingsPath;

    QString mName;
    QString mDescription;
    QString mBackground;
    int mColumnCount;
    int mRowCount;
    int mSingleElementColumnCount;
    int mSingleElementRowCount;
    bool mCustomGridSize;
};

#endif // PAGE_H
