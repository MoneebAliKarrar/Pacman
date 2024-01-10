#pragma once
#include "Direction.h"
#include "collectible.h"
#include <QGraphicsEllipseItem>
#include <QObject>
#include <QTimer>
class GhostManager;
enum class PlayerMode { Normal, Energized };
class Player : public QObject, public QGraphicsEllipseItem {
	Q_OBJECT

  public:
	Direction direction;
	Player(QGraphicsScene* scene);
	void setDirection(Direction newDirection);
	bool isValidMoveForDirection(Direction newDirection);
	void setCenterPos(qreal centerX, qreal centerY);
	void checkCollectibleCollisions();
	QPointF getCurrentTilePosition() const;
	void setCurrentMode(PlayerMode mode);
	int livesCount;
	void disconnectUpdateSignal();
	void handleSpecialCollectibleCollision();
	void stoptimer();
	void startimer();
	PlayerMode getCurrentMode();
	void setGhostManager(GhostManager* manager);
	void handleGhostCollision();

  public slots:
	void updatePosition();
	void changeMouthShape();
	void endEnergizedMode();

  private:
	QGraphicsEllipseItem* ellipseItem;
	std::unique_ptr<QTimer> timer;
	bool isValidMove(qreal nextx, qreal nexty);
	int currentMouthShape;
	QVector<QPair<int, int>> mouthShapes;
	std::unique_ptr<QTimer> energizedTimer;
	PlayerMode currentMode;
	GhostManager* ghostManager;
	bool isGhostCollision();

  signals:
	void playerModeChanged(PlayerMode newMode);
	void collectibleCollected(CollectibleItem::CollectibleType itemType);
	void pacmandead();
	void ghostCollision();
};
