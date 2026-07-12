#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSoundEffect>
#include <QSettings>
#include "game_engine.h"

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver
};

class GameWindow : public QWidget {
    Q_OBJECT

private:
    QTimer* gameTimer;
    GameEngine engine;
    GameState state;

    double turretAngle;
    bool keyLeft;
    bool keyRight;
    bool keySpace;
    double shootCooldown;

    int highScoreEasy;
    int highScoreNormal;
    int highScoreHard;

    int previousBaseHp;
    double painFaceTimer;

    QSoundEffect* sndShootMG;
    QSoundEffect* sndShootCannon;
    QSoundEffect* sndExplosion;
    QSoundEffect* sndBaseDamage;
    QSoundEffect* sndRage;
    QSoundEffect* sndPickup;
    QSoundEffect* sndClick;
    QSoundEffect* sndEnemyDeath;
    QSoundEffect* sndWaveComplete;

    QSoundEffect* sndMusicMenu;
    QSoundEffect* sndMusicGame;
    QSoundEffect* sndMusicPause;
    QSoundEffect* sndMusicGameOver;

    QPixmap texGround;
    QPixmap texBase;
    QPixmap texTurretNormal;
    QPixmap texTurretRage;
    QPixmap texMachineGunCold;
    QPixmap texMachineGunWarm;
    QPixmap texMachineGunOverheat;
    QPixmap texSoldier;
    QPixmap texKamikaze;
    QPixmap texTank;
    QPixmap texDrone;
    QPixmap texFaceNormal;
    QPixmap texFaceHurt;
    QPixmap texFaceCritical;
    QPixmap texFaceRage;

    QRect btnEasy;
    QRect btnNormal;
    QRect btnHard;
    QRect btnExitFromPause;
    QRect btnExitFromGameOver;

    void drawGround(QPainter& painter);
    void drawBaseAndTurret(QPainter& painter, double scaleX, double scaleY);
    void drawObjects(QPainter& painter);
    void drawHUD(QPainter& painter);
    void drawMainMenu(QPainter& painter);
    void drawPausedScreen(QPainter& painter);
    void drawGameOverScreen(QPainter& painter);

    bool drawButton(QPainter& painter, const QRect& rect, const QString& text, const QPoint& mousePos);
    void processEngineSounds();
    void loadHighScores();
    void saveHighScore(Difficulty diff, int score);
    void setGameState(GameState newState);

    QPoint lastMousePos;

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void changeEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void gameLoop();
};

#endif // GAMEWINDOW_H
