#include "Settings.h"


QtTypicalTool::Settings::Settings(QObject *parent)
	: QObject(parent), Application(nullptr), SystemTrayIcon(nullptr), Menu(nullptr), QmlApplicationEngine(nullptr)
{
    // 使用 QQmlApplicationEngine 加载 QML 文件
    QmlApplicationEngine = new QQmlApplicationEngine();
    if (QmlApplicationEngine->rootContext() != nullptr) {
        QmlApplicationEngine->rootContext()->setContextProperty("settings", this);
    }

}

QtTypicalTool::Settings::~Settings()
{
    LogDebug(LogErr, Printf(TEXT("Settings: 程序结束!")));
}

void QtTypicalTool::Settings::Initialize(QApplication* _Application, const Tstr& _applicationName, const Tstr& _applicationDirPath)
{
    Application = _Application;
    applicationName = _applicationName;
    applicationDirPath = _applicationDirPath;

    tytl::Win::WindowDPI();

#ifdef _DEBUG
    Log::SetDebug(true);
#endif
    Log::SetLogFileWrite(true);
    Log::SetLogWriteLevel(LogTip);
    LogDebug.SetLogFileWriteConfig(TEXT("TypicalTools"), Printf(TEXT("%s/Log"), applicationDirPath));
}

int32_t QtTypicalTool::Settings::GetIntId()
{
    return IntId++;
}

void QtTypicalTool::Settings::ShellOperate(QMenu* _Menu, std::vector<tytl::Win::ShellConfig>& _ShellConfig) {
    LogDebug(TEXT("Typical_Tool::WindowsSystem::WindowShell::ShellOperate"));

    for (auto tempShell = _ShellConfig.begin(); tempShell != _ShellConfig.end(); tempShell++) {
        //判断类型
        Tstr OperateName = tempShell->OperateName;
        Tstr MenuButton = tempShell->MenuButton;

        //区分: 程序启动项/程序菜单项
        if (MenuButton == TEXT("否")) {
            ExeRunItem.push_back(*tempShell);
            LogDebug(Printf(TEXT("操作名: [%s]"), OperateName));
            LogDebug(TEXT("  注册: 程序启动项"));
            tempShell->OutConfig(); //输出配置
        }
        else {
            ExeMenuItem.insert({ GetIntId(), *tempShell});
            LogDebug(Printf(TEXT("操作名: [%s]"), OperateName));
            LogDebug(TEXT("  注册: 程序菜单项"));
            //添加菜单项
            QAction* MenuItemAction = new QAction(OperateName.c_str());
            Menu->addAction(MenuItemAction);
            tempShell->OutConfig(); //输出配置
        }
    }
}

void QtTypicalTool::Settings::ExeRunItemShell() {
    //遍历执行所有: 程序启动项
    if (ExeRunItem.size() > 0) {
        for (auto tempShell = ExeRunItem.begin(); tempShell != ExeRunItem.end(); tempShell++) {
            auto OperateName = tempShell->OperateName;
            auto ShellOperate = tempShell->ShellOperate;
            auto File = tempShell->File;
            auto Arg = tempShell->Arg;
            auto WindowShow = tempShell->WindowShow;

            ExecuteAnalyze(OperateName, ShellOperate, File, Arg, WindowShow);
        }
    }
    else {
        LogDebug(LogWar, TEXT("ExeRunItemShell: 没有执行项!"));
    }
}

void QtTypicalTool::Settings::ExeMenuItemShell(int32_t _MenuItemID) {
    //查找并执行对应菜单ID的 Shell配置
    auto tempIndex = ExeMenuItem.find(_MenuItemID);
    if (tempIndex != ExeMenuItem.end()) {
        tytl::Win::ShellConfig tempShellConfig = tempIndex->second;

        Tstr OperateName = tempShellConfig.OperateName;
        Tstr ShellOperate = tempShellConfig.ShellOperate;
        Tstr File = tempShellConfig.File;
        Tstr Arg = tempShellConfig.Arg;
        Tstr WindowShow = tempShellConfig.WindowShow;

        ExecuteAnalyze(OperateName, ShellOperate, File, Arg, WindowShow);
    }
    else {
        LogDebug(LogErr, Printf(TEXT("ExeMenuItemShell: 没有找到菜单选项 [%s]!"), _MenuItemID));
    }
}

tytl::Win::ShellMessage QtTypicalTool::Settings::ExecuteAnalyze(Tstr OperateName, Tstr ShellOperate, Tstr ShellFile, Tstr ShellArg, Tstr WindowShow) {
    if (ShellOperate == TEXT("打开文件") || ShellOperate == TEXT("open")) {
        ShellOperate = TEXT("open");
        LogDebug(LogTip, TEXT("ExecuteAnalyze: Shell操作模式(打开文件)"));
    }
    else if (ShellOperate == TEXT("管理员运行") || ShellOperate == TEXT("runas")) {
        ShellOperate = TEXT("runas");
        LogDebug(LogTip, TEXT("ExecuteAnalyze: Shell操作模式(管理员运行)"));
    }
    else if (ShellOperate == TEXT("打开文件夹") || ShellOperate == TEXT("explore")) {
        ShellOperate = TEXT("explore");
        LogDebug(LogTip, TEXT("ExecuteAnalyze: Shell操作模式(打开文件夹)"));
    }
    else {
        LogDebug(LogWar, TEXT("ExecuteAnalyze: Shell操作模式错误(打开文件/打开文件夹/管理员运行)"));
        LogDebug(LogWar, Printf(TEXT("ExecuteAnalyze: 操作名: [%s]"), OperateName));
        return tytl::Win::ShellMessage();
    }

    int32_t ShowWindow = 0;
    if (WindowShow == TEXT("是")) {
        ShowWindow = 5;
    }
    LogDebug(LogWar, Printf(TEXT("ExecuteAnalyze: 窗口显示 [%s]"), WindowShow));

    tytl::Win::ShellMessage temp(OperateName, (int32_t)ShellExecute(NULL, ShellOperate.c_str(), ShellFile.c_str(), ShellArg.c_str(), NULL, ShowWindow));
    return temp;
}

void QtTypicalTool::Settings::LoadBaseConfig(bool _bReLoad)
{
    if (_bReLoad) {
        LogDebug(LogTip, TEXT("Settings::LoadBaseConfig: ReLoad."));
        Json::Value temp;
        jsonManage.SetJsonValue(temp); //清空 Json::Value
    }
    else {
        LogDebug(TEXT("Settings::LoadBaseConfig: Load."));
    }

    //先创建文件夹(否则后面的文件不能创建): \\Tools\\Config
    Tstr ConfigDirectory = Printf(TEXT("%s\\Config"), applicationDirPath);
    applicationConfigPath = Printf(TEXT("%s%s"), ConfigDirectory, TEXT("\\Config.ini"));
    fileSystem.SetPath(ConfigDirectory);
    if (!fileSystem.CreateDirectorys()) {
        LogDebug(LogWar, Printf(TEXT("Settings::LoadBaseConfig: 文件夹[%s]已存在!"), ConfigDirectory));
    }

    //文件不存在时，创建
    if (jsonManage.Init(Printf(TEXT("%s"), applicationConfigPath), false)) {
        LogDebug(LogWar, Printf(TEXT("Settings::LoadBaseConfig: 开始创建Json文件[%s]!"), applicationConfigPath));
        LogDebug(Printf(TEXT("Settings::LoadBaseConfig: 配置文件初始化[%s]"), jsonManage.GetJsonFilePath()));

        // 基本设置
        Json::Value BaseConfig;
        BaseConfig[TEXT("注册表开机自启动")] = TEXT("否");

        // ping百度配置
        Json::Value ping_baidu;
        ping_baidu[TEXT("菜单按键")] = TEXT("是");
        ping_baidu[TEXT("模式")] = TEXT("打开文件");
        ping_baidu[TEXT("文件")] = TEXT("cmd");
        ping_baidu[TEXT("参数")] = TEXT("/k ping -t www.baidu.com");
        ping_baidu[TEXT("显示窗口")] = TEXT("是");

        // 笔记本键盘开启配置
        Json::Value BookKeyOn;
        BookKeyOn[TEXT("菜单按键")] = TEXT("是");
        BookKeyOn[TEXT("模式")] = TEXT("管理员运行");
        BookKeyOn[TEXT("文件")] = TEXT("cmd");
        BookKeyOn[TEXT("参数")] = TEXT("/k sc config i8042prt start= auto");
        BookKeyOn[TEXT("显示窗口")] = TEXT("是");

        // 笔记本键盘关闭配置
        Json::Value BookKeyOff;
        BookKeyOff[TEXT("菜单按键")] = TEXT("是");
        BookKeyOff[TEXT("模式")] = TEXT("管理员运行");
        BookKeyOff[TEXT("文件")] = TEXT("cmd");
        BookKeyOff[TEXT("参数")] = TEXT("/k sc config i8042prt start= disabled");
        BookKeyOff[TEXT("显示窗口")] = TEXT("是");

        rootConfig[TEXT("基本设置")] = BaseConfig;
        rootConfig[TEXT("ping-baidu")] = ping_baidu;
        rootConfig[TEXT("笔记本键盘开")] = BookKeyOn;
        rootConfig[TEXT("笔记本键盘关")] = BookKeyOff;
        jsonManage.SetJsonValue(rootConfig);
        jsonManage.WriteJsonFile();
    }

    if (!jsonManage.Init(Printf(TEXT("%s"), applicationConfigPath), true)) {
        LogDebug(LogErr, Printf(TEXT("Settings::LoadBaseConfig: 读取文件[%s]失败!"), applicationConfigPath));
    }
    else {
        LogDebug(LogTip, Printf(TEXT("Settings::LoadBaseConfig: 读取文件[%s]成功!"), applicationConfigPath));
    }

    UpdateConfig();
}

void QtTypicalTool::Settings::UpdateConfig()
{
    if (!jsonManage.GetJsonValue().empty()) { //修改配置后, 重新加载
        LogDebug(LogTip, TEXT("Settings::UpdateConfig: ReLoad."));
        return;
    }
    //获取更新后的 rootConfig
    rootConfig = jsonManage.GetJsonValue();

    if (rootConfig.isMember(TEXT("基本设置"))) {
        bIsSelfAutoStarting = rootConfig[TEXT("基本设置")].isMember(TEXT("注册表开机自启动"));
    }
    LogDebug(LogTip, Printf(TEXT("Settings::UpdateConfig: 设置 开机自启动[%s]"), ToStr(bIsSelfAutoStarting)));
    LogDebug(Printf(TEXT("Settings::UpdateConfig: rootConfig的配置数量[%s]"), ToStr(rootConfig.size())));

    //更新 开机自启动
    Tstr ExePathName;
    if (Win::GetExePathName(ExePathName)) {
        if (bIsSelfAutoStarting != true) {
            LogDebug(TEXT("Settings::UpdateConfig: 注册开机自启动[否]"), LogTip);

            if (Win::SetSelfStarting(applicationName, Printf(TEXT("\"%s\""), ExePathName).str(), TEXT(""), false)) {
                LogDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 删除成功!"), LogTip);
            }
            else {
                LogDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 删除失败!"), LogErr);
            }
        }
        else {
            LogDebug(TEXT("Settings::UpdateConfig: 注册开机自启动[是]"), LogTip);
            if (Win::SetSelfStarting(applicationName, Printf(TEXT("\"%s\""), ExePathName).str(), TEXT(""), true)) {
                LogDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 添加成功!"), LogTip);
            }
            else {
                LogDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 添加失败!"), LogErr);
            }
        }
    }
}

void QtTypicalTool::Settings::LoadShellConfig()
{
    if (shellConfig.empty()) { //修改配置后
        LogDebug(LogTip, TEXT("ShellConfigInit() ReLoad"));
        shellConfig.clear(); //清空Shell配置
        ExeRunItem.clear();
        ExeMenuItem.clear();
    }
    else {
        LogDebug(TEXT("ShellConfigInit()"));
    }

    for (auto& ItConfig = rootConfig.begin(); ItConfig != rootConfig.end(); ++ItConfig) {
        Tstr Key = ItConfig.key().asString();
        Json::Value Value = *ItConfig;

        // 执行Shell配置（排除"基本设置"）
        if (Key != TEXT("基本设置")) {
            LogDebug(Printf(TEXT("ShellConfig: [%s]"), Key));

            Tstr OperateMode;
            Tstr File;
            Tstr Args;
            Tstr WindowShow;
            Tstr MenuButton;

            // 读取配置信息 - 使用Json::Value的安全访问方法
            if (Value.isMember("模式")) {
                OperateMode = Value["模式"].asString();
                LogDebug(Printf(TEXT("  操作模式: [%s]"), OperateMode));
            }
            else {
                LogDebug(Printf(TEXT("  ShellConfig: [%s]没有 操作模式!"), Key), LogWar);
            }

            if (Value.isMember("文件")) {
                File = Value["文件"].asString();
                LogDebug(Printf(TEXT("  文件: %s"), File));
            }
            else {
                LogDebug(Printf(TEXT("  ShellConfig: [%s]没有 文件!"), Key), LogWar);
            }

            if (Value.isMember("参数")) {
                Args = Value["参数"].asString();
                LogDebug(Printf(TEXT("  参数: %s"), Args));
            }
            else {
                LogDebug(Printf(TEXT("  ShellConfig: [%s]没有 参数!"), Key), LogWar);
            }

            if (Value.isMember("显示窗口")) {
                WindowShow = Value["显示窗口"].asString();
                LogDebug(Printf(TEXT("  显示窗口: [%s]"), WindowShow));
            }
            else {
                LogDebug(Printf(TEXT("  ShellConfig: [%s]没有 显示窗口!"), Key), LogWar);
            }

            if (Value.isMember("菜单按键")) {
                MenuButton = Value["菜单按键"].asString();
                LogDebug(Printf(TEXT("  菜单按键: [%s]"), MenuButton));
            }
            else {
                LogDebug(Printf(TEXT("  ShellConfig: [%s]没有 菜单按键!"), Key), LogWar);
            }

            shellConfig.push_back(tytl::Win::ShellConfig(Key, OperateMode, File, Args, WindowShow, MenuButton));
        }
        else {
            LogDebug(Printf(TEXT("OtherConfig: [%s]"), Key));
        }
    }
}

void QtTypicalTool::Settings::LoadToolsMenu()
{
    LogDebug(LogTip, TEXT("LoadToolsMenu() ReLoad"));
    Menu->clear(); //清除现有菜单项

    //为Menu添加选项  
    /*QAction* screenResolutionAction = new QAction("修改屏幕分辨率", Menu);
    Menu->addAction(screenResolutionAction);
    Menu->addSeparator();*/

    //Shell插入位置: Menu项总数 - 4
    ShellOperate(Menu, shellConfig);

    QAction* ClickTestAction = new QAction("点击测试");
    QAction* openSettingsAction = new QAction("设置");
    QAction* helpSettingsAction = new QAction("帮助");
    QAction* quitAction = new QAction("退出");

    Menu->addSeparator(); // 添加分隔符
    Menu->addAction(ClickTestAction);
    Menu->addAction(openSettingsAction);
    Menu->addAction(helpSettingsAction);
    Menu->addSeparator(); // 添加分隔符
    Menu->addAction(quitAction);
    
    SystemTrayIcon->setContextMenu(Menu);
    SystemTrayIcon->show();

    // 连接托盘菜单动作
    QObject::connect(openSettingsAction, &QAction::triggered, [this]() {
        this->openSettingWindow();
        });
    QObject::connect(ClickTestAction, &QAction::triggered, [this]() {
        this->test();
        });

    QObject::connect(quitAction, &QAction::triggered, Application, &QApplication::quit);

    //Win::RegisterHotKeyMessage RegisterHotKey_SetScreenResolution(TEXT("Ctrl + Alt + F9"), 
    // RegisterHotKey(hWnd_Tray, Menu_ScreenResolution, MOD_CONTROL | MOD_ALT, VK_F9));
}
