#pragma once
#include "ghosts.h"
class Blinky : public Ghost {
    Q_OBJECT

public:
    Blinky(const QPixmap& pixmap,QGraphicsScene *scene,
             const QPointF& initialPosition, QObject* parent = 0);
    ~Blinky();
    QPoint selectTargetTile()  override;

};
