#pragma once
#include "Blinky.h"
#include "Clyde.h"
#include "Inky.h"
#include "Pinky.h"
#include "collectible.h"
#include "player.h"
#include "wallItem.h"
#include <QGraphicsScene>
#include <QLabel>
#include <QMainWindow>
class GameWindow : public QMainWindow {
	Q_OBJECT

  public:
	GameWindow(QWidget* parent = 0);
	~GameWindow();
	void drawCollectibles();
	std::vector<Ghost*> ghosts;
	Player* getPlayer() const;
	void updateScoreDisplay();
	void gameOverScreen();
	void winScreen();

  public slots:
	void handleCollectibleCollected(CollectibleItem::CollectibleType itemType);
	void drawPacmanLives(int livesCount);
	void handlePacmanKilledForLives();

  private:
	std::unique_ptr<QGraphicsScene> scene;
	Player* player;
	void setupMaze(const QString& file);
	std::vector<std::unique_ptr<WallItem>> walls;
	std::vector<std::unique_ptr<QGraphicsRectItem>> emptySpaces;
	std::vector<std::pair<int, int>> emptySpacePositions;
	std::vector<std::unique_ptr<CollectibleItem>> collectibles;
	GhostManager* ghostManager;
	int score;
	QLabel* scoreLabel;
	std::vector<std::unique_ptr<QGraphicsEllipseItem>> Lives;
	int collectibleCounter;

  protected:
	void keyPressEvent(QKeyEvent* event) override;
};
