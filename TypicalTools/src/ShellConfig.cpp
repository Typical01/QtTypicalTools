#include "ShellConfig.h"


ShellConfig::ShellConfig(QObject *parent)
	: QObject(parent)
{}

ShellConfig::ShellConfig(const QString & _operateName, const QString & _shellOperate, const QString & _file, 
	const QString & _arg, const bool& _windowShow, const bool& _menuButton)
{
	m_operateName = _operateName;
	m_shellOperate = _shellOperate;
	m_file = _file;
	m_arg = _arg;
	m_windowShow = _windowShow;
	m_menuButton = _menuButton;
}

ShellConfig::ShellConfig(const ShellConfig& otherObject)
{
	m_operateName = otherObject.m_operateName;
	m_shellOperate = otherObject.m_shellOperate;
	m_file = otherObject.m_file;
	m_arg = otherObject.m_arg;
	m_windowShow = otherObject.m_windowShow;
	m_menuButton = otherObject.m_menuButton;
}

ShellConfig::~ShellConfig()
{}

void ShellConfig::output()
{
	qDebug() << "ShellConfig::output: \n" <<
		"\toperateName: " << m_operateName << "\n" <<
		"\tshellOperate: " << m_shellOperate << "\n" <<
		"\tfile		  : " << m_file << "\n" <<
		"\targ		  : " << m_arg << "\n" <<
		"\twindowShow  : " << m_windowShow << "\n" <<
		"\tmenuButton  : " << m_menuButton << "\n";
}