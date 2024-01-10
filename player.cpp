#include "gamewindow.h"
#include "player.h"
#include <QBrush>
#include <QDebug>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPen>
#include <iostream>
Player::Player(QGraphicsScene* scene) : QObject(), QGraphicsEllipseItem()
{

	setRect(0, 0, 20, 20);
	setPos(200, 320);
	setBrush(QBrush(QColor("#FFFF00")));
	QPen pen(QColor("#FFFF00"));
	setPen(pen);
	timer = std::make_unique<QTimer>(this);
	connect(timer.get(), &QTimer::timeout, this, &Player::updatePosition);
	connect(timer.get(), &QTimer::timeout, this, &Player::changeMouthShape);
	timer->start(100);

	direction = Direction::Right;
	currentMode = PlayerMode::Normal;
	energizedTimer = std::make_unique<QTimer>(this);
	scene->addItem(this);
	currentMouthShape = 0;
	mouthShapes << QPair<int, int>(0, 360) << QPair<int, int>(30, 300)
				<< QPair<int, int>(45, 270) << QPair<int, int>(60, 240)
				<< QPair<int, int>(75, 210);
	livesCount = 3;
}

void Player::updatePosition()
{
	qreal step = 20;
	switch (direction) {
	case Direction::Up:
		if (isValidMove(x(), y() - step)) {
			setPos(x(), y() - step);
		}
		break;
	case Direction::Down:
		if (isValidMove(x(), y() + step)) {
			setPos(x(), y() + step);
		}
		break;
	case Direction::Left:
		if (isValidMove(x() - step, y())) {
			setPos(x() - step, y());
		}
		break;
	case Direction::Right:

		if (isValidMove(x() + step, y())) {

			setPos(x() + step, y());
		}
		break;
	}
	checkCollectibleCollisions();
	handleGhostCollision();
}
void Player::setDirection(Direction newDirection)
{
	direction = newDirection;
}
bool Player::isValidMove(qreal nextX, qreal nextY)
{

	if (nextX < 0 && direction == Direction::Left) {
		setPos(420, 260);
	}
	else if (nextX >= scene()->width() && direction == Direction::Right) {
		setPos(-20, 260);
	}
	QList<QGraphicsItem*> collidingItems =
		scene()->items(QPointF(nextX, nextY));

	for (QGraphicsItem* item : collidingItems) {
		if (item == this) {
			continue;
		}
		if (item->type() == WallItem::Type) {
			return false;
		}
		if (item->type() == QGraphicsRectItem::Type) {
			return true;
		}
	}
	return true;
}
bool Player::isValidMoveForDirection(Direction newDirection)
{
	qreal nextX = x();
	qreal nextY = y();
	qreal step = 10;

	switch (newDirection) {
	case Direction::Up:
		nextY -= step;
		break;
	case Direction::Down:
		nextY += 2 * step;
		break;
	case Direction::Left:
		nextX -= step;
		break;
	case Direction::Right:
		nextX += 2 * step;
		break;
	}


	return isValidMove(nextX, nextY);
}
void Player::changeMouthShape()
{
	currentMouthShape = (currentMouthShape + 1) % mouthShapes.size();


	int startAngle = mouthShapes[currentMouthShape].first;
	int spanAngle = mouthShapes[currentMouthShape].second;

	if (direction == Direction::Left) {
		setStartAngle((180 + startAngle) * 16);
		setSpanAngle(spanAngle * 16);
	}
	else if (direction == Direction::Up) {
		setStartAngle((90 + startAngle) * 16);
		setSpanAngle(spanAngle * 16);
	}
	else if (direction == Direction::Down) {
		setStartAngle((270 + startAngle) * 16);
		setSpanAngle(spanAngle * 16);
	}
	else {

		setStartAngle(startAngle * 16);
		setSpanAngle(spanAngle * 16);
	}
}
void Player::checkCollectibleCollisions()
{

	QList<QGraphicsItem*> collidingItems = this->collidingItems();
	for (QGraphicsItem* item : collidingItems) {
		if (item->type() == CollectibleItem::Type) {
			CollectibleItem* collectible = dynamic_cast<CollectibleItem*>(item);
			if (collectible) {
				CollectibleItem::CollectibleType itemType =
					collectible->getCollectibleType();
				if (itemType == CollectibleItem::Simple) {

				}
				else if (itemType == CollectibleItem::Special) {

					handleSpecialCollectibleCollision();
				}
				emit collectibleCollected(itemType);
				scene()->removeItem(item);
			}
		}
	}
}
QPointF Player::getCurrentTilePosition() const
{

	int tileX = static_cast<int>(x());
	int tileY = static_cast<int>(y());

	return QPointF(tileX, tileY);
}
void Player::handleSpecialCollectibleCollision()
{
	setCurrentMode(PlayerMode::Energized);
	energizedTimer->start(7000);
	connect(energizedTimer.get(), &QTimer::timeout, this,
			&Player::endEnergizedMode);
}

void Player::endEnergizedMode()
{
	setCurrentMode(PlayerMode::Normal);
}
void Player::setCurrentMode(PlayerMode mode)
{
	currentMode = mode;
	emit playerModeChanged(mode);
}
void Player::disconnectUpdateSignal()
{
	disconnect(timer.get(), &QTimer::timeout, this, &Player::updatePosition);
	disconnect(timer.get(), &QTimer::timeout, this, &Player::changeMouthShape);
	timer->stop();
}
void Player::handleGhostCollision()
{
	QList<QGraphicsItem*> collidingItems = this->collidingItems();

	for (QGraphicsItem* item : collidingItems) {
		if (item->type() == Ghost::Type) {
			Ghost* collidedGhost = dynamic_cast<Ghost*>(item);

			if (!collidedGhost) {

				return;
			}

			if (isGhostCollision() &&
				this->getCurrentMode() == PlayerMode::Normal &&
				(ghostManager->getCurrentMode() == GhostManagerMode::Chase ||
				 ghostManager->getCurrentMode() == GhostManagerMode::Scatter)) {

				if (--this->livesCount == 0) {
					emit pacmandead();
				}
				emit ghostCollision();
				ghostManager->restartGameAfterCollision();
			}
			else if (isGhostCollision() &&
					 this->getCurrentMode() == PlayerMode::Energized &&
					 ghostManager->getCurrentMode() ==
						 GhostManagerMode::Frightened) {

				ghostManager->startDeadMode(collidedGhost);
			}
		}
	}
}
bool Player::isGhostCollision()
{
	QList<QGraphicsItem*> collidingItems = this->collidingItems();
	for (QGraphicsItem* item : collidingItems) {
		if (item->type() == Ghost::Type) {
			return true;
		}
	}
	return false;
}
void Player::setGhostManager(GhostManager* manager)
{
	ghostManager = manager;
}
void Player::stoptimer()
{
	timer->stop();
};
void Player::startimer()
{
	timer->start(100);
};
PlayerMode Player::getCurrentMode()
{
	return currentMode;
};
