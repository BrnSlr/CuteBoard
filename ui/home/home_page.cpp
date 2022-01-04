#include "home_page.h"
#include "ui_home_page.h"
#include "ui/settings/settings_dialog.h"
#include "ui/widgets/utils/icon_factory.h"

HomePage::HomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);

    QColor color_active = qApp->palette().color(QPalette::Active, QPalette::Text);
    QPixmap pixmap_active = QPixmap(":/cuteboard.png");
    QPainter painter_active(&pixmap_active);
    painter_active.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_active.setBrush(color_active);
    painter_active.setPen(color_active);
    painter_active.drawRect(pixmap_active.rect());

    ui->labelLogo->setPixmap(pixmap_active);    

    mProjectIcon = create_icon(":/icons8_group_of_projects_50px.png");

    QIcon actionSettings_icon = create_icon(":/icons8_settings_3_32px.png");
    QAction *actionSettings = new QAction(actionSettings_icon, QString(""));
    ui->toolButton->setDefaultAction(actionSettings);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &HomePage::prepareContextMenu);

    connect(ui->treeWidget_2, &QTreeWidget::customContextMenuRequested,
            this, &HomePage::prepareContextMenu);

    connect(ui->clickToCreateNewPro, SIGNAL(clicked()), this, SLOT(createProject()));

    ui->clickToChangeWDir->setStyleSheet("color : palette(link);");
    QFont fc = ui->clickToChangeWDir->font();
    fc.setItalic(true);
    ui->clickToChangeWDir->setFont(fc);
    connect(ui->clickToChangeWDir, SIGNAL(clicked()), this, SLOT(openUserWorkingDirectory()));

    ui->clickToCreateNewPro->setStyleSheet("color : palette(link);");
    ui->clickToCreateNewPro->setFont(fc);

    ui->clickToCreateNewPro_2->setStyleSheet("color : palette(link);");
    ui->clickToCreateNewPro_2->setFont(fc);

    connect(ui->toolButton, SIGNAL(clicked()), this, SIGNAL(showSettings()));

    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                       QSettings::IniFormat);

    mUserWorkingDir = settings.value(QString("WorkingDir")).toString();

    if(mUserWorkingDir.isEmpty()) {
        mUserWorkingDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + QString("CuteBoard");
        QDir dirWorking(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        if(!dirWorking.mkdir(QString("CuteBoard")) && !dirWorking.exists() ) {
            mUserWorkingDir.clear();
        } else {
            settings.setValue(QString("WorkingDir"), mUserWorkingDir);
        }
    }

    QFileInfo outDir(mUserWorkingDir);
    if((!outDir.exists()) || (!outDir.isDir()))
        mUserWorkingDir.clear();

    if(mUserWorkingDir.isEmpty())
        ui->tabWidget->setTabEnabled(1,false);

    ui->workingDirLineEdit->setText(mUserWorkingDir);
    ui->versionLabel->setText(QString("Product Version : %1").arg(QApplication::applicationVersion()));
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::openUserWorkingDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        mUserWorkingDir,
                                                        QFileDialog::ShowDirsOnly);

    if(!dirPath.isEmpty()&& !dirPath.isNull()){

        QMessageBox box;
        box.setWindowTitle(tr("User working directory"));
        box.setIcon(QMessageBox::Question);
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        QString text = tr("Do you want to set this directory as the default one?");
        box.setText(text);

        if (box.exec() == QMessageBox::Yes) {
            QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                               QSettings::IniFormat);
            settings.setValue(QString("WorkingDir"), dirPath);
        }

        emit workingDirChanged();
        setUserWorkingDir(dirPath);
    }
}

void HomePage::loadUserWorkingDirectory()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    ui->treeWidget->clear();
    QFileInfo dirInfo(mUserWorkingDir);

    if(dirInfo.isDir() && dirInfo.isWritable()) {

        QDirIterator directories(mUserWorkingDir, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
        while(directories.hasNext()){
            directories.next();
            QString fileName = QDir(directories.filePath()).dirName() + QString(".project");
            QStringList filter;
            if (!fileName.isEmpty())
                filter << fileName;
            QDirIterator it(directories.filePath(), filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
            QStringList files;
            while (it.hasNext()) {
                files << it.next();
            }

            if(files.size() == 1) {
                Project pro;
                if(pro.load(mUserWorkingDir,QDir(directories.filePath()).dirName(), false)) {

                    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);
                    treeItem->setText(0, pro.name());

                    treeItem->setToolTip(0, QString("Creation : %1\nModification :%2").arg(pro.creationDate().toString("yyyy/MM/dd hh:mm:ss"))
                                         .arg(pro.modificationDate().toString("yyyy/MM/dd hh:mm:ss")));
                    treeItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
                    treeItem->setSizeHint(0,QSize(100,25));
                }
            }
        }
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

void HomePage::loadProfileWorkingDirectory()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    ui->treeWidget_2->clear();

    QFileInfo dirInfo(mProfileWorkingDir);

    if(dirInfo.isDir() && dirInfo.isWritable()) {

        QDirIterator directories(mProfileWorkingDir, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
        while(directories.hasNext()){
            directories.next();
            QString fileName = QDir(directories.filePath()).dirName() + QString(".project");
            QStringList filter;
            if (!fileName.isEmpty())
                filter << fileName;
            QDirIterator it(directories.filePath(), filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
            QStringList files;
            while (it.hasNext()) {
                files << it.next();
            }

            if(files.size() == 1) {
                Project pro;
                if(pro.load(mProfileWorkingDir,QDir(directories.filePath()).dirName(), false)) {

                    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget_2);
                    treeItem->setText(0, pro.name());

                    treeItem->setToolTip(0, QString("Creation : %1\nModification :%2").arg(pro.creationDate().toString("yyyy/MM/dd hh:mm:ss"))
                                         .arg(pro.modificationDate().toString("yyyy/MM/dd hh:mm:ss")));
                    treeItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
                    treeItem->setSizeHint(0,QSize(100,25));
                }
            }
        }
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

void HomePage::prepareContextMenu( const QPoint &pos)
{
    QTreeWidget *tree = qobject_cast<QTreeWidget*>(sender());
    if(tree) {
        auto item = tree->itemAt(pos);

        if(item) {
            QAction *actionSelect = new QAction("Select project", this);
            QMenu menu(this);
            menu.addAction(actionSelect);

            connect(actionSelect, &QAction::triggered, [tree, item]() {
                //                on_treeWidget_itemDoubleClicked(item);
                emit tree->itemDoubleClicked(item, 0);
            });

            menu.exec(tree->mapToGlobal(pos));
        }
    }
}

void HomePage::setProject(const QSharedPointer<Project> &project)
{
    mProject = project;
}

void HomePage::loadUserProject(QString projectName)
{
    if(mProject && ! mProject->load(mUserWorkingDir, projectName)) {
        QMessageBox::warning(this, "Project Selection", "Error : can't load project");
    } else {
        int rowCount = ui->treeWidget->topLevelItemCount();
        for(int i=0; i<rowCount; i++)
            ui->treeWidget->topLevelItem(i)->setIcon(0, QIcon());

        rowCount = ui->treeWidget_2->topLevelItemCount();
        QFont f = ui->treeWidget_2->font();
        for(int i=0; i<rowCount; i++) {
            ui->treeWidget_2->topLevelItem(i)->setIcon(0, QIcon());
            ui->treeWidget_2->topLevelItem(i)->setFont(0,f);
        }
        ui->treeWidget_2->clearSelection();

        auto items = ui->treeWidget->findItems(projectName, Qt::MatchExactly);
        if(items.count() > 0) {
            f.setBold(true);
            items.at(0)->setIcon(0, mProjectIcon);
            items.at(0)->setFont(0, f);
        }

        emit projectSelected();
    }
}

QSize HomePage::minimumSizeHint() const
{
    return QSize(0,0);
}

void HomePage::createProject()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add new project"),
                                         tr("Project name:"), QLineEdit::Normal,
                                         "ProjectName", &ok);
    if (ok && !text.isEmpty()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();

        Project pro;
        if(pro.generate(mUserWorkingDir, text)) {

            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);
            treeItem->setText(0, pro.name());
            treeItem->setToolTip(0, QString("Creation : %1\nModification :%2").arg(pro.creationDate().toString("yyyy/MM/dd hh:mm:ss"))
                                 .arg(pro.modificationDate().toString("yyyy/MM/dd hh:mm:ss")));
            treeItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            treeItem->setSizeHint(0,QSize(100,25));
            ui->treeWidget->resizeColumnToContents(2);
            ui->treeWidget->resizeColumnToContents(1);
        } else {
            QApplication::restoreOverrideCursor();
            QApplication::processEvents();
            QMessageBox::warning(this, "Project Creation", "Error : can't create project");
        }
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
    }
}

void HomePage::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    QString proName = item->text(0);
    if(mProject && ! mProject->load(mUserWorkingDir, proName)) {
        QMessageBox::warning(this, "Project Selection", "Error : can't load project");
    } else {
        int rowCount = ui->treeWidget->topLevelItemCount();
        for(int i=0; i<rowCount; i++)
            ui->treeWidget->topLevelItem(i)->setIcon(0, QIcon());

        rowCount = ui->treeWidget_2->topLevelItemCount();
        for(int i=0; i<rowCount; i++)
            ui->treeWidget_2->topLevelItem(i)->setIcon(0, QIcon());
        ui->treeWidget_2->clearSelection();

        item->setIcon(0, QIcon(QPixmap(":/icons8_group_of_projects_50px.png")));

        emit projectSelected();
    }
    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

void HomePage::on_treeWidget_2_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    QString proName = item->text(0);
    if(mProject && ! mProject->load(mProfileWorkingDir, proName)) {
        QMessageBox::warning(this, "Project Selection", "Error : can't load project");
    } else {
        int rowCount = ui->treeWidget->topLevelItemCount();
        for(int i=0; i<rowCount; i++)
            ui->treeWidget->topLevelItem(i)->setIcon(0, QIcon());
        ui->treeWidget->clearSelection();

        rowCount = ui->treeWidget_2->topLevelItemCount();
        for(int i=0; i<rowCount; i++)
            ui->treeWidget_2->topLevelItem(i)->setIcon(0, QIcon());

        item->setIcon(0, QIcon(QPixmap(":/icons8_group_of_projects_50px.png")));

        emit projectSelected();
    }
    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

QString HomePage::profileWorkingDir() const
{
    return mProfileWorkingDir;
}

void HomePage::setProfileWorkingDir(const QString &profileName, const QString &profileWorkingDir)
{
    if(profileWorkingDir.isEmpty()) {
        ui->tabWidget->setTabEnabled(0,false);
    } else {
        ui->tabWidget->setTabEnabled(0,true);
        mProfileWorkingDir = profileWorkingDir;
        ui->workingDirLineEdit_2->setText(profileWorkingDir);
        ui->tabWidget->setTabText(0, QString("%1 Working Directory").arg(profileName));
        ui->tabWidget->setCurrentIndex(0);
        loadProfileWorkingDirectory();
    }
}

QString HomePage::userWorkingDir() const
{
    return mUserWorkingDir;
}

void HomePage::setUserWorkingDir(const QString &userWorkingDir)
{
    if(!userWorkingDir.isEmpty()) {
        ui->tabWidget->setTabEnabled(1,true);
        mUserWorkingDir = userWorkingDir;
        ui->workingDirLineEdit->setText(userWorkingDir);
        loadUserWorkingDirectory();
    }
}
