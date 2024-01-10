#pragma once
#include "ghosts.h"
class Pinky : public Ghost {
    Q_OBJECT

public:
    Pinky(const QPixmap& pixmap,QGraphicsScene *scene,
             const QPointF& initialPosition, QObject* parent = 0);
    ~Pinky();

    QPoint selectTargetTile()  override;
};
