#include "pagepickerwidget.h"
#include "pageeditordialog.h"
#include "ui_pagepickerwidget.h"

PagePickerWidget::PagePickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PagePickerWidget)
{
    ui->setupUi(this);
    connect(ui->savePageButton, SIGNAL(clicked(bool)), this, SIGNAL(saveCurrentPage()));
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *item) {
        emit openPage(item->text());
    });
}

PagePickerWidget::~PagePickerWidget()
{
    delete ui;
}

void PagePickerWidget::on_addPageButton_clicked()
{
    PageEditorDialog editor(false, this);
    if(editor.exec() == QDialog::Accepted) {
        if(mProject) {
            QTBPage *page = new QTBPage(editor.getName(), mProject->pagesPath());
            page->setDescription(editor.getDescription());
            page->setBackground(editor.getBackground());
            page->setRowCount(editor.getRowCount());
            page->setColumnCount(editor.getColumnCount());
            page->setSingleElementRowCount(editor.getSingleElementRowCount());
            page->setSingleElementColumnCount(editor.getSingleElementColumnCount());
            page->setCustomGridSize(editor.getCustomLayout());
            page->savePageInformation();
            mProject->addPage(page);
        }
    }
}

void PagePickerWidget::updateList()
{
    ui->listWidget->clear();
    if(mProject) {
        QStringList pagesNames = mProject->pages().keys();
        pagesNames.sort();
        for (int i = 0; i < pagesNames.size(); ++i) {
            QListWidgetItem *item = new QListWidgetItem(pagesNames.at(i),ui->listWidget);
            item->setIcon(QIcon(QPixmap(":/icons8_file_50px.png")));
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

void PagePickerWidget::setProject(const QSharedPointer<QTBProject> &project)
{
    mProject = project;
}

void PagePickerWidget::loadPage(const QString& pageName)
{
    if(mProject) {
        QTBPage *page = mProject->page(pageName);
        if(page) {
            for(int j=0; j<ui->listWidget->count();j++) {
                QListWidgetItem *item =  ui->listWidget->item(j);
                QFont f = item->font();
                if(item->text() == pageName) {
                    f.setBold(true);
                    item->setIcon(QIcon(QPixmap(":/icons8_file_filled_50px.png")));
                    ui->listWidget->setCurrentItem(item);
                    ui->listWidget->scrollToItem(item, QAbstractItemView::PositionAtCenter);
                } else {
                    f.setBold(false);
                    item->setIcon(QIcon(QPixmap(":/icons8_file_50px.png")));
                }
                item->setFont(f);
            }
        }
    }
}

void PagePickerWidget::on_reloadButton_clicked()
{
    if(mProject) {
        mProject->loadPages();
        updateList();
    }
}

void PagePickerWidget::on_editButton_clicked()
{
    if(mProject) {
        QListWidgetItem *item = ui->listWidget->currentItem();
        if(item) {
            QString pageName = item->text();
            QTBPage *page = mProject->page(pageName);
            if(page) {
                PageEditorDialog editor(true, this);
                editor.setName(page->name());
                editor.setDescription(page->description());
                editor.setBackground(page->background());
                editor.setRowCount(page->rowCount());
                editor.setColumnCount(page->columnCount());
                editor.setSingleElementRowCount(page->singleElementRowCount());
                editor.setSingleElementColumnCount(page->singleElementColumnCount());
                editor.setCustomLayout(page->customGridSize());
                if(editor.exec() == QDialog::Accepted) {
                    page->setDescription(editor.getDescription());
                    page->setBackground(editor.getBackground());
                    page->setRowCount(editor.getRowCount());
                    page->setColumnCount(editor.getColumnCount());
                    page->setSingleElementRowCount(editor.getSingleElementRowCount());
                    page->setSingleElementColumnCount(editor.getSingleElementColumnCount());
                    page->setCustomGridSize(editor.getCustomLayout());
                    page->savePageInformation();
                    emit pageModified(page->name());
                }
            }
        }
    }
}
