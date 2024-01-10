#include "Clyde.h"
#include <iostream>
Clyde::Clyde(const QPixmap& pixmap, QGraphicsScene* scene,
			 const QPointF& initialPosition, QObject* parent)
	: Ghost(pixmap, scene, initialPosition, parent)
{
}
Clyde::~Clyde() {}
QPoint Clyde::selectTargetTile()
{
	if (ghostManager->getCurrentMode() == GhostManagerMode::Chase) {
		Player* pacMan = ghostManager->getPlayer();
		if (pacMan) {
			QPointF pacManTile = pacMan->getCurrentTilePosition().toPoint();
			qreal distance = QLineF(pos(), pacManTile).length();
			if (distance > 8 * 20) {
				return pacManTile.toPoint();
			}
			else {
				return QPointF(0, scene()->height()).toPoint();
			}
		}
		else {
			std::cout << "Pacman not found" << std::endl;
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Scatter) {
		return QPointF(0, scene()->height()).toPoint();
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Dead) {
		qreal distance = QLineF(pos(), QPointF(220, 280)).length();

		if (distance > 1.0) {
			return QPointF(220, 280).toPoint();
		}
		else {
			ghostManager->endDeadMode(this);
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Alive) {
		return QPointF(0, scene()->height()).toPoint();
	}
	return QPointF(0, 0).toPoint();
}
