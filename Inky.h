#pragma once
#include "ghosts.h"
#include "Blinky.h"
class Inky : public Ghost {
    Q_OBJECT

public:
    Inky(const QPixmap& pixmap,QGraphicsScene *scene,
             const QPointF& initialPosition, QObject* parent = 0);
    ~Inky();

    QPoint selectTargetTile()  override;
private:
Blinky* getBlinky() const;
};

