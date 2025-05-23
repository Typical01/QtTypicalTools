#pragma once
#ifndef BACKEND_H
#define BACKEND_H


#include <QApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSystemTrayIcon>
#include <QMenu>

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QQuickView>

#include <QSharedMemory>
#include <QMessageBox>
#include <QAbstractListModel>

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>


#include <ShellConfig.h>
#include <ShellConfigModel.h>


#include <TypicalTool/Tool.h>



using namespace tytl;


namespace QtTypicalTool {
    class Settings : public QObject
    {
        Q_OBJECT
    
        Q_PROPERTY(bool bIsSelfAutoStarting READ getIsSelfAutoStarting WRITE setIsSelfAutoStarting NOTIFY isSelfAutoStartingChanged)
        Q_PROPERTY(QVariantList shellConfigList READ getShellConfigList WRITE setShellConfigList NOTIFY shellConfigListChanged)
        Q_PROPERTY(ShellConfigModel* shellConfigModel READ getShellConfigModel WRITE setShellConfigModel NOTIFY shellConfigModelChanged)

    public:
        Settings(QObject* parent = nullptr);
        ~Settings();

    public:
        QApplication* Application;
        QSystemTrayIcon* SystemTrayIcon;
        QMenu* Menu;
        QQmlApplicationEngine* QmlApplicationEngine;

    public:
        QString applicationName;
        QString applicationDirPath;
        QString applicationWindowTitleName = TEXT("TypicalTools v1.1");
        QString applicationConfigPath;

        Json::Value rootConfig;
        JsonManage jsonManage;
        FileSystem fileSystem;

        QVariantList shellConfigList;
        ShellConfigModel* shellConfigModel;
        std::vector<ShellConfig*> ExeRunItem; //程序启动项
        std::map<int32_t, ShellConfig*> ExeMenuItem; //程序菜单项

        bool bIsSelfAutoStarting = false;
        inline static int32_t IntId = 1000;

    public:
        int32_t GetIntId();
        //void ShellOperate(QMenu* _Menu, QVariantList& _ShellConfig);
        void ShellOperate(QMenu* _Menu, ShellConfigModel* _shellConfigModel);
        void ExeRunItemShell();
        void ExeMenuItemShell(int32_t _MenuItemID);
        void ExecuteAnalyze(QString OperateName, QString ShellOperate, QString ShellFile, QString ShellArg = TEXT(""), bool WindowShow = true);

        void shellConfigListSort()
        {
            auto comparator = [](const QVariant& a, const QVariant& b) {
                ShellConfig* itemA = qvariant_cast<ShellConfig*>(a);
                ShellConfig* itemB = qvariant_cast<ShellConfig*>(b);
                if (itemA && itemB) {
                    return itemA->getMenuButton() < itemB->getMenuButton();
                }
                return false;
                };
            std::sort(shellConfigList.begin(), shellConfigList.end(), comparator);
        }

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
        Q_INVOKABLE ShellConfigModel* getShellConfigModel() const { return shellConfigModel; }
        Q_INVOKABLE void setShellConfigModel(ShellConfigModel* model) {
            if (shellConfigModel != model) {
                shellConfigModel->setData(*model);
                emit shellConfigModelChanged();
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
        void shellConfigModelChanged();
        void isSelfAutoStartingChanged();

    public slots:

        void loadBaseConfig();
        void updateConfig();
        void loadShellConfig(bool reLoad = false);
        void loadToolsMenu();

        void logDebug(const QString& message)
        {
            qDebug() << message;
        }

        void test() {
            logDebug(Printf(TEXT("test: Printf!")).str().c_str());
        }

        void help() {
            QMessageBox::warning(nullptr, "TypicalTools: 帮助", 
                "作者: Typical01\nGithub: https://github.com/Typical01\n\n本程序由Qt框架制作.\nQt: https://download.qt.io/archive/qt/5.12/5.12.0/single/qt-everywhere-src-5.12.0.zip");
        }

        void openSettingWindow() {
            offLoadEngine();
            if (QmlApplicationEngine) {
                QmlApplicationEngine->load(QUrl(QStringLiteral("qrc:/qt/qml/typicaltools/resource/main.qml")));

                if (QmlApplicationEngine->rootObjects().isEmpty()) {
                    QMessageBox::critical(nullptr, "TypicalTools", "加载 QML 文件失败！");
                    delete QmlApplicationEngine;
                    QmlApplicationEngine = nullptr;
                }
            }
        }

        void onLoadEngine() {
            QmlApplicationEngine = new QQmlApplicationEngine();
            QmlApplicationEngine->rootContext()->setContextProperty("settings", this);
        }

        void offLoadEngine() {
            if (QmlApplicationEngine) {
                for (QObject* object : QmlApplicationEngine->rootObjects()) {
                    QWindow* window = qobject_cast<QWindow*>(object);
                    if (window) {
                        window->close();
                    }
                }

                if (!QmlApplicationEngine->rootObjects().isEmpty()) {
                    QmlApplicationEngine->rootObjects().first()->deleteLater();
                }
            }
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
            shellConfigListSort();
        }
        void shellConfigListRemove(int index)
        {
            shellConfigList.removeAt(index);
        }

        void saveData() {
            qDebug() << "saveData: QmlList[listViewShellConfig] Item Sun: " << shellConfigModel->rowCount();
            rootConfig.clear();

            Json::Value jsonBase;
            jsonBase["注册表开机自启动"] = bIsSelfAutoStarting;
            rootConfig[TEXT("基本设置")] = jsonBase;

#ifdef QVariantList
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
#else
            for (int index = 0; index < shellConfigModel->rowCount(); ++index) {
                QModelIndex modelIndex = shellConfigModel->index(index, 0);
                QString operateName = shellConfigModel->data(modelIndex, ShellConfigModel::OperateNameRole).toString();
                QString shellOperate = shellConfigModel->data(modelIndex, ShellConfigModel::ShellOperateRole).toString();
                QString file = shellConfigModel->data(modelIndex, ShellConfigModel::FileRole).toString();
                QString arg = shellConfigModel->data(modelIndex, ShellConfigModel::ArgRole).toString();
                bool windowShow = shellConfigModel->data(modelIndex, ShellConfigModel::WindowShowRole).toBool();
                bool menuButton = shellConfigModel->data(modelIndex, ShellConfigModel::MenuButtonRole).toBool();

                Json::Value json;
                json["模式"] = shellOperate.toStdString();
                qDebug() << "saveData: shellOperate: " << shellOperate;
                json["文件"] = file.toStdString();
                qDebug() << "saveData: file: " << file;
                json["参数"] = arg.toStdString();
                qDebug() << "saveData: arg: " << arg;
                json["显示窗口"] = windowShow;
                qDebug() << "saveData: windowShow: " << windowShow;
                json["菜单按键"] = menuButton;
                qDebug() << "saveData: menuButton: " << menuButton;

                rootConfig[operateName.toStdString()] = json;
                qDebug() << "saveData: operateName: " << operateName;
            }
#endif

            jsonManage.SetJsonValue(rootConfig);
            jsonManage.ToStreamString();
            jsonManage.WriteJsonFile();
            qDebug() << "saveData: 完成!";
        }
    };
}

#endif
