#include <QGuiApplication>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QApplication>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMenu>
#include <QUrl>

#include <QQuickView>

#include "Settings.h"



class SingleInstanceGuard {
public:
    explicit SingleInstanceGuard(const QString& key)
        : m_shared(key)
    {
        // 尝试创建1字节的共享内存
        if (!m_shared.create(1)) {
            if (m_shared.error() == QSharedMemory::AlreadyExists) {
                m_isRunning = true;
                return;
            }
            // 处理其他错误
            qCritical() << "Shared memory error:" << m_shared.errorString();
        }
        m_isRunning = false;
    }

    ~SingleInstanceGuard() {
        if (!m_isRunning) {
            m_shared.detach(); // 正常退出时释放
        }
    }

    bool isRunning() const { return m_isRunning; }

private:
    QSharedMemory m_shared;
    bool m_isRunning = true;
};



int main(int argc, char* argv[])
{
#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication* app = new QApplication(argc, argv);
    app->setApplicationName("TypicalTools");  // 设置应用程序名称
    app->setQuitOnLastWindowClosed(false);

    QIcon Icon(":/qt/qml/typicaltools/resource/Icon/DeerAndPipe.ico");
    app->setWindowIcon(Icon);

    //单例运行
    SingleInstanceGuard guard("TypicalTools_SINGLE_INSTANCE");
    if (guard.isRunning()) {
        QMessageBox::warning(nullptr, "TypicalTools", "警告: 程序已在运行中!😵‍💫");
        return 1;
    }


    QtTypicalTool::Settings* SettingsInstance = new QtTypicalTool::Settings();
    SettingsInstance->Initialize(app, QCoreApplication::applicationName(), 
        QCoreApplication::applicationDirPath()); //保存当前程序目录


    // 创建系统托盘图标
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon();
    SettingsInstance->SystemTrayIcon = trayIcon;
    trayIcon->setIcon(Icon);
    trayIcon->setToolTip(QString(SettingsInstance->applicationWindowTitleName));

    // 创建托盘菜单
    QMenu* trayMenu = new QMenu();
    SettingsInstance->Menu = trayMenu;
    trayMenu->setStyleSheet(R"(
    QMenu {
        background-color: #4e4e4e;
        border-radius: 10px;
        padding: 5px;
        color: white;
        /* 设置菜单最小宽度 */
        min-width: 180px;  /* 调整菜单整体宽度 */
        max-width: 300px;  /* 调整菜单整体宽度 */
    }
    QMenu::item {
        padding: 5px 20px;
        border-radius: 5px;
    }
    QMenu::item:selected {
        background-color: #8f8f8f;
    }
    QMenu::separator {
        height: 1px;
        background: #555;
        margin: 5px 0;
    }
)");
    SettingsInstance->loadBaseConfig(false);
    SettingsInstance->loadShellConfig();

    SettingsInstance->loadToolsMenu();
    SettingsInstance->ExeRunItemShell();

    return app->exec();
}
