#include "page_picker_widget.h"
#include "page_editor_dialog.h"
#include "ui_page_picker_widget.h"
#include "find_param_widget.h"
#include "ui/widgets/utils/icon_factory.h"

PagePickerWidget::PagePickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PagePickerWidget)
{
    ui->setupUi(this);

    QIcon actionSave_icon = create_icon(":/icons8_save_32px.png");
    QIcon actionEdit_icon = create_icon(":/icons8_edit_32px_1.png");
    QIcon actionReload_icon = create_icon(":/icons8_synchronize_32px.png");
    QIcon actionAdd_icon = create_icon(":/icons8_create_new_50px.png");
    QIcon actionFind_icon = create_icon(":/icons8_search_filled_50px.png");
    QIcon actionExport_icon = create_icon(":/icons8_export_csv_50px.png");
    QIcon actionSaveAs_icon = create_icon(":/icons8_save_as_50px.png");
    mPageIcon = create_icon(QPixmap(":/icons8_file_50px.png"));

    QAction *actionSave = new QAction(actionSave_icon, QString(""));
    actionSave->setToolTip(QString("Save page"));
    QAction *actionEdit = new QAction(actionEdit_icon, QString(""));
    actionEdit->setToolTip(QString("Edit page"));
    QAction *actionReload = new QAction(actionReload_icon, QString(""));
    actionReload->setToolTip(QString("Reload pages"));
    QAction *actionAdd = new QAction(actionAdd_icon, QString(""));
    actionAdd->setToolTip(QString("New page"));
    QAction *actionFind = new QAction(actionFind_icon, QString(""));
    actionFind->setToolTip(QString("Find parameter"));
    QAction *actionExport = new QAction(actionExport_icon, QString(""));
    actionExport->setToolTip(QString("Export pages to csv"));
    QAction *actionSaveAs = new QAction(actionSaveAs_icon, QString(""));
    actionSaveAs->setToolTip(QString("Save page as"));

    QList<QKeySequence> shortcutsSave;
    shortcutsSave << QKeySequence(Qt::CTRL + Qt::Key_S);
    actionSave->setShortcuts(shortcutsSave);

    connect(actionSave, &QAction::triggered,
            this, &PagePickerWidget::savePage);

    ui->savePageButton->setDefaultAction(actionSave);
    ui->editButton->setDefaultAction(actionEdit);
    ui->reloadButton->setDefaultAction(actionReload);
    ui->addPageButton->setDefaultAction(actionAdd);
    ui->findToolButton->setDefaultAction(actionFind);
    ui->exportToolButton->setDefaultAction(actionExport);
    ui->saveAsButton->setDefaultAction(actionSaveAs);

    connect(ui->listWidget, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *item) {
        emit openPage(item->text());
    });
}

PagePickerWidget::~PagePickerWidget()
{
    delete ui;
}

void PagePickerWidget::setBoard(Board *board)
{
    mBoard = board;
    mProject = mBoard->project();

    connect(mProject.data(), &Project::loaded,
            this, &PagePickerWidget::updateList);

    connect(mProject.data(), &Project::updated,
            this, &PagePickerWidget::updateList);

    connect(mProject.data(), &Project::pagesListUpdated,
            this, &PagePickerWidget::updateList);

    connect(mProject.data(), &Project::pageLoaded,
            this, &PagePickerWidget::pageSelected);

    connect(this, &PagePickerWidget::openPage,
            mProject.data(), &Project::requestPage);
}

void PagePickerWidget::on_addPageButton_clicked()
{
    PageEditorDialog editor(false, mProject, this);
    if(editor.exec() == QDialog::Accepted) {
        if(mProject) {
            auto page = QExplicitlySharedDataPointer<Page>(new Page(editor.getName(), mProject->pagesPath()));
            page->setDescription(editor.getDescription());
            page->setBackground(editor.getBackground());
            page->setRowCount(editor.getRowCount());
            page->setColumnCount(editor.getColumnCount());
            page->setSingleElementRowCount(editor.getSingleElementRowCount());
            page->setSingleElementColumnCount(editor.getSingleElementColumnCount());
            page->savePageInformation();
            mProject->addPage(page);

            QMessageBox msgBox;
            msgBox.setText(QString("New page : %1").arg(page->name()));
            msgBox.setInformativeText("Do you want to edit the newly created page?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if(ret == QMessageBox::Yes) {
                emit openPage(page->name());
            }
        }
    }
}

void PagePickerWidget::updateList()
{
    ui->listWidget->clear();
    if(mProject) {
        QStringList pagesNames = mProject->pages().keys();

        QCollator collator;
        collator.setNumericMode(true);

        std::sort(pagesNames.begin(),
                  pagesNames.end(),
                  collator);
        for (int i = 0; i < pagesNames.size(); ++i) {
            QListWidgetItem *item = new QListWidgetItem(pagesNames.at(i),ui->listWidget);
            item->setIcon(mPageIcon);
        }
        loadPage(mProject->currentPageName());
    }
}

void PagePickerWidget::pageSelected()
{
    const QSignalBlocker blocker(this);
    if(mProject) {
        loadPage(mProject->currentPageName());
    }
}

void PagePickerWidget::savePage()
{
    if(mBoard) {
        mBoard->savePage();
    }
}

void PagePickerWidget::loadPage(const QString& pageName)
{
    if(mProject) {
        auto page = mProject->page(pageName);
        if(page) {
            for(int j=0; j<ui->listWidget->count();j++) {
                QListWidgetItem *item =  ui->listWidget->item(j);
                QFont f = item->font();
                if(item->text() == pageName) {
                    f.setBold(true);
                    item->setIcon(create_icon(QPixmap(":/icons8_file_filled_50px.png")));
                    ui->listWidget->setCurrentItem(item);
                    ui->listWidget->scrollToItem(item, QAbstractItemView::PositionAtCenter);
                } else {
                    f.setBold(false);
                    item->setIcon(create_icon(QPixmap(":/icons8_file_50px.png")));
                }
                item->setFont(f);
            }
        }
    }
}

void PagePickerWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    auto item  = ui->listWidget->currentItem();
    if(item)
        ui->listWidget->scrollToItem(item, QAbstractItemView::EnsureVisible);
}

void PagePickerWidget::on_reloadButton_clicked()
{
    if(mProject) {
        mProject->loadPages();
    }
}

void PagePickerWidget::on_editButton_clicked()
{
    if(mProject) {
        QListWidgetItem *item = ui->listWidget->currentItem();
        if(item) {
            QString pageName = item->text();

            auto page = mProject->page(pageName);
            if(page) {
                if(pageName == mProject->currentPageName()) {
                    mBoard->checkModification();
                }
                PageEditorDialog editor(true, mProject, this);
                editor.setName(page->name());
                editor.setDescription(page->description());
                editor.setBackground(page->background());
                editor.setRowCount(page->rowCount());
                editor.setColumnCount(page->columnCount());
                editor.setSingleElementRowCount(page->singleElementRowCount());
                editor.setSingleElementColumnCount(page->singleElementColumnCount());
                if(editor.exec() == QDialog::Accepted) {
                    page->setDescription(editor.getDescription());
                    QDir dir(page->pageDirectory());
                    page->setBackground(editor.getBackground());
                    page->setRowCount(editor.getRowCount());
                    page->setColumnCount(editor.getColumnCount());
                    page->setSingleElementRowCount(editor.getSingleElementRowCount());
                    page->setSingleElementColumnCount(editor.getSingleElementColumnCount());
                    page->savePageInformation();

                    if(pageName == mProject->currentPageName()) {
                        mProject->requestPage(page->name());
                    }
                }
            }
        }
    }
}

void PagePickerWidget::on_findToolButton_clicked()
{
    auto w = new find_param_widget();
    w->setProject(mProject);
    connect(w, &find_param_widget::pageRequested,
            this, &PagePickerWidget::openPage);
    w->exec();
}

void PagePickerWidget::on_exportToolButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName (this, "Export to csv file",
                                                     QString("%1.csv").arg(mProject->name()), "CSV files (*.csv)");

    if(!mProject->exportToCsv(fileName))
        QMessageBox::critical(this, tr("Error"), tr("Cannot export to csv"), QMessageBox::Close);
}

void PagePickerWidget::on_saveAsButton_clicked()
{
    QString pageName = QInputDialog::getText(this, "Save As", "Page name");
    if(!pageName.isEmpty()) {

        auto page = mProject->currentPage();
        if(page) {
            page.detach();
            page->setName(pageName);
            page->savePageInformation();
            page->savePageElements(mBoard);
            mBoard->setPageModified(false);

            mProject->loadPages();

            mProject->requestPage(page->name());
        }
    }
}
