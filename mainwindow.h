#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QByteArray>
#include <QColor>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_continueButton_clicked();
    void on_viewResultsButton_clicked();
    void on_startGameButton_clicked();
    void on_exitButton_clicked();
    void on_backButton_clicked();
    void on_adminButton_clicked();
    void on_adminBackButton_clicked();
    void on_viewUsersButton_clicked();
    void on_viewResultsFileButton_clicked();
    void on_clearResultsButton_clicked();

private:
    struct UserRecord {
        QString username;
        QString passwordHash;
    };

    void resetLegacyStyles();
    void configureWindow();
    void applyTheme();
    void buildMenuPage();
    void buildSetupPage();
    void buildAdminPage();
    void buildResultsPage();
    QFrame *createSurface(QWidget *parent, const QRect &geometry, const QString &role,
                          const QString &accent = {});
    QLabel *createTextLabel(QWidget *parent, const QRect &geometry, const QString &text,
                            const QString &role, bool wordWrap = false);
    QLineEdit *createLineEdit(QWidget *parent, const QRect &geometry, const QString &placeholder,
                              bool password = false);
    void addGlow(QWidget *widget, const QColor &color);
    void setBannerMessage(QLabel *label, const QString &text);
    void showResultsScreen(const QString &title, const QString &subtitle,
                           const QString &banner, const QString &content,
                           bool allowPlayAgain);

    QString storageRootPath() const;
    QString appDataFilePath(const QString &fileName) const;
    QString readTextFile(const QString &filePath) const;
    QStringList loadWords() const;
    bool ensureUserStorage() const;
    bool loadUsers(QVector<UserRecord> &users) const;
    bool saveUsers(const QVector<UserRecord> &users) const;
    bool parseEncryptedUsers(const QString &rawContent, QVector<UserRecord> &users) const;
    bool parseLegacyUsers(const QString &rawContent, QVector<UserRecord> &users) const;
    bool deserializeUsers(const QString &plainContent, QVector<UserRecord> &users,
                          bool expectHeader) const;
    QString serializeUsers(const QVector<UserRecord> &users) const;
    QString hashPassword(const QString &password) const;
    QByteArray xorTransform(const QByteArray &input) const;
    QString hexEncode(const QByteArray &input) const;
    bool hexDecode(const QString &input, QByteArray &output) const;
    int findUserIndex(const QVector<UserRecord> &users, const QString &username) const;
    bool userExists(const QString &username) const;
    bool validateUserCredentials(const QString &username, const QString &password) const;
    bool createUserAccount(const QString &username, const QString &password, QString &errorMessage);
    bool updateUsername(const QString &currentUsernameValue, const QString &newUsername,
                        QString &message);
    bool updatePassword(const QString &username, const QString &newPassword,
                        const QString &confirmPassword, QString &message);
    bool deleteUserAccount(const QString &username, const QString &confirmation,
                           QString &message);
    bool authenticateManager(const QString &username, const QString &password) const;
    void completeAuth(const QString &username, const QString &bannerMessage);
    void returnToAccessConsole(const QString &bannerMessage);
    void openSavedArchive(const QString &bannerMessage);
    void showAdminText(const QString &title, const QString &content);

    void resetRoundState();
    void assignSpies(int spiesCount);
    bool runPrivateReveals();
    bool runGuessingPhase();
    void awardPoints(const QVector<int> &guessedPlayers);
    void saveFinalResults();
    QString buildSpyRevealText() const;
    QString buildFinalResultsText() const;

    QLabel *menuStatusLabel = nullptr;
    QLabel *setupStatusLabel = nullptr;
    QLabel *adminStatusLabel = nullptr;
    QLabel *resultsStatusLabel = nullptr;
    QLabel *resultsSubtitleLabel = nullptr;

    QLineEdit *loginUsernameEdit = nullptr;
    QLineEdit *loginPasswordEdit = nullptr;
    QLineEdit *registerUsernameEdit = nullptr;
    QLineEdit *registerPasswordEdit = nullptr;
    QLineEdit *registerConfirmPasswordEdit = nullptr;
    QLineEdit *managerUsernameEdit = nullptr;
    QLineEdit *managerPasswordEdit = nullptr;

    QPushButton *setupBackButton = nullptr;
    QPushButton *managerChangePasswordButton = nullptr;
    QPushButton *playAgainButton = nullptr;

    QString currentUsername;
    QString secretWord;
    QStringList roundPlayerNames;
    QStringList roundPlayerRoles;
    QVector<int> spyIndices;
    QVector<int> roundPoints;

    int loginAttempts = 0;
    int managerAttempts = 0;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
