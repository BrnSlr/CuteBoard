#ifndef ALARMSLISTWIDGET_H
#define ALARMSLISTWIDGET_H

#include <QListWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>

class AlarmsListWidget : public QListWidget
{
    Q_OBJECT
public:
    AlarmsListWidget(QWidget *parent = nullptr);

private slots:
    void contextMenuRequested(const QPoint& pos);
signals:
    void editAlarm(QString label);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;
};

#endif // ALARMSLISTWIDGET_H
