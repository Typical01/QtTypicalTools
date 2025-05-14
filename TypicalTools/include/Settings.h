#pragma once
#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>


class Settings  : public QObject
{
	Q_OBJECT

public:
	Settings(QObject *parent);
	~Settings();

public slots:
    void save() {
        qDebug() << "保存操作执行";
        // 添加保存逻辑
    }

    void exit() {
        qDebug() << "退出操作执行";
        // 添加退出逻辑
    }
};

#endif
