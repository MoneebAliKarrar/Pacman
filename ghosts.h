#pragma once
#include "Direction.h"
#include "player.h"
#include <QApplication>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QObject>
#include <QTimer>
class GhostManager;
class Player;
enum GhostManagerMode { Scatter, Chase, Frightened };
enum GhostMode { Alive, Dead };
class Ghost : public QObject, public QGraphicsPixmapItem {
	Q_OBJECT

  public:
	Ghost(const QPixmap& pixmap, QGraphicsScene* scene,
		  const QPointF& initialPosition, QObject* parent = 0);
	virtual ~Ghost() = default;
	QPointF getCurrentTilePosition() const;
	void move();
	Direction getCurrentDirection();
	void setDirection(Direction direction);
	virtual QPoint selectTargetTile() = 0;
	void setGhostManager(GhostManager* manager);
	QPixmap regularPixmap;
	GhostMode getCurrentMode() const;
	void setCurrentMode(GhostMode mode);

  protected:
	GhostManager* ghostManager;

  private:
	Direction currentDirecion;
	GhostMode currentMode;
};


class GhostManager : public QObject {

	Q_OBJECT

  public:
	GhostManager(QObject* parent, QGraphicsScene* scene,
				 const std::vector<Ghost*> allGhosts, Player* pacman);
	bool isModeSwitchTimerExpired();
	Direction calculateBestDirection(const QPointF& currentTile,
									 const QPointF& targetTile,
									 Ghost* ghost) const;
	bool isValidMove(qreal nextX, qreal nextY, Ghost* ghost) const;
	void disconnectUpdateSignal();
	void restartGameAfterCollision();
	void setInitialPositions();
	void startGame();
	QTimer* getTimer() const;
	std::vector<Ghost*> getghosts();
	GhostManagerMode getCurrentMode();
	int getWaveCount();
	void makeDirectionDecision(Ghost* ghost, const QPointF& nextTile);
	void startDeadMode(Ghost* ghost);
	void endDeadMode(Ghost* ghost);
	Player* getPlayer();
	void stopTimer();
	void reverseDirection();
	void handleReverseDirection();

  public slots:
	void startWave();
	void handleModeChange();
	void startFrightenedMode();
	void endFrightenedMode();
	void moveAllGhosts();
	void handlePlayerModeChange(PlayerMode newMode);

  private:
	std::unique_ptr<QTimer> timer;
	std::unique_ptr<QTimer> movetimer;
	GhostManagerMode currentMode;
	int waveCount;
	QGraphicsScene* scene;
	const std::vector<Ghost*> ghosts;
	Player* player;
};
