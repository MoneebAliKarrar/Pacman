#include "Inky.h"

#include <iostream>
Inky::Inky(const QPixmap& pixmap, QGraphicsScene* scene,
		   const QPointF& initialPosition, QObject* parent)
	: Ghost(pixmap, scene, initialPosition, parent)
{
}
Inky::~Inky() {}

QPoint Inky::selectTargetTile()
{
	if (ghostManager->getCurrentMode() == GhostManagerMode::Chase) {
		Player* pacMan = ghostManager->getPlayer();
		Blinky* blinky = getBlinky();

		if (pacMan && blinky) {
			QPointF pacManTile = pacMan->getCurrentTilePosition().toPoint();

			if (pacMan->direction == Direction::Up) {
				pacManTile -= QPoint(0, 40);
			}
			else if (pacMan->direction == Direction::Down) {
				pacManTile += QPoint(0, 40);
			}
			else if (pacMan->direction == Direction::Left) {
				pacManTile -= QPoint(40, 0);
			}
			else if (pacMan->direction == Direction::Right) {
				pacManTile += QPoint(40, 0);
			}

			QPointF blinkyTile = blinky->getCurrentTilePosition().toPoint();
			QPointF vector = 2 * (pacManTile - blinkyTile);

			QPointF inkyTargetTile = pacManTile + vector;

			return inkyTargetTile.toPoint();
		}
		else {
			std::cout << "Pacman or Blinky not found" << std::endl;
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Scatter) {
		return QPointF(scene()->width(), scene()->height()).toPoint();
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Dead) {
		qreal distance = QLineF(pos(), QPointF(180, 280)).length();

		if (distance > 1.0) {
			return QPointF(180, 280).toPoint();
		}
		else {
			ghostManager->endDeadMode(this);
		}
	}
	else if (ghostManager->getCurrentMode() == GhostManagerMode::Frightened &&
			 this->getCurrentMode() == GhostMode::Alive) {
		return QPointF(scene()->width(), scene()->height()).toPoint();
	}
	return QPointF(0, 0).toPoint();
}
Blinky* Inky::getBlinky() const
{
	if (ghostManager) {
		for (int i = 0; i < static_cast<int>(ghostManager->getghosts().size());
			 i++) {
			if (i == 0) {
				return dynamic_cast<Blinky*>(ghostManager->getghosts()[i]);
			}
		}
	}
	return nullptr;
}
