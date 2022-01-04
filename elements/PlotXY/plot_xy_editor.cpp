#include "plot_xy_editor.h"
#include "ui_plot_xy_editor.h"
#include "plot_xy_patron_editor.h"
#include "ui/widgets/utils/icon_factory.h"

PlotXYEditor::PlotXYEditor(PlotXY *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotXYEditor),
    mDisplay(display),
    mXPropertiesWidget(nullptr)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &PlotXYEditor::updateElement);

    QWidget *cornerWidget = new QWidget(this);
    mNewParamButton = new QToolButton(cornerWidget);
    mNewParamButton->setCursor(Qt::ArrowCursor);
    mNewParamButton->setAutoRaise(true);
    mNewParamButton->setIconSize(QSize(20, 20));
    mNewParamButton->setMinimumSize(QSize(30, 30));
    mNewParamButton->setIcon(create_icon(":/icons8_plus_math_32px_1.png"));
    QObject::connect(mNewParamButton, SIGNAL(clicked()), this, SLOT(newParameter()));
    mNewParamButton->setToolTip(tr("Add parameter"));

    mRemoveParamButton = new QToolButton(cornerWidget);
    mRemoveParamButton->setCursor(Qt::ArrowCursor);
    mRemoveParamButton->setAutoRaise(true);
    mRemoveParamButton->setIconSize(QSize(20, 20));
    mRemoveParamButton->setMinimumSize(QSize(30, 30));
    mRemoveParamButton->setIcon(create_icon(":/icons8_minus_math_32px.png"));
    QObject::connect(mRemoveParamButton, SIGNAL(clicked()), this, SLOT(removeParameter()));
    mRemoveParamButton->setToolTip(tr("Remove parameter"));
    mRemoveParamButton->setVisible(false);

    QHBoxLayout *lay = new QHBoxLayout(cornerWidget);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(mRemoveParamButton);
    lay->addWidget(mNewParamButton);

    cornerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    ui->tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [this]() {
        if(ui->tabWidget->currentIndex() > 0)
            mRemoveParamButton->setVisible(true);
        else
            mRemoveParamButton->setVisible(false);
    });

    if(mDisplay) {
        ui->titleCheckBox->setChecked(mDisplay->titleVisible());
        ui->titleLineEdit->setText(mDisplay->title());
        ui->backgroundComboBox->setCurrentIndex(mDisplay->backgroundMode());
        ui->legendPositionComboBox->setCurrentIndex(mDisplay->yLegendPosition());
        ui->yticksCheckBox->setChecked(mDisplay->yAxisTicksVisible());
        ui->ylineCheckBox->setChecked(mDisplay->yAxisLineVisible());
        ui->ylabelsCheckBox->setChecked(mDisplay->yAxisLabelsVisible());
        ui->ygridCheckBox->setChecked(mDisplay->yAxisGridVisible());
        ui->xticksCheckBox->setChecked(mDisplay->xAxisTicksVisible());
        ui->xlineCheckBox->setChecked(mDisplay->xAxisLineVisible());
        ui->xlabelsCheckBox->setChecked(mDisplay->xAxisLabelsVisible());
        ui->xgridCheckBox->setChecked(mDisplay->xAxisGridVisible());

        ui->rangeComboBox->setCurrentIndex(mDisplay->yAxisScale());
        ui->rangeMinDoubleSpinBox->setValue(mDisplay->yAxisMinCustom());
        ui->rangeMaxDoubleSpinBox->setValue(mDisplay->yAxisMaxCustom());
        ui->legendVisibleCheckBox->setChecked(mDisplay->yLegendVisible());
        ui->xrangeComboBox->setCurrentIndex(mDisplay->xAxisScale());
        ui->xrangeMinDoubleSpinBox->setValue(mDisplay->xAxisMinCustom());
        ui->xrangeMaxDoubleSpinBox->setValue(mDisplay->xAxisMaxCustom());
        ui->xLegendVisibleCheckBox->setChecked(mDisplay->xLegendVisible());
        ui->xLegendPositionComboBox->setCurrentIndex(mDisplay->xLegendPosition());

        ui->historySpinBox->setValue(mDisplay->xAxisHistory());
        ui->ythresholdsCheckBox->setChecked(mDisplay->yThresholdsVisible());
        ui->xthresholdsCheckBox->setChecked(mDisplay->xThresholdsVisible());
        ui->yAxisTicksCountSpinBox->setValue(mDisplay->yAxisTicksCount());
        ui->xAxisTicksCountSpinBox->setValue(mDisplay->xAxisTicksCount());
        ui->outOfRangeVisibleCheckBox->setChecked(mDisplay->plotInRangePoints());

        updateTabs();
    }
    setMinimumSize(600,400);
}

PlotXYEditor::~PlotXYEditor()
{
    delete ui;
}

void PlotXYEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void PlotXYEditor::updatePatronsTable()
{
    ui->patronsTableWidget->setRowCount(0);
    QList<QSharedPointer<CurvePatron>> patrons = mDisplay->patrons();
    for(auto patron : patrons) {
        QTableWidgetItem *itemName = new QTableWidgetItem(patron->name());
        QTableWidgetItem *itemColor = new QTableWidgetItem(patron->color().name(QColor::HexArgb));
        itemColor->setForeground(patron->color());
        QTableWidgetItem *itemStyle = new QTableWidgetItem(QMetaEnum::fromType<Qt::PenStyle>().valueToKey(patron->penStyle()));

        ui->patronsTableWidget->insertRow(ui->patronsTableWidget->rowCount());
        ui->patronsTableWidget->setItem(ui->patronsTableWidget->rowCount()-1,0,itemName);
        ui->patronsTableWidget->setItem(ui->patronsTableWidget->rowCount()-1,1,itemColor);
        ui->patronsTableWidget->setItem(ui->patronsTableWidget->rowCount()-1,2,itemStyle);
    }
}

void PlotXYEditor::updateElement()
{
    if(mDisplay) {
        mDisplay->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));

        mDisplay->setTitle(ui->titleLineEdit->text());
        mDisplay->setTitleVisible(ui->titleCheckBox->isChecked());

        mDisplay->setYLegendPosition(PlotXY::YLegendPosition(ui->legendPositionComboBox->currentIndex()));
        mDisplay->setXLegendPosition(PlotXY::XLegendPosition(ui->xLegendPositionComboBox->currentIndex()));

        mDisplay->setYAxisGridVisible(ui->ygridCheckBox->isChecked());
        mDisplay->setYAxisTicksVisible(ui->yticksCheckBox->isChecked());
        mDisplay->setYAxisLineVisible(ui->ylineCheckBox->isChecked());
        mDisplay->setYAxisLabelsVisible(ui->ylabelsCheckBox->isChecked());
        mDisplay->setXAxisGridVisible(ui->xgridCheckBox->isChecked());
        mDisplay->setXAxisTicksVisible(ui->xticksCheckBox->isChecked());
        mDisplay->setXAxisLineVisible(ui->xlineCheckBox->isChecked());
        mDisplay->setXAxisLabelsVisible(ui->xlabelsCheckBox->isChecked());

        mDisplay->setYLegendVisible(ui->legendVisibleCheckBox->isChecked());
        mDisplay->setXLegendVisible(ui->xLegendVisibleCheckBox->isChecked());

        mDisplay->setXAxisHistory(ui->historySpinBox->value());

        mDisplay->setYAxisScale(PlotXY::AxisScale(ui->rangeComboBox->currentIndex()));
        mDisplay->setYAxisMinCustom(ui->rangeMinDoubleSpinBox->value());
        mDisplay->setYAxisMaxCustom(ui->rangeMaxDoubleSpinBox->value());
        mDisplay->setXAxisScale(PlotXY::AxisScale(ui->xrangeComboBox->currentIndex()));
        mDisplay->setXAxisMinCustom(ui->xrangeMinDoubleSpinBox->value());
        mDisplay->setXAxisMaxCustom(ui->xrangeMaxDoubleSpinBox->value());

        mDisplay->setYThresholdsVisible(ui->ythresholdsCheckBox->isChecked());
        mDisplay->setXThresholdsVisible(ui->xthresholdsCheckBox->isChecked());

        mDisplay->setPlotInRangePoints(ui->outOfRangeVisibleCheckBox->isChecked());

        mDisplay->setYAxisTicksCount(ui->yAxisTicksCountSpinBox->value());
        mDisplay->setXAxisTicksCount(ui->xAxisTicksCountSpinBox->value());

        auto listParamY = mDisplay->yParameters();
        for(int i=0; i<listParamY.count();i++) {
            QSharedPointer<BoardParameter> dashParam = listParamY.at(i);
            if(dashParam) {
                if(!mPropertiesWidgets.at(i)->isConnected()) {
                    mPropertiesWidgets.at(i)->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                    dashParam->disconnectSharedConfiguration();
                } else {
                    dashParam->setSharedParameterConfiguration(mPropertiesWidgets.at(i)->currentSettings(), true);
                }
            }
        }

        if(mDisplay->xParameter()) {
            if(!mXPropertiesWidget->isConnected()) {
                mXPropertiesWidget->updateParameterSettings(mDisplay->xParameter()->exclusiveParameterConfiguration());
                mDisplay->xParameter()->disconnectSharedConfiguration();
            } else {
                mDisplay->xParameter()->setSharedParameterConfiguration(mXPropertiesWidget->currentSettings(), true);
            }
        }

        mDisplay->rebuildElement(true);
    }
}

void PlotXYEditor::updateTabs()
{
    int tabCount = ui->tabWidget->count();
    for(int i = 1; i< tabCount;i++) {
        ui->tabWidget->removeTab(1);
    }

    qDeleteAll(mPropertiesWidgets);
    mPropertiesWidgets.clear();

    if(mXPropertiesWidget) {
        delete mXPropertiesWidget;
        mXPropertiesWidget = nullptr;
    }

    if(mDisplay->xParameter()) {
        mXPropertiesWidget = new PropertiesWidget(this);
        mXPropertiesWidget->setVisible(false);
        mXPropertiesWidget->setProject(mDisplay->board()->project());

        if(mDisplay->xParameter()->connected()) {
            mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
            mXPropertiesWidget->updateUi(mDisplay->xParameter()->sharedParameterConfiguration());
        } else {
            if(mDisplay->xParameter()->sharedParameterConfiguration())
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
            else
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mXPropertiesWidget->updateUi(mDisplay->xParameter()->exclusiveParameterConfiguration());
        }
        mXPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmCurveX);
        mXPropertiesWidget->setVisible(true);
        ui->tabWidget->addTab(mXPropertiesWidget, QString("X : %1").arg(mDisplay->xParameter()->getDisplayedLabel()));

        connect(mXPropertiesWidget, &PropertiesWidget::connectProperties,
                [=](bool connected){
            if(connected) {
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                mXPropertiesWidget->updateUi(mDisplay->xParameter()->sharedParameterConfiguration());
            } else {
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                mXPropertiesWidget->updateUi(mDisplay->xParameter()->exclusiveParameterConfiguration());
            }
        });
    } else {
        ui->tabWidget->addTab(new QWidget(), QString("X : "));
    }

    auto listParamY = mDisplay->yParameters();
    for(int i=0; i<listParamY.count();i++) {

        QSharedPointer<BoardParameter> dashParam = listParamY.at(i);
        if(dashParam) {

            PropertiesWidget *propertiesWidget = new PropertiesWidget(this);
            propertiesWidget->setVisible(false);
            propertiesWidget->setProject(mDisplay->board()->project());

            if(dashParam->connected()) {
                propertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                propertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
            } else {
                if(dashParam->sharedParameterConfiguration())
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                else
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
                propertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            }
            propertiesWidget->setPropertiesMode(ParameterConfiguration::cmCurveY);
            propertiesWidget->setVisible(true);
            ui->tabWidget->addTab(propertiesWidget, QString("Y : %1").arg(dashParam->getDisplayedLabel()));

            connect(propertiesWidget, &PropertiesWidget::connectProperties,
                    [propertiesWidget, dashParam](bool connected){
                if(connected) {
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                    propertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
                } else {
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                    propertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
                }
            });

            mPropertiesWidgets.append(propertiesWidget);
        }
    }

   updatePatronsTable();
}

void PlotXYEditor::newParameter()
{
    if(ui->tabWidget->currentIndex() == 1) {
        QString paramLabel = QInputDialog::getText(this, "X Axis : Parameter Label", "X Axis : Parameter Label");
        if(!paramLabel.isEmpty()) {
            mDisplay->addXParameter(paramLabel);
            updateTabs();
            if(!mDisplay->board()->dataManager()->liveDataEnabled()) {
                mDisplay->board()->resetPlayback();
                mDisplay->board()->replot(QCustomPlot::rpQueuedReplot);
            }
        }
    } else {
        QString paramLabel = QInputDialog::getText(this, "Y Axis : Parameter Label", "Y Axis : Parameter Label");
        if(!paramLabel.isEmpty()) {
            mDisplay->addYParameter(paramLabel);
            updateTabs();
            if(!mDisplay->board()->dataManager()->liveDataEnabled()) {
                mDisplay->board()->resetPlayback();
                mDisplay->board()->replot(QCustomPlot::rpQueuedReplot);
            }
        }
    }
}

void PlotXYEditor::removeParameter()
{
    logger()->debug() << Q_FUNC_INFO << ui->tabWidget->currentIndex();
    if(ui->tabWidget->currentIndex() == 1 ) {
        mDisplay->removeXParameter();
    } else if(ui->tabWidget->currentIndex() > 1 ) {
        mDisplay->removeYParameter(ui->tabWidget->currentIndex() - 2 );
    }
    updateTabs();
}

void PlotXYEditor::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void PlotXYEditor::on_rangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget->setEnabled(true);
    else
        ui->scaleWidget->setEnabled(false);
}

void PlotXYEditor::on_xrangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget_2->setEnabled(true);
    else
        ui->scaleWidget_2->setEnabled(false);
}

void PlotXYEditor::on_addPatronButton_clicked()
{    
    QDialog dial;
    dial.setWindowTitle("Plot XY - Add Patron");
    QVBoxLayout dialLayout;
    CurvesXYPatronEditor patronConfig;
    dialLayout.addWidget(&patronConfig);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox,  &QDialogButtonBox::rejected, &dial, &QDialog::reject);
    connect(&buttonBox, &QDialogButtonBox::accepted, [&dial, &patronConfig](){
        if(patronConfig.checkName())
            dial.accept();
        else
            QMessageBox::warning(nullptr, "Patron Creation", "Error : empty name");
    });

    dialLayout.addWidget(&buttonBox);
    dial.setLayout(&dialLayout);
    int res = dial.exec();
    if( res == QDialog::Accepted) {
        auto patron = patronConfig.createPatron();
        if(patron) {
            mDisplay->addPatron(patron);

            updatePatronsTable();
        }
    }
}

void PlotXYEditor::on_deletePatronButton_clicked()
{
    auto items = ui->patronsTableWidget->selectedItems();

    if(items.count() > 0) {
        int row = items.at(0)->row();
        mDisplay->removePatron(row);
        mDisplay->updateItems();

        updatePatronsTable();
    }
}

void PlotXYEditor::on_patronsTableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    QDialog dial;
    dial.setWindowTitle("Plot XY - Patron Edition");
    QVBoxLayout dialLayout;
    CurvesXYPatronEditor patronConfig;
    dialLayout.addWidget(&patronConfig);

    patronConfig.loadPatron(mDisplay->patrons().at(item->row()));

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox,  &QDialogButtonBox::rejected, &dial, &QDialog::reject);
    connect(&buttonBox, &QDialogButtonBox::accepted, [&dial](){
        dial.accept();
    });

    dialLayout.addWidget(&buttonBox);
    dial.setLayout(&dialLayout);
    int res = dial.exec();
    if( res == QDialog::Accepted) {
        patronConfig.updatePatron();
        mDisplay->updateItems();

        updatePatronsTable();
    }
}
