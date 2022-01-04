#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QDebug>
#include "log4qt/logger.h"

class ClickableLabel : public QLabel
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR);
    ~ClickableLabel() Q_DECL_OVERRIDE;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
};

#endif // CLICKABLELABEL_H
