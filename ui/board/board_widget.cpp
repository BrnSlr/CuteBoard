#include "board_widget.h"
#include <QVBoxLayout>
#include "ui/board/management/project.h"
#include "ui/board/factory/element_factory_helper.h"
#include "ui/widgets/element_picker_widget.h"

BoardWidget::BoardWidget(QWidget *parent) : QFrame(parent),
    mBoard(nullptr)
{    
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    mBoard = new Board(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(mBoard);

    setLayout(mainLayout);
}

BoardWidget::~BoardWidget()
{
    if(mBoard != nullptr) {
        delete mBoard;
    }
}

Board *BoardWidget::board() const
{
    return mBoard;
}
