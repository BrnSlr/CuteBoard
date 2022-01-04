#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>
#include <QDirIterator>
#include <QTableWidgetItem>
#include <QMessageBox>
#include "ui/board/management/project.h"

namespace Ui {
class HomePage;
}

class HomePage : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage() override;

    void setProject(const QSharedPointer<Project> &project);

    void loadUserProject(QString projectName);

    QString userWorkingDir() const;
    void setUserWorkingDir(const QString &userWorkingDir);

    QString profileWorkingDir() const;
    void setProfileWorkingDir(const QString &profileName, const QString &profileWorkingDir);

protected:
    QSize minimumSizeHint() const override;

signals:
    void projectSelected();
    void showSettings();
    void workingDirChanged();

public slots:
    void loadUserWorkingDirectory();
    void loadProfileWorkingDirectory();

private slots:
    void prepareContextMenu(const QPoint &pos);
    void openUserWorkingDirectory();
    void createProject();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column = 0);

    void on_treeWidget_2_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::HomePage *ui;
    QSharedPointer<Project> mProject;
    QString mUserWorkingDir;
    QString mProfileWorkingDir;
    QIcon mProjectIcon;
};

#endif // HOMEPAGE_H
