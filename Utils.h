#pragma once
#pragma execution_character_set("UTF-8")

#include "ui_RecipeManager.h"
#include <QtSql>
#include <QMessageBox>
#include <QDateTime>

class Utils {//工具包类
private:
	class DBConnection {//数据库连接类（仅供工具包类使用）
	private:
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//数据库实体
	public:
		DBConnection() {};
		~DBConnection();
		bool init();//初始化数据库
		inline QSqlDatabase& getDBObj();//获取数据库实体
	};
	static DBConnection conn;//数据库连接实体

public:
	static inline QString getAppDataPath();//获取程序数据路径
	static QString createID();//依据当前时间创建ID
	static QString getTimeFromID(const QString& ID);//将作为ID的时间格式转化为用户易读的格式

	static bool initDBConnection();//初始化数据库连接
	static QSqlDatabase& getDBObj();//获取数据库实体

	static inline QString prepareDataForSQL(const QString& original) {
		return QString(original).replace("'", "''");//处理准备与数据库交互的内容
	}
	static bool SQLInsert(const QString& tableName, const QStringList& header, const QStringList& value);//执行SQL插入语句
	static bool SQLInsert_Multiple(const QString& tableName, const QStringList& header, const QList<QStringList>& value);//执行SQL批量插入语句
	static QList<QStringList> SQLQuery(const QString& sql);//执行SQL查询语句
	static bool SQLExcecute(const QString& sql);//执行除查询以外的其他SQL语句
	static bool SQLExcecute_Multiple(const QStringList& sql);//执行多条除查询以外的其他SQL语句

	static double round(double number, const unsigned int& precisionAfterPoint);//小数点后n位四舍五入

	static bool isContentInvalid(const QString& content);//内容合规性检查，下同
	static bool isContentInvalid(const QStringList& content);
	static bool isContentInvalid(const QList<QStringList>& content);

	static QList<QStringList> fuzzySearch(const QString& keywordText, const QString& dataBaseName, const QStringList& dataBaseHeader);//多关键词多属性查找

	static QString amountCalc(const QString&, const QString&);//处理数量 1×2
	static QString amountCalc(const QString&, const QString&, const QString&);//处理数量 1+(2×3)
};
