#include <QGuiApplication>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QApplication>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMenu>

#include <QQuickView>



int main(int argc, char* argv[])
{
#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    QIcon Icon(":/qt/qml/typicaltools/resource/Icon/DeerAndPipe.ico");
    app.setWindowIcon(Icon);

    // 创建系统托盘图标
    QSystemTrayIcon trayIcon;
    trayIcon.setIcon(Icon);
    trayIcon.setToolTip("TypicalTools v1.1");

    // 创建托盘菜单
    QMenu trayMenu;
    QAction openSettingsAction("设置");
    QAction quitAction("退出");
    trayMenu.addAction(&openSettingsAction);
    trayMenu.addAction(&quitAction);
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();

    // 使用 QQmlApplicationEngine 加载 QML 文件
    QQmlApplicationEngine engine;

    // 连接托盘菜单动作
    QObject::connect(&openSettingsAction, &QAction::triggered, [&engine]() {
        engine.load(QUrl("qrc:/qt/qml/typicaltools/main.qml"));
        });

    QObject::connect(&quitAction, &QAction::triggered, &app, &QApplication::quit);

    return app.exec();
}
