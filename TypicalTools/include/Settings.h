#pragma once
#ifndef BACKEND_H
#define BACKEND_H

//#include <QQmlEngine>
#include <QApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSystemTrayIcon>
#include <QMenu>

#include <QObject>
#include <QDebug>
#include <QUrl>
#include <QQuickView>

#include <QSharedMemory>
#include <QMessageBox>
#include <QAbstractListModel>

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>


#include <ShellConfig.h>


#include <TypicalTool/Tool.h>



using namespace tytl;


namespace QtTypicalTool {
    class Settings : public QObject
    {
        Q_OBJECT
    
        Q_PROPERTY(bool bIsSelfAutoStarting READ getIsSelfAutoStarting WRITE setIsSelfAutoStarting NOTIFY isSelfAutoStartingChanged)
        Q_PROPERTY(QVariantList shellConfigList READ getShellConfigList WRITE setShellConfigList NOTIFY shellConfigListChanged)

    public:
        Settings(QObject* parent = nullptr);
        ~Settings();

    public:
        QApplication* Application;
        QSystemTrayIcon* SystemTrayIcon;
        QMenu* Menu;
        QQmlApplicationEngine* QmlApplicationEngine;
        bool bEnginValid;

    public:
        QString applicationName;
        QString applicationDirPath;
        QString applicationWindowTitleName = TEXT("TypicalTools v1.1");

        QString applicationConfigPath;

        Json::Value rootConfig;
        JsonManage jsonManage;
        FileSystem fileSystem;

        QVariantList shellConfigList;
        std::vector<ShellConfig> ExeRunItem; //程序启动项
        std::map<int32_t, ShellConfig*> ExeMenuItem; //程序菜单项

        bool bIsSelfAutoStarting = false;
        inline static int32_t IntId = 1000;

    public:
        int32_t GetIntId();
        void ShellOperate(QMenu* _Menu, QVariantList& _ShellConfig);
        void ExeRunItemShell();
        void ExeMenuItemShell(int32_t _MenuItemID);
        void ExecuteAnalyze(QString OperateName, QString ShellOperate, QString ShellFile, QString ShellArg = TEXT(""), bool WindowShow = true);

    public:
        void Initialize(QApplication* _Application, const QString& _applicationName, const QString& _applicationDirPath);

        static void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    public:
        Q_INVOKABLE QVariantList getShellConfigList() const { return shellConfigList; }
        Q_INVOKABLE void setShellConfigList(const QVariantList& list) {
            if (shellConfigList != list) {
                shellConfigList = list;
                emit shellConfigListChanged();
            }
        }
        Q_INVOKABLE bool getIsSelfAutoStarting() const { return bIsSelfAutoStarting; }
        Q_INVOKABLE void setIsSelfAutoStarting(const bool& IsSelfAutoStarting) {
            if (bIsSelfAutoStarting != IsSelfAutoStarting) {
                bIsSelfAutoStarting = IsSelfAutoStarting;
                emit isSelfAutoStartingChanged();
            }
        }

    signals:
        void shellConfigListChanged();
        void isSelfAutoStartingChanged();

    public slots:

        void loadBaseConfig(bool _bReLoad);
        void updateConfig();
        void loadShellConfig();
        void loadToolsMenu();

        void logDebug(const QString& message)
        {
            qDebug() << message;
        }

        void test() {
            logDebug(Printf(TEXT("test: Printf!")).str().c_str());
        }

        void openSettingWindow(const QString& _ComponentName) {
            //offLoadEngine(_ComponentName);
            //onLoadEngine();
            if (!bEnginValid) {
                qDebug() << Printf(TEXT("openSettingWindow: Qml引擎无效!")).str().c_str();
                return;
            }
            QmlApplicationEngine->load(QUrl(QStringLiteral("qrc:/qt/qml/typicaltools/main.qml")));
        }

        void destroySettingWindow(const QString& _ComponentName) {
            //offLoadEngine(_ComponentName);
        }

        bool onLoadEngine() {
            // 使用 QQmlApplicationEngine 加载 QML 文件
            QmlApplicationEngine = new QQmlApplicationEngine();
            if (QmlApplicationEngine->rootContext() != nullptr) {
                QmlApplicationEngine->rootContext()->setContextProperty("settings", this);
                return true;
            }
            return false;
        }

        bool offLoadEngine(const QString& _ComponentName) {
            if (QmlApplicationEngine != nullptr) {
                qDebug() << Printf(TEXT("[%s]offLoadEngine: 释放Qml引擎资源!"), _ComponentName.toStdString()).str().c_str();
                QmlApplicationEngine->deleteLater();
                QCoreApplication::processEvents();
                QmlApplicationEngine->clearComponentCache();
                QmlApplicationEngine->collectGarbage();
                delete QmlApplicationEngine;

                return true;
            }
            return false;
        }

        void shellConfigListAppend(const QString& OperateName, const QString& ShellOperate, const QString& File,
            const QString& Args, const bool& WindowShow, const bool& MenuButton)
        {
            // 创建 ShellConfig 对象
            ShellConfig* config = new ShellConfig();
            config->setOperateName(OperateName);
            config->setShellOperate(ShellOperate);
            config->setFile(File);
            config->setArg(Args);
            config->setWindowShow(WindowShow);
            config->setMenuButton(MenuButton);

            // 将 ShellConfig 对象添加到 QVariantList
            shellConfigList.append(QVariant::fromValue(config));
        }
        void shellConfigListRemove(int index)
        {
            shellConfigList.removeAt(index);
        }

        void saveData() {
            qDebug() << "saveData: QmlList[listViewShellConfig] Item Sun: " << shellConfigList.size();
            rootConfig.clear();

            Json::Value jsonBase;
            jsonBase["注册表开机自启动"] = bIsSelfAutoStarting;
            rootConfig[TEXT("基本设置")] = jsonBase;

            for (int index = 0; index < shellConfigList.size(); ++index) {
                qDebug() << "saveData: QmlList[listViewShellConfig] Item Count: " << index;

                // 处理获取到的数据
                ShellConfig* config = qvariant_cast<ShellConfig*>(shellConfigList[index]); 

                Json::Value json;
                json["模式"] = config->getShellOperate().toStdString();
                qDebug() << "saveData: shellOperate: " << config->getShellOperate();
                json["文件"] = config->getFile().toStdString();
                qDebug() << "saveData: file:         " << config->getFile();
                json["参数"] = config->getArg().toStdString();
                qDebug() << "saveData: arg:          " << config->getArg();
                json["显示窗口"] = config->getWindowShow();
                qDebug() << "saveData: windowShow:   " << config->getWindowShow();
                json["菜单按键"] = config->getMenuButton();
                qDebug() << "saveData: menuButton:   " << config->getMenuButton();

                rootConfig[config->getOperateName().toStdString()] = json;
                qDebug() << "saveData: operateName:  " << config->getOperateName();
            }

            jsonManage.SetJsonValue(rootConfig);
            jsonManage.ToStreamString();
            jsonManage.WriteJsonFile();
            qDebug() << "saveData: 完成!";
        }
    };
}

#endif
