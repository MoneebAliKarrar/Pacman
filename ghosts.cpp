#include "gamewindow.h"
#include "ghosts.h"
#include "player.h"
#include <QDebug>
#include <QtGlobal>
#include <iostream>
Ghost::Ghost(const QPixmap& pixmap, QGraphicsScene* scene,
			 const QPointF& initialPosition, QObject* parent)
	: QObject(parent), QGraphicsPixmapItem(pixmap),
	  currentDirecion(Direction::Up)
{

	setPos(initialPosition);
	regularPixmap = pixmap;

	setPixmap(pixmap.scaled(20, 20));

	this->setCurrentMode(GhostMode::Alive);
	scene->addItem(this);
}
QPointF Ghost::getCurrentTilePosition() const
{
	int tileX = static_cast<int>(this->x());
	int tileY = static_cast<int>(this->y());

	return QPointF(tileX, tileY);
}
void Ghost::move()
{
	QPointF nextTile = selectTargetTile();
	ghostManager->makeDirectionDecision(this, nextTile);
	ghostManager->handleReverseDirection();

	qreal step = 20;
	switch (getCurrentDirection()) {
	case Direction::Up:
		if (ghostManager->isValidMove(x(), y() - step, this)) {
			setPos(x(), y() - step);
		}
		break;
	case Direction::Down:
		if (ghostManager->isValidMove(x(), y() + step, this)) {
			setPos(x(), y() + step);
		}
		break;
	case Direction::Left:
		if (ghostManager->isValidMove(x() - step, y(), this)) {
			setPos(x() - step, y());
		}
		break;
	case Direction::Right:
		if (ghostManager->isValidMove(x() + step, y(), this)) {
			setPos(x() + step, y());
		}
		break;
	}
}
GhostMode Ghost::getCurrentMode() const
{
	return currentMode;
}
void Ghost::setCurrentMode(GhostMode mode)
{
	currentMode = mode;
}
Direction Ghost::getCurrentDirection()
{
	return currentDirecion;
};
void Ghost::setDirection(Direction direction)
{
	currentDirecion = direction;
};
GhostManager::GhostManager(QObject* parent, QGraphicsScene* scene,
						   const std::vector<Ghost*> allGhosts, Player* pacman)
	: QObject(parent), currentMode(GhostManagerMode::Chase), waveCount(0),
	  ghosts(allGhosts), player(pacman)
{
	this->scene = scene;
	timer = std::make_unique<QTimer>(this);
	movetimer = std::make_unique<QTimer>(this);
	timer->setSingleShot(false);
	connect(movetimer.get(), &QTimer::timeout, this,
			&GhostManager::moveAllGhosts);
	connect(timer.get(), &QTimer::timeout, this, &GhostManager::startWave);
	connect(player, &Player::playerModeChanged, this,
			&GhostManager::handlePlayerModeChange);
	timer->start(0);
	movetimer->start(300);
}
void GhostManager::moveAllGhosts()
{

	for (const auto& ghost : ghosts) {
		ghost->move();
	}
}
void GhostManager::startWave()
{

	std::cout << "wavecount is " << waveCount << std::endl;
	std::cout << "the moode is " << getCurrentMode() << std::endl;

	if (waveCount <= 5) {
		handleModeChange();
	}
	else {
		currentMode = GhostManagerMode::Chase;
	}
	if (currentMode == GhostManagerMode::Scatter) {
		timer->start(7000);
	}
	else if (currentMode == GhostManagerMode::Chase) {
		timer->start(20000);
	}

	++waveCount;
}

void GhostManager::handleModeChange()
{

	if (getWaveCount() == 0 && isModeSwitchTimerExpired()) {
		currentMode = GhostManagerMode::Scatter;
		return;
	}
	if (getWaveCount() > 0) {
		if (getCurrentMode() == Scatter) {
			currentMode = GhostManagerMode::Chase;
		}
		else {
			currentMode = GhostManagerMode::Scatter;
		}
	}
}

void GhostManager::startFrightenedMode()
{
	currentMode = GhostManagerMode::Frightened;
	for (const auto& ghost : ghosts) {
		QPixmap frightenedPixmap("frightened.png");
		ghost->QGraphicsPixmapItem::setPixmap(frightenedPixmap.scaled(20, 20));
	}
	connect(timer.get(), &QTimer::timeout, this,
			&GhostManager::endFrightenedMode);
	timer->start(4000);
}

void GhostManager::endFrightenedMode()
{
	qDebug() << "frightend moode finished";
	disconnect(timer.get(), &QTimer::timeout, this,
			   &GhostManager::endFrightenedMode);
	for (const auto& ghost : ghosts) {
		ghost->QGraphicsPixmapItem::setPixmap(
			ghost->regularPixmap.scaled(20, 20));
	}
	if (currentMode == GhostManagerMode::Scatter) {
		timer->start(7000);
	}
	else {
		timer->start(20000);
	}
}
bool GhostManager::isModeSwitchTimerExpired()
{
	return timer->remainingTime() <= 100;
}
GhostManagerMode GhostManager::getCurrentMode()
{
	return currentMode;
}
int GhostManager::getWaveCount()
{
	return waveCount;
}


void GhostManager::reverseDirection()
{
	for (const auto& ghost : ghosts) {
		if (ghost->getCurrentDirection() == Direction::Up) {
			ghost->setDirection(Direction::Down);
		}
		else if (ghost->getCurrentDirection() == Direction::Down) {
			ghost->setDirection(Direction::Up);
		}
		else if (ghost->getCurrentDirection() == Direction::Left) {
			ghost->setDirection(Direction::Right);
		}
		else if (ghost->getCurrentDirection() == Direction::Right) {
			ghost->setDirection(Direction::Left);
		}
	}
}
void GhostManager::handleReverseDirection()
{
	if (isModeSwitchTimerExpired()) {
		reverseDirection();
	}
}
Direction GhostManager::calculateBestDirection(const QPointF& currentTile,
											   const QPointF& targetTile,
											   Ghost* ghost) const
{
	qreal tileSize = 20.0;
	Direction currentDirection = ghost->getCurrentDirection();
	bool canMoveUp =
		currentDirection != Direction::Down &&
		isValidMove(currentTile.x(), currentTile.y() - tileSize, ghost);
	bool canMoveDown =
		currentDirection != Direction::Up &&
		isValidMove(currentTile.x(), currentTile.y() + tileSize, ghost);
	bool canMoveLeft =
		currentDirection != Direction::Right &&
		isValidMove(currentTile.x() - tileSize, currentTile.y(), ghost);
	bool canMoveRight =
		currentDirection != Direction::Left &&
		isValidMove(currentTile.x() + tileSize, currentTile.y(), ghost);
	qreal distanceUp =
		canMoveUp
			? QLineF(QPointF(currentTile.x(), currentTile.y() - 20), targetTile)
				  .length()
			: std::numeric_limits<qreal>::max();
	qreal distanceDown =
		canMoveDown
			? QLineF(QPointF(currentTile.x(), currentTile.y() + 20), targetTile)
				  .length()
			: std::numeric_limits<qreal>::max();
	qreal distanceLeft =
		canMoveLeft
			? QLineF(QPointF(currentTile.x() - 20, currentTile.y()), targetTile)
				  .length()
			: std::numeric_limits<qreal>::max();
	qreal distanceRight =
		canMoveRight
			? QLineF(QPointF(currentTile.x() + 20, currentTile.y()), targetTile)
				  .length()
			: std::numeric_limits<qreal>::max();

	qreal minDistance =
		qMin(distanceUp, qMin(distanceDown, qMin(distanceLeft, distanceRight)));
	if (minDistance == distanceUp) {
		return Direction::Up;
	}
	else if (minDistance == distanceDown) {
		return Direction::Down;
	}
	else if (minDistance == distanceLeft) {
		return Direction::Left;
	}
	else {
		return Direction::Right;
	}
}
void GhostManager::makeDirectionDecision(Ghost* ghost, const QPointF& nextTile)
{

	QPointF currentTile = ghost->getCurrentTilePosition();
	Direction bestDirection =
		calculateBestDirection(currentTile, nextTile, ghost);
	ghost->setDirection(bestDirection);
}

bool GhostManager::isValidMove(qreal nextX, qreal nextY, Ghost* ghost) const
{
	if (!ghost) {
		qDebug() << "Invalid Ghost Pointer";
		return false;
	}

	if (nextX < 0 && ghost->getCurrentDirection() == Direction::Left) {
		ghost->setPos(420, 260);
	}
	else if (nextX >= 420 && ghost->getCurrentDirection() == Direction::Right) {
		ghost->setDirection(Direction::Right);
		ghost->setPos(0, 260);
	}
	QList<QGraphicsItem*> collidingItems = scene->items(QPointF(nextX, nextY));

	for (QGraphicsItem* item : collidingItems) {
		if (item == ghost) {
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
void Ghost::setGhostManager(GhostManager* manager)
{
	ghostManager = manager;
}

void GhostManager::handlePlayerModeChange(PlayerMode newMode)
{
	if (newMode == PlayerMode::Energized) {
		startFrightenedMode();
	}
}
void GhostManager::disconnectUpdateSignal()
{
	player->stoptimer();
	movetimer->stop();
	timer->stop();
}
void GhostManager::restartGameAfterCollision()
{
	disconnectUpdateSignal();
	setInitialPositions();
	QTimer::singleShot(2000, this, &GhostManager::startGame);
}
void GhostManager::setInitialPositions()
{
	player->setPos(200, 320);
	for (int i = 0; i <= static_cast<int>(ghosts.size()); i++) {
		if (i == 0) {
			ghosts[i]->setPos(200, 220);
		}
		if (i == 1) {
			ghosts[i]->setPos(200, 280);
		}
		if (i == 2) {
			ghosts[i]->setPos(180, 280);
		}
		if (i == 3) {
			ghosts[i]->setPos(220, 280);
		}
	}
}
void GhostManager::startGame()
{
	qDebug() << "game started";
	currentMode = GhostManagerMode::Scatter;
	player->startimer();
	timer->start(0);
	movetimer->start(300);
}
void GhostManager::startDeadMode(Ghost* ghost)
{
	ghost->setCurrentMode(GhostMode::Dead);
	QPixmap frightenedPixmap("deadghost.png");
	ghost->QGraphicsPixmapItem::setPixmap(frightenedPixmap.scaled(20, 20));
}
void GhostManager::endDeadMode(Ghost* ghost)
{

	ghost->QGraphicsPixmapItem::setPixmap(ghost->regularPixmap.scaled(20, 20));
	ghost->setCurrentMode(GhostMode::Alive);
	currentMode = GhostManagerMode::Chase;
}

QTimer* GhostManager::getTimer() const
{
	return timer.get();
};

std::vector<Ghost*> GhostManager::getghosts()
{
	return ghosts;
}
Player* GhostManager::getPlayer()
{
	return player;
};

void GhostManager::stopTimer()
{
	timer->stop();
};
