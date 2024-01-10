#include "gamewindow.h"
#include <QBrush>
#include <QDebug>
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPen>
#include <QTextStream>
#include <random>
GameWindow::GameWindow(QWidget* parent) : QMainWindow(parent), score(0)
{
	setWindowTitle("Pac-Man");
	setFixedSize(425, 560);

	scene = std::make_unique<QGraphicsScene>(this);

	scene->setBackgroundBrush(QBrush(QColor("#000000")));
	std::unique_ptr<QGraphicsView> graphicsView =
		std::make_unique<QGraphicsView>(scene.get());
	graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	graphicsView->setFixedSize(425, 560);

	setCentralWidget(graphicsView.release());

	setupMaze("Maze.txt");
	//player
	player = new Player(scene.get());
	player->setObjectName("pacman_player");
	connect(player, &Player::collectibleCollected, this,
			&GameWindow::handleCollectibleCollected);
	//lives
	drawPacmanLives(player->livesCount);
	// score
	scoreLabel = new QLabel(this);
	// ghosts

	QPixmap blinkyPixmap("Blinky.png");
	ghosts.push_back(new Blinky(blinkyPixmap, scene.get(), QPointF(200, 220)));
	QPixmap pinkyPixmap("Pinky.png");
	ghosts.push_back(new Pinky(pinkyPixmap, scene.get(), QPointF(200, 280)));
	QPixmap inkyPixmap("Inky.png");
	ghosts.push_back(new Inky(inkyPixmap, scene.get(), QPointF(180, 280)));
	QPixmap clydePixmap("Clyde.png");
	ghosts.push_back(new Clyde(clydePixmap, scene.get(), QPointF(220, 280)));
	ghostManager = new GhostManager(this, scene.get(), ghosts, player);
	for (auto& ghost : ghosts) {
		ghost->setGhostManager(ghostManager);
		connect(ghostManager->getTimer(), &QTimer::timeout, ghost,
				&Ghost::move);
	}
	connect(player, &Player::ghostCollision, this,
			&GameWindow::handlePacmanKilledForLives);
	connect(player, &Player::pacmandead, this, &GameWindow::gameOverScreen);
	player->setGhostManager(ghostManager);
	//
	collectibleCounter = 213;
}
void GameWindow::setupMaze(const QString& filename)
{
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning("Couldn't open maze file.");
		return;
	}

	QTextStream in(&file);
	int row = 0;

	while (!in.atEnd()) {
		QString line = in.readLine();
		for (int col = 0; col < line.length(); ++col) {
			if (line.at(col) == '1') {
				std::unique_ptr<WallItem> wall =
					std::make_unique<WallItem>(col * 20, row * 20, 20, 20);
				scene->addItem(wall.get());
				walls.push_back(std::move(wall));
			}
			else if (line.at(col) == '0') {
				std::unique_ptr<QGraphicsRectItem> emptySpace =
					std::make_unique<QGraphicsRectItem>(col * 20, row * 20, 20,
														20);
				QBrush brush(QColor("#000000"));
				QPen pen(QColor("#000000"));
				emptySpace->setBrush(brush);
				emptySpace->setPen(pen);
				scene->addItem(emptySpace.get());
				emptySpaces.push_back(std::move(emptySpace));
				emptySpacePositions.emplace_back(row, col);
			}
			else if (line.at(col) == '3') {
				std::unique_ptr<QGraphicsRectItem> emptySpace =
					std::make_unique<QGraphicsRectItem>(col * 20, row * 20, 20,
														20);
				QBrush brush(QColor("#000000"));
				QPen pen(QColor("#000000"));
				emptySpace->setBrush(brush);
				emptySpace->setPen(pen);
				scene->addItem(emptySpace.get());
				emptySpaces.push_back(std::move(emptySpace));
			}
		}

		++row;
	}
	drawCollectibles();
	file.close();
}
void GameWindow::keyPressEvent(QKeyEvent* event)
{
	Direction newDirection = player->direction;
	switch (event->key()) {
	case 87:
		newDirection = Direction::Up;
		break;
	case 83:
		newDirection = Direction::Down;
		break;
	case 65:
		newDirection = Direction::Left;
		break;
	case 68:
		newDirection = Direction::Right;
		break;
	default:
		QMainWindow::keyPressEvent(event);
		break;
	}
	if (player->isValidMoveForDirection(newDirection)) {
		player->setDirection(newDirection);
	}
}
GameWindow::~GameWindow()
{
	walls.clear();
	emptySpaces.clear();
	emptySpacePositions.clear();
	collectibles.clear();
	for (auto* ghost : ghosts) {
		delete ghost;
	}
	ghosts.clear();
	delete ghostManager;
	delete player;
	delete scoreLabel;
}
void GameWindow::drawCollectibles()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(emptySpacePositions.begin(), emptySpacePositions.end(), gen);
	int numSpecialPositions =
		std::min(4, static_cast<int>(emptySpacePositions.size()));
	std::vector<std::pair<int, int>> specialPositions(
		emptySpacePositions.begin(),
		emptySpacePositions.begin() + numSpecialPositions);

	for (int i = 0; i < static_cast<int>(emptySpacePositions.size()); ++i) {
		int row = emptySpacePositions[i].first;
		int col = emptySpacePositions[i].second;

		bool isSpecialPosition =
			std::find(specialPositions.begin(), specialPositions.end(),
					  std::make_pair(row, col)) != specialPositions.end();

		if (!isSpecialPosition) {
			std::unique_ptr<CollectibleItem> simpleCollectible =
				std::make_unique<CollectibleItem>(col * 20 + 5, row * 20 + 5, 7,
												  7, CollectibleItem::Simple);
			scene->addItem(simpleCollectible.get());
			collectibles.push_back(std::move(simpleCollectible));
		}
		else {
			std::unique_ptr<CollectibleItem> specialCollectible =
				std::make_unique<CollectibleItem>(col * 20 + 5, row * 20 + 5,
												  10, 10,
												  CollectibleItem::Special);
			scene->addItem(specialCollectible.get());
			collectibles.push_back(std::move(specialCollectible));
		}
	}
}
void GameWindow::handleCollectibleCollected(
	CollectibleItem::CollectibleType itemType)
{
	if (itemType == CollectibleItem::Simple) {
		score += 1;
		updateScoreDisplay();
	}
	else if (itemType == CollectibleItem::Special) {
		score += 5;
	}
	if(--collectibleCounter == 0){
	winScreen();
	}
}
void GameWindow::updateScoreDisplay()
{
	scoreLabel->setText("<font color='white'>Score: " + QString::number(score) +
						"</font>");
	scoreLabel->setGeometry(350, 520, 100, 30);
	scoreLabel->show();
}
void GameWindow::gameOverScreen()
{
	player->disconnectUpdateSignal();
	ghostManager->disconnectUpdateSignal();
	ghostManager->stopTimer();

	QLabel* gameOverLabel = new QLabel(this);
	gameOverLabel->setText("<font color='white'>Game Over!</font>");
	gameOverLabel->setAlignment(Qt::AlignCenter);
	gameOverLabel->setGeometry(100, 285, 200, 70);
	gameOverLabel->show();

	QTimer::singleShot(2000, [=]() {
		gameOverLabel->hide();
		gameOverLabel->deleteLater();
		qApp->quit();
	});
}
void GameWindow::drawPacmanLives(int livesCount)
{
	Lives.clear();
	qreal pacmanSize = 20.0;
	qreal spacing = 5.0;
	for (int i = 0; i < livesCount; ++i) {
		QPoint lifePosition(i * (pacmanSize + spacing), 530);
		std::unique_ptr<QGraphicsEllipseItem> pacmanLife =
			std::make_unique<QGraphicsEllipseItem>(
				lifePosition.x(), lifePosition.y(), pacmanSize, pacmanSize);
		pacmanLife->setBrush(Qt::yellow);
		pacmanLife->setData(Qt::UserRole, 1);
		scene->addItem(pacmanLife.get());
		Lives.push_back(std::move(pacmanLife));
	}
}
void GameWindow::handlePacmanKilledForLives()
{
	if (!Lives.empty()) {
		std::unique_ptr<QGraphicsEllipseItem> lastLife =
			std::move(Lives.back());
		Lives.pop_back();
		scene->removeItem(lastLife.get());
	}
}
Player* GameWindow::getPlayer() const
{
	return player;
}
void GameWindow::winScreen()
{
	player->disconnectUpdateSignal();
	ghostManager->disconnectUpdateSignal();
	ghostManager->stopTimer();

	QLabel* gameOverLabel = new QLabel(this);
	gameOverLabel->setText("<font color='white'>You won!</font>");
	gameOverLabel->setAlignment(Qt::AlignCenter);
	gameOverLabel->setGeometry(100, 285, 200, 70);
	gameOverLabel->show();

	QTimer::singleShot(2000, [=]() {
		gameOverLabel->hide();
		gameOverLabel->deleteLater();
		qApp->quit();
	});
}
