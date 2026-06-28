#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QByteArray>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSaveFile>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStyle>
#include <QTextStream>
#include <QVBoxLayout>

#include <algorithm>

namespace {
const QString kUsersHeader = QStringLiteral("SPYGAME_USERS_V1");
const QString kEncryptionKey = QStringLiteral("Team32SpyGameUsersKey");
const QString kManagerUsername = QStringLiteral("Team32");
const QString kManagerPassword = QStringLiteral("team32");

QString dialogStyle(const QString &accent)
{
    return QStringLiteral(R"(
QDialog {
    background-color: #0a1220;
    color: #f7efe3;
    font-family: "Avenir Next", "Helvetica Neue", sans-serif;
}
QFrame#dialogPanel {
    background-color: rgba(15, 23, 39, 228);
    border: 1px solid rgba(255, 255, 255, 24);
    border-radius: 22px;
}
QLabel#dialogTitle {
    color: #fff6e7;
    font-size: 24px;
    font-weight: 800;
}
QLabel#dialogCopy {
    color: rgba(238, 243, 251, 210);
    font-size: 14px;
    font-weight: 500;
}
QComboBox {
    background-color: rgba(8, 14, 24, 214);
    border: 1px solid rgba(255, 255, 255, 28);
    border-radius: 16px;
    color: #f8f4ed;
    font-size: 15px;
    font-weight: 700;
    padding: 10px 14px;
}
QPushButton {
    background-color: %1;
    border: 1px solid rgba(255, 255, 255, 48);
    border-radius: 16px;
    color: #102438;
    font-size: 14px;
    font-weight: 800;
    padding: 12px 18px;
}
QPushButton:hover {
    background-color: #fff7ef;
}
)")
        .arg(accent);
}

bool runStoryDialog(QWidget *parent, const QString &windowTitle, const QString &heading,
                    const QStringList &lines, const QString &buttonText,
                    const QString &accent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(windowTitle);
    dialog.setModal(true);
    dialog.setMinimumWidth(560);
    dialog.setStyleSheet(dialogStyle(accent));

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(18);

    auto *titleLabel = new QLabel(heading, &dialog);
    titleLabel->setObjectName("dialogTitle");
    titleLabel->setWordWrap(true);
    layout->addWidget(titleLabel);

    auto *panel = new QFrame(&dialog);
    panel->setObjectName("dialogPanel");
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(18, 18, 18, 18);
    panelLayout->setSpacing(10);

    for (const QString &line : lines) {
        auto *copyLabel = new QLabel(line.isEmpty() ? QStringLiteral(" ") : line, panel);
        copyLabel->setObjectName("dialogCopy");
        copyLabel->setWordWrap(true);
        panelLayout->addWidget(copyLabel);
    }

    layout->addWidget(panel);

    auto *actionButton = new QPushButton(buttonText, &dialog);
    QObject::connect(actionButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(actionButton);

    return dialog.exec() == QDialog::Accepted;
}

int runChoiceDialog(QWidget *parent, const QString &windowTitle, const QString &heading,
                    const QStringList &lines, const QStringList &choices,
                    const QString &buttonText, const QString &accent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(windowTitle);
    dialog.setModal(true);
    dialog.setMinimumWidth(560);
    dialog.setStyleSheet(dialogStyle(accent));

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(18);

    auto *titleLabel = new QLabel(heading, &dialog);
    titleLabel->setObjectName("dialogTitle");
    titleLabel->setWordWrap(true);
    layout->addWidget(titleLabel);

    auto *panel = new QFrame(&dialog);
    panel->setObjectName("dialogPanel");
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(18, 18, 18, 18);
    panelLayout->setSpacing(10);

    for (const QString &line : lines) {
        auto *copyLabel = new QLabel(line.isEmpty() ? QStringLiteral(" ") : line, panel);
        copyLabel->setObjectName("dialogCopy");
        copyLabel->setWordWrap(true);
        panelLayout->addWidget(copyLabel);
    }

    auto *comboBox = new QComboBox(panel);
    comboBox->addItems(choices);
    panelLayout->addWidget(comboBox);
    layout->addWidget(panel);

    auto *actionButton = new QPushButton(buttonText, &dialog);
    QObject::connect(actionButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(actionButton);

    return dialog.exec() == QDialog::Accepted ? comboBox->currentIndex() : -1;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resetLegacyStyles();
    configureWindow();
    applyTheme();
    buildMenuPage();
    buildSetupPage();
    buildAdminPage();
    buildResultsPage();

    ui->stackedWidget->setCurrentIndex(0);
    ui->resultsTextEdit->setReadOnly(true);
    ui->adminTextEdit->setReadOnly(true);
    ui->playersSpinBox->setAlignment(Qt::AlignCenter);
    ui->spiesSpinBox->setAlignment(Qt::AlignCenter);
    ui->playersTextEdit->setPlaceholderText("Lina\nOmar\nNour\nYoussef");
    ui->resultsTextEdit->setPlaceholderText("Saved archives and final leaderboards appear here.");
    ui->adminTextEdit->setPlaceholderText("Manager output appears here.");
    ui->spiesSpinBox->setMaximum(std::max(1, ui->playersSpinBox->value() - 1));

    connect(ui->playersSpinBox, qOverload<int>(&QSpinBox::valueChanged), this,
            [this](int value) {
                const int maxSpies = std::max(1, value - 1);
                ui->spiesSpinBox->setMaximum(maxSpies);
                if (ui->spiesSpinBox->value() > maxSpies) {
                    ui->spiesSpinBox->setValue(maxSpies);
                }
            });

    style()->unpolish(this);
    style()->polish(this);
    ensureUserStorage();
    resetRoundState();
    setBannerMessage(menuStatusLabel, "Choose how you want to enter the game.");
    setBannerMessage(setupStatusLabel, "Authenticate from the access screen before starting a round.");
    setBannerMessage(adminStatusLabel, "Manager authentication is required before using the control room.");
    setBannerMessage(resultsStatusLabel, "Saved mission archives and final leaderboards appear here.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetLegacyStyles()
{
    const QList<QWidget *> widgets = {
        ui->stackedWidget,
        ui->startGameButton,
        ui->exitButton,
        ui->viewResultsButton,
        ui->adminButton,
        ui->continueButton,
        ui->backButton,
        ui->playersSpinBox,
        ui->spiesSpinBox,
        ui->label
    };

    for (QWidget *widget : widgets) {
        widget->setStyleSheet({});
    }
}

void MainWindow::configureWindow()
{
    setWindowTitle("SpyGame GUI");
    resize(1240, 780);
    setMinimumSize(1240, 780);

    if (menuBar()) {
        menuBar()->hide();
    }

    if (statusBar()) {
        statusBar()->hide();
    }

    ui->stackedWidget->setGeometry(0, 0, 1240, 780);
}

void MainWindow::applyTheme()
{
    setStyleSheet(QStringLiteral(R"(
QMainWindow#MainWindow, QWidget#centralwidget {
    background-color: #060b14;
    color: #f6efe5;
    font-family: "Avenir Next", "Helvetica Neue", sans-serif;
}

QWidget#menuPage, QWidget#setupPage, QWidget#adminPage, QWidget#resultsPage {
    background-color: qradialgradient(cx: 0.18, cy: 0.12, radius: 1.08,
                                      fx: 0.22, fy: 0.06,
                                      stop: 0 #1b2740,
                                      stop: 0.38 #10182b,
                                      stop: 1 #050914);
}

QStackedWidget {
    background: transparent;
}

QFrame[role="hero"] {
    background-color: rgba(18, 25, 39, 224);
    border: 1px solid rgba(255, 255, 255, 28);
    border-radius: 30px;
}

QFrame[role="card"][accent="blue"] {
    background-color: rgba(19, 31, 56, 210);
    border: 1px solid rgba(151, 188, 255, 85);
    border-radius: 26px;
}

QFrame[role="card"][accent="teal"] {
    background-color: rgba(13, 39, 52, 214);
    border: 1px solid rgba(138, 227, 236, 88);
    border-radius: 26px;
}

QFrame[role="card"][accent="plum"] {
    background-color: rgba(44, 22, 45, 214);
    border: 1px solid rgba(222, 151, 214, 84);
    border-radius: 26px;
}

QFrame[role="panel"] {
    background-color: rgba(13, 20, 33, 220);
    border: 1px solid rgba(255, 255, 255, 22);
    border-radius: 26px;
}

QLabel {
    background: transparent;
}

QLabel[role="heroTitle"] {
    color: #fff6e7;
    font-size: 36px;
    font-weight: 800;
}

QLabel[role="heroSubtitle"] {
    color: rgba(238, 243, 251, 205);
    font-size: 15px;
    font-weight: 500;
}

QLabel[role="banner"] {
    background-color: rgba(104, 208, 255, 220);
    color: #102438;
    border-radius: 18px;
    padding: 0 18px;
    font-size: 14px;
    font-weight: 700;
}

QLabel[role="cardTitle"] {
    color: #fff3df;
    font-size: 18px;
    font-weight: 800;
}

QLabel[role="cardCopy"] {
    color: rgba(242, 246, 252, 210);
    font-size: 13px;
    font-weight: 600;
}

QLabel[role="fieldLabel"] {
    color: #f4ede3;
    font-size: 15px;
    font-weight: 700;
}

QPushButton {
    background-color: rgba(236, 228, 219, 230);
    border: 1px solid rgba(255, 255, 255, 48);
    border-radius: 18px;
    color: #152136;
    font-size: 14px;
    font-weight: 800;
    padding: 12px 18px;
}

QPushButton:hover {
    background-color: rgba(255, 247, 239, 245);
}

QPushButton:pressed {
    background-color: rgba(214, 207, 199, 240);
}

QPushButton#adminBackButton,
QPushButton#backButton,
QPushButton#setupBackButton {
    background-color: rgba(255, 255, 255, 18);
    border: 1px solid rgba(255, 255, 255, 34);
    color: #f2e8df;
}

QPushButton#adminBackButton:hover,
QPushButton#backButton:hover,
QPushButton#setupBackButton:hover {
    background-color: rgba(255, 255, 255, 28);
}

QTextEdit, QPlainTextEdit, QLineEdit {
    background-color: rgba(8, 14, 26, 198);
    border: 1px solid rgba(255, 255, 255, 25);
    border-radius: 18px;
    color: #f6f7fb;
    font-size: 15px;
    padding: 12px 16px;
    selection-background-color: rgba(104, 208, 255, 120);
}

QLineEdit::placeholder, QTextEdit::placeholder, QPlainTextEdit::placeholder {
    color: rgba(236, 241, 248, 150);
}

QSpinBox {
    background-color: rgba(8, 14, 24, 214);
    border: 1px solid rgba(255, 255, 255, 28);
    border-radius: 18px;
    color: #f8f4ed;
    font-size: 16px;
    font-weight: 700;
    padding: 10px 16px;
}

QSpinBox::up-button, QSpinBox::down-button {
    width: 24px;
    border: none;
    background: rgba(255, 255, 255, 14);
    margin: 4px;
    border-radius: 10px;
}

QSpinBox::up-button:hover, QSpinBox::down-button:hover {
    background: rgba(255, 255, 255, 22);
}
    )"));
}

void MainWindow::buildMenuPage()
{
    auto *hero = createSurface(ui->menuPage, QRect(58, 36, 1124, 122), "hero");
    auto *loginCard = createSurface(ui->menuPage, QRect(62, 250, 330, 356), "card", "blue");
    auto *registerCard = createSurface(ui->menuPage, QRect(455, 250, 330, 356), "card", "teal");
    auto *managerCard = createSurface(ui->menuPage, QRect(848, 250, 330, 356), "card", "plum");

    addGlow(hero, QColor(12, 18, 32, 145));
    addGlow(loginCard, QColor(50, 88, 170, 95));
    addGlow(registerCard, QColor(39, 130, 144, 90));
    addGlow(managerCard, QColor(126, 57, 108, 95));

    ui->label->setParent(hero);
    ui->label->setGeometry(34, 24, 860, 42);
    ui->label->setText("SpyGame Graphical Access Console");
    ui->label->setProperty("role", "heroTitle");

    createTextLabel(
        hero, QRect(34, 74, 1030, 28),
        "A cinematic graphical front end for your SpyGame project. Use the panels below to log in, register, manage users, or open the saved archive.",
        "heroSubtitle", true
    );

    menuStatusLabel = createTextLabel(ui->menuPage, QRect(62, 178, 1116, 42),
                                      QString(), "banner");

    createTextLabel(loginCard, QRect(24, 22, 220, 30), "Player Login", "cardTitle");
    createTextLabel(loginCard, QRect(24, 60, 276, 50),
                    "Use an existing player account and jump straight into round setup.",
                    "cardCopy", true);
    loginUsernameEdit = createLineEdit(loginCard, QRect(24, 132, 282, 48), "Username");
    loginPasswordEdit = createLineEdit(loginCard, QRect(24, 190, 282, 48), "Password", true);
    ui->viewResultsButton->setParent(loginCard);
    ui->viewResultsButton->setGeometry(24, 252, 282, 50);
    ui->viewResultsButton->setText("Login");

    createTextLabel(registerCard, QRect(24, 22, 220, 30), "Register", "cardTitle");
    createTextLabel(registerCard, QRect(24, 60, 276, 50),
                    "Create a new player profile for this graphical version of SpyGame.",
                    "cardCopy", true);
    registerUsernameEdit = createLineEdit(registerCard, QRect(24, 132, 282, 48), "New username");
    registerPasswordEdit = createLineEdit(registerCard, QRect(24, 190, 282, 48), "Password", true);
    registerConfirmPasswordEdit = createLineEdit(registerCard, QRect(24, 248, 282, 48),
                                                 "Confirm password", true);
    ui->startGameButton->setParent(registerCard);
    ui->startGameButton->setGeometry(24, 310, 282, 50);
    ui->startGameButton->setText("Create Account");

    createTextLabel(managerCard, QRect(24, 22, 220, 30), "Manager Access", "cardTitle");
    createTextLabel(managerCard, QRect(24, 60, 276, 50),
                    "Open the control room or browse the saved archive from the graphical project.",
                    "cardCopy", true);
    managerUsernameEdit = createLineEdit(managerCard, QRect(24, 132, 282, 48), "Manager username");
    managerPasswordEdit = createLineEdit(managerCard, QRect(24, 190, 282, 48), "Manager password", true);
    ui->adminButton->setParent(managerCard);
    ui->adminButton->setGeometry(24, 252, 282, 50);
    ui->adminButton->setText("Open Manager Console");
    ui->exitButton->setParent(managerCard);
    ui->exitButton->setGeometry(24, 312, 282, 44);
    ui->exitButton->setText("Open Saved Results");
}

void MainWindow::buildSetupPage()
{
    auto *hero = createSurface(ui->setupPage, QRect(58, 36, 1124, 118), "hero");
    auto *controlsPanel = createSurface(ui->setupPage, QRect(62, 232, 430, 432), "panel");
    auto *rosterPanel = createSurface(ui->setupPage, QRect(520, 232, 662, 432), "panel");

    addGlow(hero, QColor(10, 16, 28, 145));
    addGlow(controlsPanel, QColor(45, 98, 171, 70));
    addGlow(rosterPanel, QColor(36, 130, 150, 72));

    createTextLabel(hero, QRect(34, 22, 760, 36), "Game Setup", "heroTitle");
    createTextLabel(hero, QRect(34, 70, 980, 28),
                    "Build the table, register everyone for the round, and launch the private briefing sequence.",
                    "heroSubtitle", true);

    setupStatusLabel = createTextLabel(ui->setupPage, QRect(62, 178, 1116, 42),
                                       QString(), "banner");

    createTextLabel(controlsPanel, QRect(28, 26, 230, 28), "Round Configuration", "cardTitle");
    createTextLabel(controlsPanel, QRect(28, 60, 350, 38),
                    "Choose the player count and decide how many spies are hiding among the group.",
                    "cardCopy", true);

    ui->numofplayers->setParent(controlsPanel);
    ui->numofplayers->setGeometry(28, 122, 180, 22);
    ui->numofplayers->setProperty("role", "fieldLabel");
    ui->numofplayers->setText("Number of players");

    ui->playersSpinBox->setParent(controlsPanel);
    ui->playersSpinBox->setGeometry(28, 152, 160, 52);

    ui->numofspies->setParent(controlsPanel);
    ui->numofspies->setGeometry(28, 234, 180, 22);
    ui->numofspies->setProperty("role", "fieldLabel");
    ui->numofspies->setText("Number of spies");

    ui->spiesSpinBox->setParent(controlsPanel);
    ui->spiesSpinBox->setGeometry(28, 264, 160, 52);

    createTextLabel(controlsPanel, QRect(220, 150, 176, 44), "Minimum players: 3", "cardCopy");
    createTextLabel(controlsPanel, QRect(220, 206, 176, 44), "Spies must stay below the player count.", "cardCopy", true);
    createTextLabel(controlsPanel, QRect(220, 260, 176, 70),
                    "The bigger the table, the more chaotic and cinematic the round becomes.",
                    "cardCopy", true);

    ui->continueButton->setParent(controlsPanel);
    ui->continueButton->setGeometry(28, 352, 178, 52);
    ui->continueButton->setText("Begin Briefing");

    setupBackButton = new QPushButton("Return to Access Console", controlsPanel);
    setupBackButton->setObjectName("setupBackButton");
    setupBackButton->setGeometry(218, 352, 178, 52);
    connect(setupBackButton, &QPushButton::clicked, this, [this]() {
        returnToAccessConsole("You returned to the access console before starting the round.");
    });

    ui->label_2->setParent(rosterPanel);
    ui->label_2->setGeometry(28, 26, 220, 28);
    ui->label_2->setProperty("role", "cardTitle");
    ui->label_2->setText("Player Roster");

    createTextLabel(rosterPanel, QRect(28, 60, 560, 40),
                    "Create the cast for this round of deception. Add one player name per line.",
                    "cardCopy", true);

    ui->playersTextEdit->setParent(rosterPanel);
    ui->playersTextEdit->setGeometry(28, 118, 606, 286);
}

void MainWindow::buildAdminPage()
{
    auto *hero = createSurface(ui->adminPage, QRect(58, 36, 1124, 118), "hero");
    auto *toolsPanel = createSurface(ui->adminPage, QRect(62, 232, 340, 432), "card", "plum");
    auto *consolePanel = createSurface(ui->adminPage, QRect(430, 232, 752, 432), "panel");

    addGlow(hero, QColor(10, 16, 28, 145));
    addGlow(toolsPanel, QColor(129, 61, 109, 92));
    addGlow(consolePanel, QColor(42, 91, 164, 70));

    ui->label_3->setParent(hero);
    ui->label_3->setGeometry(34, 22, 620, 38);
    ui->label_3->setProperty("role", "heroTitle");
    ui->label_3->setText("Manager Panel");

    createTextLabel(hero, QRect(34, 70, 930, 28),
                    "Inspect and maintain the encrypted player vault from the graphical control room.",
                    "heroSubtitle", true);

    adminStatusLabel = createTextLabel(ui->adminPage, QRect(62, 178, 1116, 42),
                                       QString(), "banner");

    createTextLabel(toolsPanel, QRect(26, 24, 220, 28), "Control Room", "cardTitle");
    createTextLabel(toolsPanel, QRect(26, 60, 286, 40),
                    "Choose a manager action. The output appears in the console to the right.",
                    "cardCopy", true);

    ui->viewUsersButton->setParent(toolsPanel);
    ui->viewUsersButton->setGeometry(26, 124, 288, 48);
    ui->viewUsersButton->setText("View Decrypted Users");

    ui->viewResultsFileButton->setParent(toolsPanel);
    ui->viewResultsFileButton->setGeometry(26, 184, 288, 48);
    ui->viewResultsFileButton->setText("Edit Username");

    managerChangePasswordButton = new QPushButton("Change Password", toolsPanel);
    managerChangePasswordButton->setGeometry(26, 244, 288, 48);
    connect(managerChangePasswordButton, &QPushButton::clicked, this, [this]() {
        bool accepted = false;
        const QString username = QInputDialog::getText(
            this,
            "Edit Password",
            "Enter the username to update:",
            QLineEdit::Normal,
            QString(),
            &accepted
        ).trimmed();
        if (!accepted) {
            return;
        }

        const QString newPassword = QInputDialog::getText(
            this,
            "Edit Password",
            "Enter the new password:",
            QLineEdit::Password,
            QString(),
            &accepted
        );
        if (!accepted) {
            return;
        }

        const QString confirmPassword = QInputDialog::getText(
            this,
            "Edit Password",
            "Confirm the new password:",
            QLineEdit::Password,
            QString(),
            &accepted
        );
        if (!accepted) {
            return;
        }

        QString message;
        if (updatePassword(username, newPassword, confirmPassword, message)) {
            showAdminText("Password Updated", message);
            setBannerMessage(adminStatusLabel, "Password updated successfully.");
        } else {
            showAdminText("Password Update Failed", message);
            setBannerMessage(adminStatusLabel, message);
        }
    });

    ui->clearResultsButton->setParent(toolsPanel);
    ui->clearResultsButton->setGeometry(26, 304, 288, 48);
    ui->clearResultsButton->setText("Delete User");

    ui->adminBackButton->setParent(toolsPanel);
    ui->adminBackButton->setGeometry(26, 364, 288, 48);
    ui->adminBackButton->setText("Return to Access Console");

    createTextLabel(consolePanel, QRect(28, 24, 240, 28), "Manager Display", "cardTitle");
    createTextLabel(consolePanel, QRect(28, 60, 620, 40),
                    "View users, inspect changes, and verify manager actions from this console panel.",
                    "cardCopy", true);

    ui->adminTextEdit->setParent(consolePanel);
    ui->adminTextEdit->setGeometry(28, 114, 696, 290);
}

void MainWindow::buildResultsPage()
{
    auto *hero = createSurface(ui->resultsPage, QRect(58, 36, 1124, 118), "hero");
    auto *reportPanel = createSurface(ui->resultsPage, QRect(62, 232, 1120, 432), "panel");

    addGlow(hero, QColor(10, 16, 28, 145));
    addGlow(reportPanel, QColor(36, 99, 169, 74));

    ui->gameresult->setParent(hero);
    ui->gameresult->setGeometry(34, 22, 620, 38);
    ui->gameresult->setProperty("role", "heroTitle");
    ui->gameresult->setText("Mission Archive");

    resultsSubtitleLabel = createTextLabel(
        hero, QRect(34, 70, 960, 28),
        "Saved rounds and final leaderboards are preserved here for later bragging rights.",
        "heroSubtitle", true
    );

    resultsStatusLabel = createTextLabel(ui->resultsPage, QRect(62, 178, 1116, 42),
                                         QString(), "banner");

    createTextLabel(reportPanel, QRect(28, 24, 260, 28), "Archive Output", "cardTitle");
    createTextLabel(reportPanel, QRect(28, 60, 620, 40),
                    "The live round assignment, final leaderboard, or saved results file appears in the report deck below.",
                    "cardCopy", true);

    ui->resultsTextEdit->setParent(reportPanel);
    ui->resultsTextEdit->setGeometry(28, 114, 1064, 244);

    playAgainButton = new QPushButton("Play Another Round", reportPanel);
    playAgainButton->setGeometry(236, 372, 204, 48);
    playAgainButton->hide();
    connect(playAgainButton, &QPushButton::clicked, this, [this]() {
        resetRoundState();
        ui->playersTextEdit->clear();
        ui->resultsTextEdit->clear();
        ui->stackedWidget->setCurrentIndex(2);
        setBannerMessage(setupStatusLabel,
                         currentUsername.isEmpty()
                             ? "Authenticate from the access screen before starting a round."
                             : "Fresh round ready. Configure the table and launch the private briefing.");
    });

    ui->backButton->setParent(reportPanel);
    ui->backButton->setGeometry(28, 372, 190, 48);
    ui->backButton->setText("Return to Access Console");
}

QFrame *MainWindow::createSurface(QWidget *parent, const QRect &geometry, const QString &role,
                                  const QString &accent)
{
    auto *frame = new QFrame(parent);
    frame->setGeometry(geometry);
    frame->setFrameShape(QFrame::NoFrame);
    frame->setProperty("role", role);
    if (!accent.isEmpty()) {
        frame->setProperty("accent", accent);
    }
    return frame;
}

QLabel *MainWindow::createTextLabel(QWidget *parent, const QRect &geometry, const QString &text,
                                    const QString &role, bool wordWrap)
{
    auto *label = new QLabel(parent);
    label->setGeometry(geometry);
    label->setText(text);
    label->setProperty("role", role);
    label->setWordWrap(wordWrap);
    return label;
}

QLineEdit *MainWindow::createLineEdit(QWidget *parent, const QRect &geometry,
                                      const QString &placeholder, bool password)
{
    auto *edit = new QLineEdit(parent);
    edit->setGeometry(geometry);
    edit->setPlaceholderText(placeholder);
    edit->setClearButtonEnabled(!password);
    if (password) {
        edit->setEchoMode(QLineEdit::Password);
    }
    return edit;
}

void MainWindow::addGlow(QWidget *widget, const QColor &color)
{
    auto *effect = new QGraphicsDropShadowEffect(widget);
    effect->setBlurRadius(42);
    effect->setOffset(0, 18);
    effect->setColor(color);
    widget->setGraphicsEffect(effect);
}

void MainWindow::setBannerMessage(QLabel *label, const QString &text)
{
    if (label) {
        label->setText(text);
    }
}

void MainWindow::showResultsScreen(const QString &title, const QString &subtitle,
                                   const QString &banner, const QString &content,
                                   bool allowPlayAgain)
{
    ui->gameresult->setText(title);
    if (resultsSubtitleLabel) {
        resultsSubtitleLabel->setText(subtitle);
    }
    setBannerMessage(resultsStatusLabel, banner);
    ui->resultsTextEdit->setPlainText(content);
    if (playAgainButton) {
        playAgainButton->setVisible(allowPlayAgain);
    }
}

QString MainWindow::storageRootPath() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int depth = 0; depth < 6; ++depth) {
        if (dir.exists("Spy_game.pro")) {
            return dir.path();
        }
        if (!dir.cdUp()) {
            break;
        }
    }

    QString fallback = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir fallbackDir(fallback);
    fallbackDir.mkpath(".");
    return fallbackDir.path();
}

QString MainWindow::appDataFilePath(const QString &fileName) const
{
    QDir dir(storageRootPath());
    dir.mkpath(".");
    return dir.filePath(fileName);
}

QString MainWindow::readTextFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    QTextStream in(&file);
    return in.readAll();
}

QStringList MainWindow::loadWords() const
{
    QStringList words;
    QFile file(":/data/words.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return words;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString word = in.readLine().trimmed();
        if (!word.isEmpty()) {
            words.append(word);
        }
    }

    return words;
}

bool MainWindow::ensureUserStorage() const
{
    const QString path = appDataFilePath("users.txt");
    QFile file(path);

    if (!file.exists()) {
        return saveUsers({});
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    const QString rawContent = in.readAll();
    file.close();

    if (rawContent.trimmed().isEmpty()) {
        return saveUsers({});
    }

    QVector<UserRecord> users;
    if (parseEncryptedUsers(rawContent, users)) {
        return true;
    }

    if (parseLegacyUsers(rawContent, users)) {
        return saveUsers(users);
    }

    return false;
}

bool MainWindow::loadUsers(QVector<UserRecord> &users) const
{
    users.clear();
    if (!ensureUserStorage()) {
        return false;
    }

    QFile file(appDataFilePath("users.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    const QString rawContent = in.readAll();

    if (rawContent.trimmed().isEmpty()) {
        return true;
    }

    return parseEncryptedUsers(rawContent, users);
}

bool MainWindow::saveUsers(const QVector<UserRecord> &users) const
{
    QSaveFile file(appDataFilePath("users.txt"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    const QString serializedUsers = serializeUsers(users);
    const QByteArray encryptedUsers = xorTransform(serializedUsers.toUtf8());
    out << hexEncode(encryptedUsers);
    return file.commit();
}

bool MainWindow::parseEncryptedUsers(const QString &rawContent, QVector<UserRecord> &users) const
{
    QByteArray encryptedBytes;
    if (!hexDecode(rawContent, encryptedBytes)) {
        return false;
    }

    const QString decryptedContent = QString::fromUtf8(xorTransform(encryptedBytes));
    return deserializeUsers(decryptedContent, users, true);
}

bool MainWindow::parseLegacyUsers(const QString &rawContent, QVector<UserRecord> &users) const
{
    return deserializeUsers(rawContent, users, false);
}

bool MainWindow::deserializeUsers(const QString &plainContent, QVector<UserRecord> &users,
                                  bool expectHeader) const
{
    users.clear();
    QStringList lines = plainContent.split('\n');

    int startIndex = 0;
    if (expectHeader) {
        if (lines.isEmpty() || lines.first().trimmed() != kUsersHeader) {
            return false;
        }
        startIndex = 1;
    }

    for (int i = startIndex; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const int commaIndex = line.indexOf(',');
        if (commaIndex <= 0 || commaIndex >= line.size() - 1) {
            return false;
        }

        UserRecord record;
        record.username = line.left(commaIndex);
        record.passwordHash = line.mid(commaIndex + 1);

        if (record.username.isEmpty() || record.passwordHash.isEmpty()) {
            return false;
        }

        users.append(record);
    }

    return true;
}

QString MainWindow::serializeUsers(const QVector<UserRecord> &users) const
{
    QString serializedUsers = kUsersHeader + "\n";

    for (const UserRecord &user : users) {
        serializedUsers += user.username + "," + user.passwordHash + "\n";
    }

    return serializedUsers;
}

QString MainWindow::hashPassword(const QString &password) const
{
    quint64 hash = 5381;
    const QByteArray bytes = password.toUtf8();

    for (char byte : bytes) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(byte);
    }

    return QString::number(hash, 16);
}

QByteArray MainWindow::xorTransform(const QByteArray &input) const
{
    QByteArray output = input;
    const QByteArray key = kEncryptionKey.toUtf8();

    for (int i = 0; i < output.size(); ++i) {
        output[i] = static_cast<char>(output[i] ^ key[i % key.size()]);
    }

    return output;
}

QString MainWindow::hexEncode(const QByteArray &input) const
{
    return QString::fromLatin1(input.toHex().toUpper());
}

bool MainWindow::hexDecode(const QString &input, QByteArray &output) const
{
    QString cleanedInput;
    cleanedInput.reserve(input.size());

    for (const QChar character : input) {
        if (!character.isSpace()) {
            if (!character.isDigit() &&
                (character.toLower() < QChar('a') || character.toLower() > QChar('f'))) {
                return false;
            }
            cleanedInput.append(character);
        }
    }

    if (cleanedInput.size() % 2 != 0) {
        return false;
    }

    output = QByteArray::fromHex(cleanedInput.toLatin1());
    return output.size() * 2 == cleanedInput.size();
}

int MainWindow::findUserIndex(const QVector<UserRecord> &users, const QString &username) const
{
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].username == username) {
            return i;
        }
    }

    return -1;
}

bool MainWindow::userExists(const QString &username) const
{
    QVector<UserRecord> users;
    if (!loadUsers(users)) {
        return false;
    }

    return findUserIndex(users, username) != -1;
}

bool MainWindow::validateUserCredentials(const QString &username, const QString &password) const
{
    QVector<UserRecord> users;
    if (!loadUsers(users)) {
        return false;
    }

    const int userIndex = findUserIndex(users, username);
    if (userIndex == -1) {
        return false;
    }

    return users[userIndex].passwordHash == hashPassword(password);
}

bool MainWindow::createUserAccount(const QString &username, const QString &password,
                                   QString &errorMessage)
{
    if (username.isEmpty() || password.isEmpty()) {
        errorMessage = "Username and password cannot be empty.";
        return false;
    }

    if (username.contains(',')) {
        errorMessage = "Username cannot contain commas.";
        return false;
    }

    if (password.size() < 4) {
        errorMessage = "Password must be at least 4 characters.";
        return false;
    }

    QVector<UserRecord> users;
    if (!loadUsers(users)) {
        errorMessage = "Could not read users.txt safely.";
        return false;
    }

    if (findUserIndex(users, username) != -1) {
        errorMessage = "Username already taken. Please try a different one.";
        return false;
    }

    UserRecord newUser;
    newUser.username = username;
    newUser.passwordHash = hashPassword(password);
    users.append(newUser);

    if (!saveUsers(users)) {
        errorMessage = "Could not save the new user.";
        return false;
    }

    errorMessage = "Account created successfully.";
    return true;
}

bool MainWindow::updateUsername(const QString &currentUsernameValue, const QString &newUsername,
                                QString &message)
{
    QVector<UserRecord> users;
    if (!loadUsers(users)) {
        message = "Could not decrypt or read users.txt.";
        return false;
    }

    const int userIndex = findUserIndex(users, currentUsernameValue);
    if (userIndex == -1) {
        message = "User not found.";
        return false;
    }

    if (newUsername.isEmpty()) {
        message = "New username cannot be empty.";
        return false;
    }

    if (newUsername.contains(',')) {
        message = "Username cannot contain commas.";
        return false;
    }

    if (findUserIndex(users, newUsername) != -1) {
        message = "That username is already in use.";
        return false;
    }

    users[userIndex].username = newUsername;

    if (!saveUsers(users)) {
        message = "Failed to save updated users.";
        return false;
    }

    message = "Username updated successfully.";
    return true;
}

bool MainWindow::updatePassword(const QString &username, const QString &newPassword,
                                const QString &confirmPassword, QString &message)
{
    QVector<UserRecord> users;
    if (!loadUsers(users)) {
        message = "Could not decrypt or read users.txt.";
        return false;
    }

    const int userIndex = findUserIndex(users, username);
    if (userIndex == -1) {
        message = "User not found.";
        return false;
    }

    if (newPassword.size() < 4) {
        message = "Password must be at least 4 characters.";
        return false;
    }

    if (newPassword != confirmPassword) {
        message = "Passwords do not match.";
        return false;
    }

    users[userIndex].passwordHash = hashPassword(newPassword);

    if (!saveUsers(users)) {
        message = "Failed to save updated users.";
        return false;
    }

    message = "Password updated successfully.";
    return true;
}

bool MainWindow::deleteUserAccount(const QString &username, const QString &confirmation,
                                   QString &message)
{
    QVector<UserRecord> users;
    if (!loadUsers(users)) {
        message = "Could not decrypt or read users.txt.";
        return false;
    }

    const int userIndex = findUserIndex(users, username);
    if (userIndex == -1) {
        message = "User not found.";
        return false;
    }

    if (confirmation != "YES") {
        message = "Deletion cancelled.";
        return false;
    }

    users.removeAt(userIndex);

    if (!saveUsers(users)) {
        message = "Failed to save updated users.";
        return false;
    }

    message = "User deleted successfully.";
    return true;
}

bool MainWindow::authenticateManager(const QString &username, const QString &password) const
{
    return username == kManagerUsername && password == kManagerPassword;
}

void MainWindow::completeAuth(const QString &username, const QString &bannerMessage)
{
    currentUsername = username;
    loginAttempts = 0;
    resetRoundState();

    if (loginUsernameEdit) {
        loginUsernameEdit->clear();
    }
    if (loginPasswordEdit) {
        loginPasswordEdit->clear();
    }
    if (registerUsernameEdit) {
        registerUsernameEdit->clear();
    }
    if (registerPasswordEdit) {
        registerPasswordEdit->clear();
    }
    if (registerConfirmPasswordEdit) {
        registerConfirmPasswordEdit->clear();
    }

    ui->playersTextEdit->clear();
    ui->stackedWidget->setCurrentIndex(2);
    setBannerMessage(setupStatusLabel, bannerMessage);
}

void MainWindow::returnToAccessConsole(const QString &bannerMessage)
{
    currentUsername.clear();
    managerAttempts = 0;
    loginAttempts = 0;
    resetRoundState();

    if (managerUsernameEdit) {
        managerUsernameEdit->clear();
    }
    if (managerPasswordEdit) {
        managerPasswordEdit->clear();
    }

    ui->stackedWidget->setCurrentIndex(0);
    setBannerMessage(menuStatusLabel, bannerMessage);
}

void MainWindow::openSavedArchive(const QString &bannerMessage)
{
    const QString archiveContent = readTextFile(appDataFilePath("results.txt"));
    const QString displayContent = archiveContent.trimmed().isEmpty()
        ? "No saved results were found yet."
        : archiveContent.trimmed();

    showResultsScreen(
        "Mission Archive",
        "Every completed round is preserved here for later bragging rights.",
        bannerMessage,
        displayContent,
        false
    );
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::showAdminText(const QString &title, const QString &content)
{
    const QString body = content.trimmed().isEmpty()
        ? QStringLiteral("No data available yet.")
        : content.trimmed();
    ui->adminTextEdit->setPlainText(title + "\n\n" + body);
}

void MainWindow::resetRoundState()
{
    secretWord.clear();
    roundPlayerNames.clear();
    roundPlayerRoles.clear();
    spyIndices.clear();
    roundPoints.clear();
    if (playAgainButton) {
        playAgainButton->hide();
    }
}

void MainWindow::assignSpies(int spiesCount)
{
    roundPlayerRoles.clear();
    for (int i = 0; i < roundPlayerNames.size(); ++i) {
        roundPlayerRoles.append(QStringLiteral("Normal"));
    }
    spyIndices.clear();

    while (spyIndices.size() < spiesCount) {
        const int randomIndex = QRandomGenerator::global()->bounded(roundPlayerNames.size());
        if (!spyIndices.contains(randomIndex)) {
            spyIndices.append(randomIndex);
            roundPlayerRoles[randomIndex] = QStringLiteral("Spy");
        }
    }
}

bool MainWindow::runPrivateReveals()
{
    if (!runStoryDialog(
            this,
            "Private Briefing",
            "Role Reveal",
            {
                "Pass the screen to one player at a time and keep every reveal secret.",
                "If you are a civilian, protect the word and find the spy.",
                "If you are the spy, blend in and stay hidden."
            },
            "Begin Private Reveals",
            "#e8d2a0")) {
        return false;
    }

    for (int i = 0; i < roundPlayerNames.size(); ++i) {
        if (!runStoryDialog(
                this,
                "Secure Handoff",
                "Player Turn",
                {
                    "Only this player should be looking at the screen.",
                    "Player: " + roundPlayerNames[i],
                    "Press continue when the room is ready for the private reveal."
                },
                "Reveal Briefing",
                "#9edcff")) {
            return false;
        }

        QStringList lines;
        lines << ("Player: " + roundPlayerNames[i]) << "";

        if (roundPlayerRoles[i] == "Spy") {
            lines << "[ SPY ]"
                  << "Blend in, improvise convincingly, and avoid drawing suspicion.";
            if (!runStoryDialog(this, "Classified Role", "Spy Briefing", lines,
                                "Lock Screen", "#ffb1aa")) {
                return false;
            }
        } else {
            lines << "[ CIVILIAN ]"
                  << ("Secret word: " + secretWord)
                  << "Protect the word and expose the spy before they escape.";
            if (!runStoryDialog(this, "Classified Role", "Civilian Briefing", lines,
                                "Lock Screen", "#9edcff")) {
                return false;
            }
        }
    }

    return runStoryDialog(
        this,
        "Everyone Is Ready",
        "Accusation Phase",
        {
            "All roles have been revealed.",
            "The room is now live. Watch every clue, every hesitation, and every accusation."
        },
        "Start Guessing",
        "#b7f0c6");
}

bool MainWindow::runGuessingPhase()
{
    if (!runStoryDialog(
            this,
            "Accusation Phase",
            "Lock Your Guesses",
            {
                "Each player will privately accuse the person they believe is the spy.",
                "Every player who correctly names a spy earns one point.",
                "If nobody identifies a spy, the spy team earns the point instead."
            },
            "Begin Guessing",
            "#e8d2a0")) {
        return false;
    }

    QStringList choices;
    for (int i = 0; i < roundPlayerNames.size(); ++i) {
        choices << QString("%1. %2").arg(i + 1).arg(roundPlayerNames[i]);
    }

    QVector<int> guessedPlayers;
    int correctGuessCount = 0;

    for (int i = 0; i < roundPlayerNames.size(); ++i) {
        const int guess = runChoiceDialog(
            this,
            "Private Turn",
            "Make Your Accusation",
            {
                roundPlayerNames[i] + ", trust your instincts and pick the most suspicious player.",
                "Choose one name from the suspect board below."
            },
            choices,
            "Lock Guess",
            "#9edcff"
        );

        if (guess < 0) {
            return false;
        }

        guessedPlayers.append(guess);
        if (spyIndices.contains(guess)) {
            ++correctGuessCount;
        }

        if (!runStoryDialog(
                this,
                "Guess Locked",
                "Private Guess Saved",
                {
                    roundPlayerNames[i] + "'s accusation has been recorded.",
                    "Pass the screen to the next player."
                },
                "Continue",
                "#f3d0e6")) {
            return false;
        }
    }

    awardPoints(guessedPlayers);

    if (correctGuessCount > 0) {
        return runStoryDialog(
            this,
            "Round Outcome",
            "Spy Identified",
            {
                QString("%1 player(s) correctly identified a spy and earned a point.")
                    .arg(correctGuessCount),
                "The undercover identities are about to be revealed."
            },
            "Reveal the Spies",
            "#b7f0c6");
    }

    return runStoryDialog(
        this,
        "Round Outcome",
        "Spy Team Escaped",
        {
            "Nobody uncovered a spy this round.",
            "The undercover team stayed hidden and claimed the point."
        },
        "Reveal the Spies",
        "#ffb1aa");
}

void MainWindow::awardPoints(const QVector<int> &guessedPlayers)
{
    roundPoints = QVector<int>(roundPlayerNames.size(), 0);
    bool someoneGuessedSpy = false;

    for (int guess : guessedPlayers) {
        if (spyIndices.contains(guess)) {
            someoneGuessedSpy = true;
            break;
        }
    }

    if (someoneGuessedSpy) {
        for (int i = 0; i < guessedPlayers.size(); ++i) {
            if (spyIndices.contains(guessedPlayers[i])) {
                roundPoints[i]++;
            }
        }
    } else {
        for (int spyIndex : spyIndices) {
            roundPoints[spyIndex]++;
        }
    }
}

void MainWindow::saveFinalResults()
{
    QFile file(appDataFilePath("results.txt"));
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << "========================================\n";
    out << "Secret Word: " << secretWord << "\n";
    out << "Spies: ";

    if (spyIndices.isEmpty()) {
        out << "None";
    } else {
        QStringList spyNames;
        for (int spyIndex : spyIndices) {
            spyNames << roundPlayerNames[spyIndex];
        }
        out << spyNames.join(", ");
    }

    out << "\n";
    out << "Final Results:\n";

    for (int i = 0; i < roundPlayerNames.size(); ++i) {
        out << i + 1 << ". "
            << roundPlayerNames[i]
            << " | Role: " << roundPlayerRoles[i]
            << " | Points: " << roundPoints.value(i) << "\n";
    }

    out << "========================================\n\n";
}

QString MainWindow::buildSpyRevealText() const
{
    QStringList lines;
    lines << ("Secret Word: " + secretWord) << "";

    for (int spyIndex : spyIndices) {
        lines << QString("Player %1 - %2 was a spy.")
                     .arg(spyIndex + 1)
                     .arg(roundPlayerNames[spyIndex]);
    }

    return lines.join('\n');
}

QString MainWindow::buildFinalResultsText() const
{
    QVector<QPair<QString, int>> scores;
    for (int i = 0; i < roundPlayerNames.size(); ++i) {
        scores.append(qMakePair(roundPlayerNames[i], roundPoints.value(i)));
    }

    std::stable_sort(scores.begin(), scores.end(),
                     [](const QPair<QString, int> &left, const QPair<QString, int> &right) {
                         if (left.second != right.second) {
                             return left.second > right.second;
                         }
                         return left.first < right.first;
                     });

    QStringList lines;
    if (scores.isEmpty()) {
        lines << "No scores have been recorded yet.";
        return lines.join('\n');
    }

    for (int i = 0; i < scores.size(); ++i) {
        QString pointsLabel = QString("%1 point%2")
                                  .arg(scores[i].second)
                                  .arg(scores[i].second == 1 ? "" : "s");

        QString line = QString("%1. %2").arg(i + 1).arg(scores[i].first);
        if (line.size() < 28) {
            line += QString(28 - line.size(), ' ');
        }

        line += "  " + pointsLabel;
        if (i == 0) {
            line += "  [TOP AGENT]";
        }

        lines << line;
    }

    return lines.join('\n');
}

void MainWindow::on_continueButton_clicked()
{
    if (currentUsername.isEmpty()) {
        returnToAccessConsole("Authenticate from the access screen before starting a round.");
        return;
    }

    const int players = ui->playersSpinBox->value();
    const int spies = ui->spiesSpinBox->value();

    QStringList names = ui->playersTextEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    for (QString &name : names) {
        name = name.trimmed();
    }
    names.erase(std::remove_if(names.begin(), names.end(),
                               [](const QString &value) { return value.isEmpty(); }),
                names.end());

    if (spies >= players || names.isEmpty()) {
        setBannerMessage(setupStatusLabel, "Check the player count, spy count, and roster before continuing.");
        return;
    }

    if (names.size() != players) {
        setBannerMessage(setupStatusLabel, "The number of names must exactly match the player count.");
        return;
    }

    const QStringList words = loadWords();
    if (words.isEmpty()) {
        showResultsScreen(
            "Mission Archive",
            "The round could not be prepared because the bundled word bank is unavailable.",
            "No secret word was loaded. Please check words.txt.",
            "Error: No secret word was loaded. Please check words.txt.",
            false
        );
        ui->stackedWidget->setCurrentIndex(3);
        return;
    }

    resetRoundState();
    roundPlayerNames = names;
    secretWord = words[QRandomGenerator::global()->bounded(words.size())];
    assignSpies(spies);

    if (!runPrivateReveals()) {
        setBannerMessage(setupStatusLabel, "Private briefing cancelled. The round has not been scored.");
        return;
    }

    if (!runGuessingPhase()) {
        setBannerMessage(setupStatusLabel, "Accusation phase cancelled. The round has not been scored.");
        return;
    }

    runStoryDialog(
        this,
        "Truth Time",
        "Spy Reveal",
        buildSpyRevealText().split('\n'),
        "Open Final Standings",
        "#ffb1aa"
    );

    saveFinalResults();
    showResultsScreen(
        "Final Standings",
        "Here is how the round scored after every bluff and accusation.",
        "Final results were saved to results.txt.",
        buildFinalResultsText(),
        true
    );
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_viewResultsButton_clicked()
{
    const QString username = loginUsernameEdit ? loginUsernameEdit->text().trimmed() : QString();
    const QString password = loginPasswordEdit ? loginPasswordEdit->text() : QString();

    if (username.isEmpty() || password.isEmpty()) {
        setBannerMessage(menuStatusLabel, "Username and password are required for login.");
        return;
    }

    if (validateUserCredentials(username, password)) {
        completeAuth(username, "Login successful. Welcome back, " + username + ".");
        return;
    }

    ++loginAttempts;
    if (loginAttempts < 3) {
        setBannerMessage(
            menuStatusLabel,
            QString("Invalid username or password. Try again (%1 attempt(s) left).")
                .arg(3 - loginAttempts)
        );
    } else {
        setBannerMessage(menuStatusLabel, "Too many failed login attempts.");
        loginAttempts = 0;
        if (loginPasswordEdit) {
            loginPasswordEdit->clear();
        }
    }
}

void MainWindow::on_startGameButton_clicked()
{
    const QString username = registerUsernameEdit ? registerUsernameEdit->text().trimmed() : QString();
    const QString password = registerPasswordEdit ? registerPasswordEdit->text() : QString();
    const QString confirmPassword = registerConfirmPasswordEdit ? registerConfirmPasswordEdit->text() : QString();

    if (password != confirmPassword) {
        setBannerMessage(menuStatusLabel, "Passwords do not match.");
        return;
    }

    QString message;
    if (createUserAccount(username, password, message)) {
        completeAuth(username, "Account created successfully. Welcome, " + username + ".");
    } else {
        setBannerMessage(menuStatusLabel, message);
    }
}

void MainWindow::on_exitButton_clicked()
{
    openSavedArchive("Opening the saved mission archive.");
}

void MainWindow::on_backButton_clicked()
{
    returnToAccessConsole("The round is complete. Choose how you want to enter the game.");
}

void MainWindow::on_adminButton_clicked()
{
    const QString username = managerUsernameEdit ? managerUsernameEdit->text().trimmed() : QString();
    const QString password = managerPasswordEdit ? managerPasswordEdit->text() : QString();

    if (username.isEmpty() || password.isEmpty()) {
        setBannerMessage(menuStatusLabel, "Manager username and password are required.");
        return;
    }

    if (authenticateManager(username, password)) {
        managerAttempts = 0;
        if (managerUsernameEdit) {
            managerUsernameEdit->clear();
        }
        if (managerPasswordEdit) {
            managerPasswordEdit->clear();
        }
        showAdminText("Manager Console",
                      "Manager access granted.\n\nChoose an admin action from the left.");
        setBannerMessage(adminStatusLabel, "Manager access granted.");
        ui->stackedWidget->setCurrentIndex(1);
        return;
    }

    ++managerAttempts;
    if (managerAttempts < 3) {
        setBannerMessage(
            menuStatusLabel,
            QString("Invalid manager credentials. Try again (%1 attempt(s) left).")
                .arg(3 - managerAttempts)
        );
    } else {
        setBannerMessage(menuStatusLabel, "Too many failed manager attempts.");
        managerAttempts = 0;
        if (managerPasswordEdit) {
            managerPasswordEdit->clear();
        }
    }
}

void MainWindow::on_adminBackButton_clicked()
{
    returnToAccessConsole("You returned to the access console.");
}

void MainWindow::on_viewUsersButton_clicked()
{
    QVector<UserRecord> users;
    if (!loadUsers(users)) {
        showAdminText("Decrypted Users", "Could not decrypt or read users.txt.");
        setBannerMessage(adminStatusLabel, "Could not decrypt or read users.txt.");
        return;
    }

    if (users.isEmpty()) {
        showAdminText("Decrypted Users", "No users were found in the encrypted storage.");
        setBannerMessage(adminStatusLabel, "No users were found in the encrypted storage.");
        return;
    }

    QStringList lines;
    for (int i = 0; i < users.size(); ++i) {
        lines << QString("%1. Username: %2 | Password Hash: %3")
                     .arg(i + 1)
                     .arg(users[i].username)
                     .arg(users[i].passwordHash);
    }

    showAdminText("Decrypted Users", lines.join('\n'));
    setBannerMessage(adminStatusLabel, "Decrypted users loaded into the manager display.");
}

void MainWindow::on_viewResultsFileButton_clicked()
{
    bool accepted = false;
    const QString currentName = QInputDialog::getText(
        this,
        "Edit Username",
        "Enter the current username:",
        QLineEdit::Normal,
        QString(),
        &accepted
    ).trimmed();

    if (!accepted) {
        return;
    }

    const QString newName = QInputDialog::getText(
        this,
        "Edit Username",
        "Enter the new username:",
        QLineEdit::Normal,
        QString(),
        &accepted
    ).trimmed();

    if (!accepted) {
        return;
    }

    QString message;
    if (updateUsername(currentName, newName, message)) {
        showAdminText("Username Updated", message);
        setBannerMessage(adminStatusLabel, "Username updated successfully.");
    } else {
        showAdminText("Username Update Failed", message);
        setBannerMessage(adminStatusLabel, message);
    }
}

void MainWindow::on_clearResultsButton_clicked()
{
    bool accepted = false;
    const QString username = QInputDialog::getText(
        this,
        "Delete User",
        "Enter the username to delete:",
        QLineEdit::Normal,
        QString(),
        &accepted
    ).trimmed();

    if (!accepted) {
        return;
    }

    const QString confirmation = QInputDialog::getText(
        this,
        "Delete User",
        "Type YES to confirm deletion:",
        QLineEdit::Normal,
        QString(),
        &accepted
    );

    if (!accepted) {
        return;
    }

    QString message;
    if (deleteUserAccount(username, confirmation, message)) {
        showAdminText("User Deleted", message);
        setBannerMessage(adminStatusLabel, "User deleted successfully.");
    } else {
        showAdminText("User Deletion Failed", message);
        setBannerMessage(adminStatusLabel, message);
    }
}
