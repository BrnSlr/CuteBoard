#ifndef ICON_FACTORY_H
#define ICON_FACTORY_H

#include <QIcon>
#include <QString>
#include <QPainter>
#include <QPalette>
#include <QApplication>


inline QIcon create_icon(QPixmap pix) {

    QColor color_normal = qApp->palette().color(QPalette::Normal, QPalette::ButtonText);
    QPixmap pixmap_enabled = QPixmap(pix);
    QPainter painter(&pixmap_enabled);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setBrush(color_normal);
    painter.setPen(color_normal);
    painter.drawRect(pixmap_enabled.rect());

    QColor color_disabled = qApp->palette().color(QPalette::Disabled, QPalette::ButtonText);
    QPixmap pixmap_disabled = QPixmap(pix);
    QPainter painter_disabled(&pixmap_disabled);
    painter_disabled.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_disabled.setBrush(color_disabled);
    painter_disabled.setPen(color_disabled);
    painter_disabled.drawRect(pixmap_disabled.rect());

    QColor color_active =qApp-> palette().color(QPalette::Normal, QPalette::LinkVisited);
    QPixmap pixmap_active = QPixmap(pix);
    QPainter painter_active(&pixmap_active);
    painter_active.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_active.setBrush(color_active);
    painter_active.setPen(color_active);
    painter_active.drawRect(pixmap_active.rect());

    QColor color_selected = qApp->palette().color(QPalette::Normal, QPalette::Link);
    QPixmap pixmap_selected = QPixmap(pix);
    QPainter painter_selected(&pixmap_selected);
    painter_selected.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_selected.setBrush(color_selected);
    painter_selected.setPen(color_selected);
    painter_selected.drawRect(pixmap_selected.rect());

    // Here is our new colored icon!
    QIcon icon;
    icon.addPixmap(pixmap_selected, QIcon::Normal, QIcon::Off);
    icon.addPixmap(pixmap_selected, QIcon::Normal, QIcon::On);
    icon.addPixmap( pixmap_disabled, QIcon::Disabled, QIcon::Off );
    icon.addPixmap( pixmap_disabled, QIcon::Disabled, QIcon::On );
    icon.addPixmap( pixmap_active, QIcon::Active, QIcon::Off );
    icon.addPixmap( pixmap_active, QIcon::Active, QIcon::On );
    icon.addPixmap( pixmap_enabled, QIcon::Selected, QIcon::Off );
    icon.addPixmap( pixmap_enabled, QIcon::Selected, QIcon::On );

    return icon;
}

inline QIcon create_icon(QString path) {

    QColor color_normal = qApp->palette().color(QPalette::Normal, QPalette::ButtonText);
    QPixmap pixmap_enabled = QPixmap(path);
    QPainter painter(&pixmap_enabled);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setBrush(color_normal);
    painter.setPen(color_normal);
    painter.drawRect(pixmap_enabled.rect());

    QColor color_disabled = qApp->palette().color(QPalette::Disabled, QPalette::ButtonText);
    QPixmap pixmap_disabled = QPixmap(path);
    QPainter painter_disabled(&pixmap_disabled);
    painter_disabled.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_disabled.setBrush(color_disabled);
    painter_disabled.setPen(color_disabled);
    painter_disabled.drawRect(pixmap_disabled.rect());

    QColor color_active = qApp->palette().color(QPalette::Normal, QPalette::LinkVisited);
    QPixmap pixmap_active = QPixmap(path);
    QPainter painter_active(&pixmap_active);
    painter_active.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_active.setBrush(color_active);
    painter_active.setPen(color_active);
    painter_active.drawRect(pixmap_active.rect());

    QColor color_selected = qApp->palette().color(QPalette::Normal, QPalette::Link);
    QPixmap pixmap_selected = QPixmap(path);
    QPainter painter_selected(&pixmap_selected);
    painter_selected.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter_selected.setBrush(color_selected);
    painter_selected.setPen(color_selected);
    painter_selected.drawRect(pixmap_selected.rect());

    // Here is our new colored icon!
    QIcon icon;
    icon.addPixmap(pixmap_enabled, QIcon::Normal, QIcon::Off);
    icon.addPixmap( pixmap_disabled, QIcon::Disabled, QIcon::Off );
    icon.addPixmap( pixmap_active, QIcon::Active, QIcon::Off );
    icon.addPixmap( pixmap_selected, QIcon::Selected, QIcon::Off );
    icon.addPixmap( pixmap_selected, QIcon::Selected, QIcon::On );
    icon.addPixmap( pixmap_selected, QIcon::Normal, QIcon::On);
    icon.addPixmap( pixmap_disabled, QIcon::Disabled, QIcon::On );
    icon.addPixmap( pixmap_active, QIcon::Active, QIcon::On );

    return icon;
}
#endif // ICON_FACTORY_H
