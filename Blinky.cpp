#include "Blinky.h"
#include <iostream>
Blinky::Blinky(const QPixmap& pixmap, QGraphicsScene* scene,
			   const QPointF& initialPosition, QObject* parent)
	: Ghost(pixmap, scene, initialPosition, parent)
{
}
Blinky::~Blinky()
{

}

QPoint Blinky::selectTargetTile()
{

	if (ghostManager->getCurrentMode() == GhostManagerMode::Chase) {
		Player* pacMan = ghostManager->getPlayer();
		if (pacMan) {
			QPointF pacManTile = pacMan->getCurrentTilePosition().toPoint();
			return pacManTile.toPoint();
		}
		else {
			std::cout << "Pacman not found" << std::endl;
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Scatter) {
		return QPointF(405, 0).toPoint();
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Dead) {
		qreal distance = QLineF(pos(), QPointF(200, 220)).length();

		if (distance > 1.0) {
			return QPointF(200, 220).toPoint();
		}
		else {
			ghostManager->endDeadMode(this);
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Alive) {
		return QPointF(405, 0).toPoint();
	}

	return QPointF(0, 0).toPoint();
}
