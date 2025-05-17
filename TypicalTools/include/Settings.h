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


#include <TypicalTool/Tool.h>



using namespace tytl;


namespace QtTypicalTool {
    class Settings : public QObject
    {
        Q_OBJECT

    public:
        Settings(QObject* parent = nullptr);
        ~Settings();

    public:
        QApplication* Application;
        QSystemTrayIcon* SystemTrayIcon;
        QMenu* Menu;
        QQmlApplicationEngine* QmlApplicationEngine;

    public:
        Tstr applicationName;
        Tstr applicationDirPath;
        Tstr applicationWindowTitleName = TEXT("TypicalTools v1.1");

        Tstr applicationConfigPath;

        Json::Value rootConfig;
        JsonManage jsonManage;
        FileSystem fileSystem;

        std::vector<tytl::Win::ShellConfig> ExeRunItem; //程序启动项
        std::map<int32_t, tytl::Win::ShellConfig> ExeMenuItem; //程序菜单项
        std::vector<tytl::Win::ShellConfig> shellConfig;

        bool bIsSelfAutoStarting = false;
        inline static int32_t IntId = 1000;

    public:
        int32_t GetIntId();
        void ShellOperate(QMenu* _Menu, std::vector<tytl::Win::ShellConfig>& _ShellConfig);
        void ExeRunItemShell();
        void ExeMenuItemShell(int32_t _MenuItemID);
        tytl::Win::ShellMessage ExecuteAnalyze(Tstr OperateName, Tstr ShellOperate, Tstr ShellFile, Tstr ShellArg = TEXT(""), Tstr WindowShow = TEXT("是"));

    public:
        void Initialize(QApplication* _Application, const Tstr& _applicationName, const Tstr& _applicationDirPath);

        void LoadBaseConfig(bool _bReLoad);
        void UpdateConfig();
        void LoadShellConfig();
        void LoadToolsMenu();

    public slots:
        void test() {
            LogDebug(LogErr, Printf(TEXT("test: Printf!")));
        }

        void openSettingWindow() {
            offLoadEngine();
            QmlApplicationEngine = new QQmlApplicationEngine();
            QmlApplicationEngine->load(QUrl(QStringLiteral("qrc:/qt/qml/typicaltools/main.qml")));
        }

        void destroySettingWindow() {
            offLoadEngine();
        }

        bool offLoadEngine() {
            if (QmlApplicationEngine != nullptr) {
                LogDebug(LogErr, Printf(TEXT("offLoadEngine: 释放Qml引擎资源!")));
                QmlApplicationEngine->deleteLater();
                QCoreApplication::processEvents();
                QmlApplicationEngine->clearComponentCache();
                QmlApplicationEngine->collectGarbage();
                delete QmlApplicationEngine;

                return true;
            }
            return false;
        }

        void saveData() {
            qDebug() << "保存操作执行";
            // 添加保存逻辑
        }
    };
}

#endif
