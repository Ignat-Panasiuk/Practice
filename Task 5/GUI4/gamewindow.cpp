#include "gamewindow.h"
#include <QTime>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent), state(GameState::MainMenu), turretAngle(0.0),
    keyLeft(false), keyRight(false), keySpace(false), shootCooldown(0.0),
    highScoreEasy(0), highScoreNormal(0), highScoreHard(0), previousBaseHp(100), painFaceTimer(0.0)
{
    setMinimumSize(800, 700);
    setWindowTitle("DOOM: Base Defender");
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    setStyleSheet("background-color: black;");

    std::srand(QTime::currentTime().msec());

    btnEasy = QRect(100, 320, 180, 50);
    btnNormal = QRect(310, 320, 180, 50);
    btnHard = QRect(520, 320, 180, 50);
    btnExitFromPause = QRect(250, 380, 300, 50);
    btnExitFromGameOver = QRect(250, 540, 300, 50);

    texGround.load(":/ground.png");
    texBase.load(":/base.png");
    texTurretNormal.load(":/turret.png");
    texTurretRage.load(":/turret_rage.png");
    texMachineGunCold.load(":/machine_gun.png");
    texMachineGunWarm.load(":/machine_gun_warm.png");
    texMachineGunOverheat.load(":/machine_gun_overheat.png");
    texSoldier.load(":/soldier.png");
    texKamikaze.load(":/kamikaze.png");
    texTank.load(":/tank.png");
    texDrone.load(":/drone.png");
    texFaceNormal.load(":/face_normal.png");
    texFaceHurt.load(":/face_hurt.png");
    texFaceCritical.load(":/face_critical.png");
    texFaceRage.load(":/face_rage.png");

    sndShootMG = new QSoundEffect(this);
    sndShootMG->setSource(QUrl("qrc:/dspistol.wav"));
    sndShootMG->setVolume(0.4);

    sndShootCannon = new QSoundEffect(this);
    sndShootCannon->setSource(QUrl("qrc:/dsrlaunc.wav"));
    sndShootCannon->setVolume(0.9);

    sndExplosion = new QSoundEffect(this);
    sndExplosion->setSource(QUrl("qrc:/dsbarexp.wav"));
    sndExplosion->setVolume(0.85);

    sndBaseDamage = new QSoundEffect(this);
    sndBaseDamage->setSource(QUrl("qrc:/dsplpain.wav"));

    sndRage = new QSoundEffect(this);
    sndRage->setSource(QUrl("qrc:/dsgetpow.wav"));

    sndPickup = new QSoundEffect(this);
    sndPickup->setSource(QUrl("qrc:/dsitemup.wav"));

    sndClick = new QSoundEffect(this);
    sndClick->setSource(QUrl("qrc:/dsswtchn.wav"));

    sndEnemyDeath = new QSoundEffect(this);
    sndEnemyDeath->setSource(QUrl("qrc:/dsgldth.wav"));
    sndEnemyDeath->setVolume(0.65);

    sndWaveComplete = new QSoundEffect(this);
    sndWaveComplete->setSource(QUrl("qrc:/dstelept.wav"));

    sndMusicMenu = new QSoundEffect(this);
    sndMusicMenu->setSource(QUrl("qrc:/music_menu.wav"));
    sndMusicMenu->setLoopCount(QSoundEffect::Infinite);
    sndMusicMenu->setVolume(0.35);

    sndMusicGame = new QSoundEffect(this);
    sndMusicGame->setSource(QUrl("qrc:/music_game.wav"));
    sndMusicGame->setLoopCount(QSoundEffect::Infinite);
    sndMusicGame->setVolume(0.4);

    sndMusicPause = new QSoundEffect(this);
    sndMusicPause->setSource(QUrl("qrc:/music_pause.wav"));
    sndMusicPause->setLoopCount(QSoundEffect::Infinite);
    sndMusicPause->setVolume(0.47);

    sndMusicGameOver = new QSoundEffect(this);
    sndMusicGameOver->setSource(QUrl("qrc:/music_gameover.wav"));
    sndMusicGameOver->setLoopCount(QSoundEffect::Infinite);
    sndMusicGameOver->setVolume(0.5);

    loadHighScores();
    setGameState(GameState::MainMenu);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameWindow::gameLoop);
    gameTimer->start(1000 / 60);
}

GameWindow::~GameWindow() {
    delete gameTimer;
    if (state == GameState::Playing || state == GameState::Paused) {
        int currentHigh = (engine.difficulty == Difficulty::Easy) ? highScoreEasy : ((engine.difficulty == Difficulty::Normal) ? highScoreNormal : highScoreHard);
        if (engine.score > currentHigh) {
            saveHighScore(engine.difficulty, engine.score);
        }
    }
}

void GameWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (!isFullScreen()) {
            setWindowState(Qt::WindowFullScreen);
        }
    }
    QWidget::changeEvent(event);
}

void GameWindow::setGameState(GameState newState) {
    state = newState;

    sndMusicMenu->stop();
    sndMusicGame->stop();
    sndMusicPause->stop();
    sndMusicGameOver->stop();

    if (state == GameState::MainMenu) {
        sndMusicMenu->play();
    } else if (state == GameState::Playing) {
        if (engine.isRageActive) {
            sndMusicMenu->setVolume(1.0);
            sndMusicMenu->play();
        } else {
            sndMusicGame->play();
        }
    } else if (state == GameState::Paused) {
        sndMusicPause->play();
    } else if (state == GameState::GameOver) {
        sndMusicGameOver->play();
    }
}

void GameWindow::loadHighScores() {
    QSettings settings("BelarusUniversity", "DoomBaseDefense");
    highScoreEasy = settings.value("highscore_easy", 0).toInt();
    highScoreNormal = settings.value("highscore_normal", 0).toInt();
    highScoreHard = settings.value("highscore_hard", 0).toInt();
}

void GameWindow::saveHighScore(Difficulty diff, int score) {
    QSettings settings("BelarusUniversity", "DoomBaseDefense");
    if (diff == Difficulty::Easy) {
        highScoreEasy = score; settings.setValue("highscore_easy", score);
    } else if (diff == Difficulty::Normal) {
        highScoreNormal = score; settings.setValue("highscore_normal", score);
    } else {
        highScoreHard = score; settings.setValue("highscore_hard", score);
    }
}

void GameWindow::processEngineSounds() {
    if (engine.soundTriggerShootMG)     { sndShootMG->play();     engine.soundTriggerShootMG = false; }
    if (engine.soundTriggerShootCannon) { sndShootCannon->play(); engine.soundTriggerShootCannon = false; }
    if (engine.soundTriggerExplosion)   { sndExplosion->play();   engine.soundTriggerExplosion = false; }
    if (engine.soundTriggerBaseDamage)  { sndBaseDamage->play();  engine.soundTriggerBaseDamage = false; }
    if (engine.soundTriggerPickup)      { sndPickup->play();      engine.soundTriggerPickup = false; }
    if (engine.soundTriggerEnemyDeath)  { sndEnemyDeath->play();  engine.soundTriggerEnemyDeath = false; }
    if (engine.soundTriggerWaveComplete){ sndWaveComplete->play(); engine.soundTriggerWaveComplete = false; }

    if (engine.soundTriggerRage) {
        sndRage->play();
        engine.soundTriggerRage = false;
        setGameState(GameState::Playing);
    }
}

void GameWindow::gameLoop() {
    double deltaTime = 1.0 / 60.0;

    if (state == GameState::Playing) {
        if (keyLeft) turretAngle -= 3.5;
        if (keyRight) turretAngle += 3.5;

        if (turretAngle < 0) turretAngle += 360.0;
        if (turretAngle >= 360) turretAngle -= 360.0;
        if (keySpace) {
            shootCooldown += deltaTime;
            double fireInterval = (engine.activeWeapon == WeaponType::MachineGun) ? 0.08 : (engine.isRageActive ? 0.15 : 1.0);
            if (shootCooldown >= fireInterval) {
                engine.fireWeapon(turretAngle);
                shootCooldown = 0.0;
            }
        } else {
            shootCooldown = 1.0;
        }

        if (engine.baseHp < previousBaseHp) {
            painFaceTimer = 0.4;
        }
        previousBaseHp = engine.baseHp;

        if (painFaceTimer > 0.0) {
            painFaceTimer -= deltaTime;
        }

        bool wasRage = engine.isRageActive;
        engine.update(deltaTime);
        if (wasRage && !engine.isRageActive) {
            sndMusicMenu->setVolume(0.35);
            setGameState(GameState::Playing);
        }

        processEngineSounds();

        if (engine.baseHp <= 0) {
            setGameState(GameState::GameOver);
            int currentHigh = (engine.difficulty == Difficulty::Easy) ? highScoreEasy : ((engine.difficulty == Difficulty::Normal) ? highScoreNormal : highScoreHard);
            if (engine.score > currentHigh) {
                saveHighScore(engine.difficulty, engine.score);
            }
        }
    }
    update();
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (!texGround.isNull()) {
        QPixmap scaledGround = texGround.scaled(96, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        int tileW = scaledGround.width();
        int tileH = scaledGround.height();
        for (int x = 0; x < width(); x += tileW) {
            for (int y = 0; y < height(); y += tileH) {
                painter.drawPixmap(x, y, scaledGround);
            }
        }
    }

    double scaleX = static_cast<double>(width()) / 800.0;
    double scaleY = static_cast<double>(height()) / 700.0;
    double scale = std::min(scaleX, scaleY);

    double offsetX = (width() - 800.0 * scale) / 2.0;
    double offsetY = (height() - 700.0 * scale) / 2.0;

    double hudTopPhysical = height() - 100.0 * scale;
    painter.fillRect(0, hudTopPhysical, width(), 100.0 * scale, QColor(15, 15, 15));
    painter.setPen(QPen(QColor(80, 80, 80), 3 * scale));
    painter.drawLine(0, hudTopPhysical, width(), hudTopPhysical);

    if (state == GameState::Paused) {
        painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 180));
    }

    painter.save();
    painter.scale(scaleX, scaleY);

    switch (state) {
    case GameState::MainMenu:   drawMainMenu(painter); break;
    case GameState::Playing:
        painter.save();
        painter.setClipRect(0, 0, 800, 600);

        drawBaseAndTurret(painter, scaleX, scaleY);

        drawObjects(painter);

        painter.restore();
        drawHUD(painter);
        break;
    case GameState::Paused:
        painter.save();
        painter.setClipRect(0, 0, 800, 600);

        drawBaseAndTurret(painter, scaleX, scaleY);

        drawObjects(painter);

        painter.restore();
        drawHUD(painter);
        drawPausedScreen(painter);
        break;
    case GameState::GameOver:   drawGameOverScreen(painter); break;
    }
    painter.restore();
}

void GameWindow::drawGround(QPainter&) {

}

void GameWindow::drawObjects(QPainter& painter) {
    for (auto& obj : engine.objects) {
        painter.save();
        painter.translate(obj->getX(), obj->getY());

        double angleRad = std::atan2(engine.BASE_Y - obj->getY(), engine.BASE_X - obj->getX());
        double angleDeg = angleRad * 180.0 / M_PI;
        painter.rotate(angleDeg);

        QPixmap sprite;
        double targetWidth = 40.0;
        double targetHeight = 40.0;

        switch (obj->getType()) {
        case ObjectType::Soldier:
            sprite = texSoldier; targetWidth = 56.0; targetHeight = 56.0; break;
        case ObjectType::Kamikaze:
            sprite = texKamikaze; targetWidth = 56.0; targetHeight = 56.0; break;
        case ObjectType::Tank:
            sprite = texTank; targetWidth = 96.0; targetHeight = 96.0; break;
        case ObjectType::Drone:
            sprite = texDrone; targetWidth = 72.0; targetHeight = 72.0; break;
        case ObjectType::Projectile:
            painter.restore();
            painter.setBrush(QBrush(QColor(255, 200, 0)));
            painter.setPen(QPen(QColor(255, 100, 0), 2));
            painter.drawEllipse(QPointF(obj->getX(), obj->getY()), 4, 4);
            continue;
        case ObjectType::EnemyProjectile:
            painter.restore();
            painter.setBrush(QBrush(Qt::red));
            painter.setPen(QPen(QColor(150, 0, 0), 2));
            painter.drawEllipse(QPointF(obj->getX(), obj->getY()), 6, 6);
            continue;
        }

        double scaleY = 1.0 - (obj->getAnimOffset() / 40.0);
        double scaleX = 1.0 + (obj->getAnimOffset() / 40.0);
        painter.scale(scaleX, scaleY);

        painter.drawPixmap(-targetWidth / 2, -targetHeight / 2, targetWidth, targetHeight, sprite);
        painter.restore();
    }
}

void GameWindow::drawBaseAndTurret(QPainter& painter, double scaleX, double scaleY) {
    painter.drawPixmap(engine.BASE_X - 88, engine.BASE_Y - 65, 176, 130, texBase);

    double physX = engine.BASE_X * scaleX;
    double physY = engine.BASE_Y * scaleY;
    double uniformScale = std::min(scaleX, scaleY);

    painter.save();

    painter.resetTransform();

    painter.translate(physX, physY);

    painter.scale(uniformScale, uniformScale);

    painter.rotate(turretAngle);

    QPixmap activeTurret;
    if (engine.activeWeapon == WeaponType::MachineGun) {
        if (engine.isOverheated)            activeTurret = texMachineGunOverheat;
        else if (engine.gunHeat > 50.0)     activeTurret = texMachineGunWarm;
        else                                activeTurret = texMachineGunCold;
    } else {
        activeTurret = engine.isRageActive ? texTurretRage : texTurretNormal;
    }

    painter.drawPixmap(-52, -57, 110, 110, activeTurret);

    painter.restore();
}

void GameWindow::drawHUD(QPainter& painter) {
    painter.save();


    painter.setPen(QPen(QColor(80, 80, 80), 2));

    if (engine.isRageActive) {
        painter.setBrush(QBrush(QColor(150, 0, 0)));
    } else if (engine.baseHp < 30) {
        painter.setBrush(QBrush(QColor(255, 120, 0)));
    } else {
        painter.setBrush(QBrush(QColor(180, 140, 0)));
    }
    painter.drawRect(359, 610, 82, 81);

    QPixmap activeFace = texFaceNormal;
    if (engine.isRageActive) {
        activeFace = texFaceRage;
    } else if (painFaceTimer > 0.0) {
        activeFace = texFaceHurt;
    } else if (engine.baseHp < 30) {
        activeFace = texFaceCritical;
    } else {
        activeFace = texFaceNormal;
    }

    double aspect = static_cast<double>(activeFace.width()) / activeFace.height();
    int targetFaceWidth = static_cast<int>(80.0 * aspect);
    painter.drawPixmap(400 - targetFaceWidth / 2, 612, targetFaceWidth, 78, activeFace);

    painter.setPen(QColor(0, 200, 0));
    painter.setFont(QFont("Courier New", 14, QFont::Bold));

    painter.drawText(20, 640, QString("HP: %1%").arg(engine.baseHp));

    QString ammoStr = (engine.activeWeapon == WeaponType::HeavyCannon && !engine.isRageActive) ? QString::number(engine.cannonAmmo) : "INF";
    painter.drawText(20, 670, QString("AMMO: %1").arg(ammoStr));

    if (engine.activeWeapon == WeaponType::MachineGun) {
        painter.setFont(QFont("Courier New", 11, QFont::Bold));
        painter.drawText(150, 670, "HEAT:");
        painter.fillRect(220, 660, 80, 12, Qt::black);
        if (engine.gunHeat > 0) {
            QColor heatColor = engine.isOverheated ? Qt::red : (engine.gunHeat > 50 ? QColor(255, 140, 0) : Qt::yellow);
            painter.fillRect(220, 660, static_cast<int>(engine.gunHeat * 0.8), 12, heatColor);
        }
    }

    painter.setFont(QFont("Courier New", 11, QFont::Bold));
    painter.drawText(150, 630, "RAGE METER:");
    painter.fillRect(260, 620, 80, 12, Qt::black);
    if (engine.isRageActive) {
        int rageWidth = static_cast<int>((engine.rageTimer / 6.0) * 80.0);
        painter.fillRect(260, 620, rageWidth, 12, Qt::red);
    } else if (engine.rageMeter > 0) {
        int rageWidth = static_cast<int>((engine.rageMeter / 100.0) * 80.0);
        painter.fillRect(260, 620, rageWidth, 12, engine.rageMeter >= 100 ? Qt::red : Qt::yellow);
    }
    if (engine.rageMeter >= 100 && !engine.isRageActive) {
        painter.setPen(Qt::red);
        painter.drawText(150, 650, "READY! [R]");
    }

    painter.setPen(QColor(0, 200, 0));
    painter.setFont(QFont("Courier New", 14, QFont::Bold));
    painter.drawText(470, 640, QString("SCORE: %1").arg(engine.score));
    painter.drawText(470, 670, QString("WAVE:  %1").arg(engine.currentWave));

    QString diffStr = (engine.difficulty == Difficulty::Easy) ? "EASY" : ((engine.difficulty == Difficulty::Normal) ? "NORMAL" : "HARD");
    painter.drawText(650, 640, QString("DIFF:  %1").arg(diffStr));

    if (engine.waveDelayTimer > 0.0) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Courier New", 20, QFont::Bold));
        painter.drawText(QRect(0, 150, 800, 100), Qt::AlignCenter,
                         QString("WAVE %1 INCOMING! PREPARE...").arg(engine.currentWave));
    }

    painter.restore();
}

bool GameWindow::drawButton(QPainter& painter, const QRect& rect, const QString& text, const QPoint& mousePos) {
    bool hovered = rect.contains(mousePos);
    QColor baseColor = hovered ? QColor(0, 255, 0) : QColor(0, 150, 0);
    painter.setBrush(QBrush(QColor(20, 20, 20, 200)));
    painter.setPen(QPen(baseColor, 3));
    painter.drawRoundedRect(rect, 8, 8);

    painter.setPen(baseColor);
    painter.setFont(QFont("Courier New", 14, QFont::Bold));
    painter.drawText(rect, Qt::AlignCenter, text);
    return hovered;
}

void GameWindow::drawMainMenu(QPainter& painter) {
    painter.fillRect(QRect(0, 0, 800, 700), QColor(10, 10, 10));

    painter.setPen(QColor(0, 200, 0));
    painter.setFont(QFont("Courier New", 32, QFont::Bold));
    painter.drawText(QRect(0, 0, 800, 700), Qt::AlignHCenter | Qt::AlignTop, "\nDOOM: BASE DEFENDER");

    painter.setPen(QColor(0, 150, 0));
    painter.setFont(QFont("Courier New", 12, QFont::Bold));
    painter.drawText(QRect(0, 160, 800, 50), Qt::AlignCenter,
                     QString("HIGH RECORDS - EASY: %1 | NORMAL: %2 | HARD: %3")
                         .arg(highScoreEasy).arg(highScoreNormal).arg(highScoreHard));

    painter.drawText(QRect(0, 240, 800, 50), Qt::AlignCenter, "CHOOSE DIFFICULTY TO START:");

    drawButton(painter, btnEasy, "EASY", lastMousePos);
    drawButton(painter, btnNormal, "NORMAL", lastMousePos);
    drawButton(painter, btnHard, "HARD", lastMousePos);
    painter.setPen(QColor(0, 150, 0));
    painter.setFont(QFont("Courier New", 12, QFont::Bold));
    painter.drawText(QRect(0, 480, 800, 100), Qt::AlignCenter, "CONTROLS:\n[A]/[D] - Rotate Turret   [1]/[2] - Switch Weapons\n[Space] - Shoot   [R] - Active Rage   [ESC] - Pause");

}

void GameWindow::drawPausedScreen(QPainter& painter) {
    painter.fillRect(QRect(-1000, -1000, 3000, 3000), QColor(0, 0, 0, 180));
    painter.setPen(QColor(0, 200, 0));
    painter.setFont(QFont("Courier New", 24, QFont::Bold));
    painter.drawText(QRect(0, 100, 800, 150), Qt::AlignCenter, "DEFENSE PAUSED\n\nPRESS ESC TO RESUME");

    drawButton(painter, btnExitFromPause, "EXIT TO MENU", lastMousePos);
}

void GameWindow::drawGameOverScreen(QPainter& painter) {
    painter.fillRect(QRect(-1000, -1000, 3000, 3000), QColor(25, 0, 0));

    painter.setPen(Qt::red);
    painter.setFont(QFont("Courier New", 36, QFont::Bold));
    painter.drawText(QRect(0, 100, 800, 200), Qt::AlignCenter, "BASE DESTROYED\n\nGAME OVER");

    painter.setPen(QColor(255, 140, 0));
    painter.setFont(QFont("Courier New", 18, QFont::Bold));
    painter.drawText(QRect(0, 320, 800, 100), Qt::AlignCenter, QString("FINAL SCORE: %1").arg(engine.score));

    drawButton(painter, btnExitFromGameOver, "RETURN TO MENU", lastMousePos);
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    if (state == GameState::Playing) {
        if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left)  keyLeft = true;
        if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) keyRight = true;

        if (event->key() == Qt::Key_1) { engine.activeWeapon = WeaponType::MachineGun; sndClick->play(); }
        if (event->key() == Qt::Key_2) { engine.activeWeapon = WeaponType::HeavyCannon; sndClick->play(); }
        if (event->key() == Qt::Key_R) engine.activateRage();

        if (event->key() == Qt::Key_Space) keySpace = true;

        if (event->key() == Qt::Key_Escape) {
            setGameState(GameState::Paused);
            sndClick->play();
        }
    } else if (state == GameState::Paused) {
        if (event->key() == Qt::Key_Escape) {
            setGameState(GameState::Playing);
            sndClick->play();
        }
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left)  keyLeft = false;
    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) keyRight = false;
    if (event->key() == Qt::Key_Space) keySpace = false;
}

void GameWindow::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);

    QPoint clickPos = lastMousePos;

    if (state == GameState::MainMenu) {
        if (btnEasy.contains(clickPos)) {
            sndClick->play();
            engine.startNewGame(Difficulty::Easy);
            setGameState(GameState::Playing);
            setFocus();
        } else if (btnNormal.contains(clickPos)) {
            sndClick->play();
            engine.startNewGame(Difficulty::Normal);
            setGameState(GameState::Playing);
            setFocus();
        } else if (btnHard.contains(clickPos)) {
            sndClick->play();
            engine.startNewGame(Difficulty::Hard);
            setGameState(GameState::Playing);
            setFocus();
        }
    } else if (state == GameState::Paused) {
        if (btnExitFromPause.contains(clickPos)) {
            sndClick->play();
            setGameState(GameState::MainMenu);
        }
    } else if (state == GameState::GameOver) {
        if (btnExitFromGameOver.contains(clickPos)) {
            sndClick->play();
            setGameState(GameState::MainMenu);
        }
    }
}

void GameWindow::mouseMoveEvent(QMouseEvent *event) {
    double scaleX = static_cast<double>(width()) / 800.0;
    double scaleY = static_cast<double>(height()) / 700.0;
    lastMousePos = QPoint(static_cast<int>(event->x() / scaleX),
                          static_cast<int>(event->y() / scaleY));
}
