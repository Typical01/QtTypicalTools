import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.12  // 必须导入此模块
import QtQuick.Layouts 1.12

Window {
    visible: true
    width: 500
    height: 800
    // 设置最大尺寸
    maximumWidth: 500
    maximumHeight: 800
    // 可选：设置最小尺寸
    minimumWidth: 500
    minimumHeight: 800
    title: "TypicalTools"
    flags: Qt.Dialog | Qt.WindowMinMaxButtonsHint | Qt.WindowSystemMenuHint | Qt.WindowTitleHint

    // Main
    ColumnLayout {
        anchors.fill: parent
        spacing: 2  // 垂直间距

        // 区域: 一般设置
        ColumnLayout {
            height: 80 
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距
            
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft    
                font.bold: false
                font.pointSize: 12
                text: "基础设置"
                font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
            }

            RowLayout {
                implicitWidth: 500; implicitHeight: 50 
                Layout.fillWidth: true
                Layout.leftMargin: 16  // 设置偏移量
                spacing: 5  // 水平间距
                
                CheckBox {
                    id: settingOption
                    checked: false  
                    text: "开机自启动"   // 单选框文本
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                }
            }
        }

        // ListView: ShellConfigItem 
        ColumnLayout {
            height: 380 
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距

            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft    
                font.bold: false
                font.pointSize: 12
                text: "Shell项"
                font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
            }

            // 分隔线
            Rectangle {
                Layout.fillWidth: true
                height: 2
                color: "#cccccc"
                border { width: 1; color: "#aaaaaa" }
            }

            RowLayout {
                height: 400 
                Layout.fillWidth: true
                spacing: 5
                
                ListView {
                    id: shellList
                    clip: true    // 超出部分裁剪
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: ListModel {
                        ListElement { name: "项 1" }
                        ListElement { name: "项 2" }
                        ListElement { name: "项 3" }
                        ListElement { name: "项 4" }
                        ListElement { name: "项 5" }
                        ListElement { name: "项 6" }
                        ListElement { name: "项 7" }
                        ListElement { name: "项 8" }
                        ListElement { name: "项 9" }
                        ListElement { name: "项 10" }
                        ListElement { name: "项 11" }
                        ListElement { name: "项 12" }
                        ListElement { name: "项 13" }
                        ListElement { name: "项 14" }
                        ListElement { name: "项 15" }
                        ListElement { name: "项 16" }
                    }

                    delegate: Item {
                        width: shellList.width
                        height: 40

                        Rectangle {
                            anchors.fill: parent
                            color: ListView.isCurrentItem ? "#d0eaff" : "white"
                            border.color: "#cccccc"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: name
                                font.pointSize: 10
                                font.family: "Microsoft YaHei"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: shellList.currentIndex = index
                            }
                        }
                    }

                    highlight: Rectangle {
                        color: "lightblue"
                        radius: 4
                    }

                    focus: true
                }
            }
        }

        // 分隔线
        Rectangle {
            Layout.fillWidth: true
            height: 2
            color: "#cccccc"
            border { width: 1; color: "#aaaaaa" }
        }

        // 配置: ShellConfigItem 
        ColumnLayout {
            height: 180 
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距
            
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft    
                font.bold: false
                font.pointSize: 12
                text: "Shell项配置"
                font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight    
                    font.bold: false
                    font.pointSize: 10
                    text: "文件路径:"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 文件: 文本编辑框
                TextField {
                    id: fileField
                    placeholderText: "例: C:/Windows/System32/cmd.exe"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight    
                    font.bold: false
                    text: "参数:"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 参数: 文本编辑框
                TextField {
                    id: paramField
                    placeholderText: "例: -Test -K -C"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight    
                    font.bold: false
                    font.pointSize: 10
                    text: "模式:"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 模式: 下拉列表框
                ComboBox {
                    id: modeCombo
                    model: ["打开文件", "打开文件夹", "管理员运行"]
                    font.family: "Microsoft YaHei"
                    font.pointSize: 10
                    Layout.preferredWidth: 400
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // 显示窗口: 复选框
                CheckBox {
                    id: showWindowCheck
                    Layout.leftMargin: 94  // 设置偏移量
                    text: "显示窗口"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // 菜单按钮: 复选框
                CheckBox {
                    id: menuButtonCheck
                    Layout.leftMargin: 94  // 设置偏移量
                    text: "菜单按钮"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                }
            }
        }
        
        // 按钮: 保存/退出
        ColumnLayout {
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距

            RowLayout {
                Layout.leftMargin: 240  // 设置偏移量
                Layout.fillWidth: true
                Layout.fillHeight: true  // 添加此行
                spacing: 5
                    
                Button {
                    text: "保存"
                    //onClicked: backend.save()
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 120

                    background: Rectangle {
                                color: "white"
                                border.color: "black"
                                border.width: 1
                                radius: 4
                    }
                }

                Button {
                    text: "退出"
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 120

                    onClicked: {
                        // 关闭当前窗口
                        close()
                    }

                    background: Rectangle {
                                color: "white"
                                border.color: "black"
                                border.width: 1
                                radius: 4
                    }
                }
            }
        }
    }
}
