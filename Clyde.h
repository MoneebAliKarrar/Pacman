#pragma once
#include "ghosts.h"
class Clyde : public Ghost {
    Q_OBJECT

public:
    Clyde(const QPixmap& pixmap,QGraphicsScene *scene,
             const QPointF& initialPosition, QObject* parent = 0);
    ~Clyde();

    QPoint selectTargetTile()  override;
};
