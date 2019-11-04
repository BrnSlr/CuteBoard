#include "homewidget.h"
#include "ui_homewidget.h"

HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    connect(ui->clickToCreateNewPro, SIGNAL(clicked()), this, SLOT(createProject()));

    int idRegular = QFontDatabase::addApplicationFont(":Roboto-Light.ttf");
    QString familyRegular = QFontDatabase::applicationFontFamilies(idRegular).at(0);
    QFont titleFont = QFont(familyRegular);
    titleFont.setPointSize(22);

    ui->titleProjects->setFont(titleFont);
    ui->titleProjects->setStyleSheet("color : rgb(160,175,185);");

    ui->clickToChangeWDir->setStyleSheet("color : rgb(20,140,210);");
    QFont fc = ui->clickToChangeWDir->font();
    fc.setItalic(true);
    ui->clickToChangeWDir->setFont(fc);
    ui->clickToChangeWDir->setMargin(10);
    connect(ui->clickToChangeWDir, SIGNAL(clicked()), this, SLOT(openWorkingDirectory()));

    ui->clickToCreateNewPro->setStyleSheet("color : rgb(20,140,210);");
    ui->clickToCreateNewPro->setFont(fc);
    ui->clickToCreateNewPro->setMargin(10);

    QString dir = QDir::currentPath();
    ui->workingDirLineEdit->setText(dir);
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::openWorkingDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        QDir::currentPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);

    if(!dirPath.isEmpty()&& !dirPath.isNull()){

        ui->workingDirLineEdit->setText(dirPath);
        loadWorkingDirectory();
    }
}

void HomeWidget::loadWorkingDirectory()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    ui->treeWidget->clear();
    mProject->clear();

    QString dirPath = ui->workingDirLineEdit->text();

    QFileInfo dirInfo(dirPath);

    int idRegular = QFontDatabase::addApplicationFont(":Roboto-Light.ttf");
    QString familyRegular = QFontDatabase::applicationFontFamilies(idRegular).at(0);
    QFont proFont = QFont(familyRegular);
    proFont.setPointSize(16);

    if(dirInfo.isDir() && dirInfo.isWritable()) {

        QDir::setCurrent(dirPath);

        QDirIterator directories(dirPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while(directories.hasNext()){
            directories.next();
            QString fileName = QDir(directories.filePath()).dirName() + QString(".project");
            QStringList filter;
            if (!fileName.isEmpty())
                filter << fileName;
            QDirIterator it(directories.filePath(), filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
            QStringList files;
            while (it.hasNext())
                files << it.next();

            if(files.size() == 1) {
                QTBProject pro;
                pro.load(dirPath,QDir(directories.filePath()).dirName(), false);

                QStringList list;
                list << pro.name();
                list << QString("\nCreated : ") + pro.creationDate().toString("yyyy/MM/dd hh:mm:ss") + QString("\n Last modified : ")  + pro.modificationDate().toString("yyyy/MM/dd hh:mm:ss");
                QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget, list);
                treeItem->setIcon(0,QIcon(QPixmap(":/icons8_group_of_projects_50px.png")));
                treeItem->setFont(0, proFont);
                treeItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
                treeItem->setTextAlignment(1, Qt::AlignRight | Qt::AlignBottom);
            }
        }

        ui->treeWidget->resizeColumnToContents(0);
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
}

void HomeWidget::setProject(const QSharedPointer<QTBProject> &project)
{
    mProject = project;
}

void HomeWidget::createProject()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add new project"),
                                         tr("Project name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty()) {
//        qDebug() << QDir::current() << text;

        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();

        QTBProject pro;
        if(pro.generate(QDir::currentPath(), text)) {

            int idRegular = QFontDatabase::addApplicationFont(":Roboto-Light.ttf");
            QString familyRegular = QFontDatabase::applicationFontFamilies(idRegular).at(0);
            QFont proFont = QFont(familyRegular);
            proFont.setPointSize(16);
            proFont.setUnderline(true);

            QStringList list;
            list << pro.name();
            list << QString("\nCreated : ") + pro.creationDate().toString("yyyy/MM/dd hh:mm:ss") + QString("\n Last modified : ")  + pro.modificationDate().toString("yyyy/MM/dd hh:mm:ss");
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget, list);
            treeItem->setIcon(0,QIcon(QPixmap(":/icons8_group_of_projects_50px.png")));
            treeItem->setFont(0, proFont);
            //                treeItem->setSizeHint(0, QSize(250,60));
            treeItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            treeItem->setTextAlignment(1, Qt::AlignRight | Qt::AlignBottom);
            ui->treeWidget->resizeColumnToContents(0);

        } else {
            QApplication::restoreOverrideCursor();
            QApplication::processEvents();
            QMessageBox::warning(this, "Project Creation", "Error : can't create project");
        }
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
    }
}

void HomeWidget::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QString proName = item->text(0);
    if(! mProject->load(QDir::currentPath(), proName)) {
        QMessageBox::warning(this, "Project Selection", "Error : can't load project");
    }
}

