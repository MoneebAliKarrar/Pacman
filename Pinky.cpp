#include "Pinky.h"
#include <iostream>
Pinky::Pinky(const QPixmap& pixmap, QGraphicsScene* scene,
			 const QPointF& initialPosition, QObject* parent)
	: Ghost(pixmap, scene, initialPosition, parent)
{
}
Pinky::~Pinky() {}

QPoint Pinky::selectTargetTile()
{
	if (ghostManager->getCurrentMode() == GhostManagerMode::Chase) {
		Player* pacMan = ghostManager->getPlayer();
		if (pacMan) {
			QPointF pacManTile = pacMan->getCurrentTilePosition().toPoint();
			if (pacMan->direction == Direction::Up) {
				pacManTile -= QPoint(80, 80);
			}
			else if (pacMan->direction == Direction::Down) {
				pacManTile += QPoint(0, 80);
			}
			else if (pacMan->direction == Direction::Left) {
				pacManTile += QPoint(-80, 0);
			}
			else if (pacMan->direction == Direction::Right) {
				pacManTile += QPoint(80, 0);
			}
			return pacManTile.toPoint();
		}
		else {
			std::cout << "Pacman not found" << std::endl;
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Scatter) {
		return QPointF(0, 0).toPoint();
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Dead) {
		qreal distance = QLineF(pos(), QPointF(200, 280)).length();

		if (distance > 1.0) {
			return QPointF(200, 280).toPoint();
		}
		else {
			ghostManager->endDeadMode(this);
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Alive) {
		return QPointF(0, 0).toPoint();
	}
	return QPointF(0, 0).toPoint();
}
