#include "Utils.h"

QString Utils::getAppDataPath() {//获取应用数据路径
	return QApplication::applicationDirPath() % "/RecipeManager/";
}
QString Utils::createID() {//将当前时间转化为作为ID的格式 示例:20230801120000
	return QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
}
QString Utils::getTimeFromID(const QString& ID) {//将作为ID的时间格式转化为用户易读的格式 示例:2023.08.01 12:00:00
	return QString("%1.%2.%3 %4:%5:%6").arg(ID.mid(0,4),ID.mid(4,2),ID.mid(6,2),ID.mid(8,2),ID.mid(10,2),ID.mid(12,2));
}

Utils::DBConnection Utils::conn = DBConnection();//数据库连接实体
Utils::DBConnection::~DBConnection(){
	db.close();//关闭数据库连接
}
QSqlDatabase& Utils::DBConnection::getDBObj(){
	return db;//返回数据库实体
}

bool Utils::DBConnection::init() {//初始化数据库
	auto path = Utils::getAppDataPath();//获取应用数据文件夹路径
	if (!QDir(path).exists())//应用数据文件夹不存在？
		if (!QDir().mkdir(path)) {//应用数据文件夹创建失败？
			QString msg = "<font size='9'>数据文件夹创建失败!<br/>请检查文件夹是否被其他程序占用!</font>";
			QMessageBox::critical(nullptr, "初始化错误", msg, "确定");//错误提示
			return false;//初始化失败
		}
	db.setDatabaseName(path % "RecipeManager.db");//设置数据库名称
	if (!db.open()) {//数据库文件创建/打开失败？（数据库不存在的情况下会先自动创建数据库并打开，存在则直接打开）
		QString msg = "<font size='9'>数据库打开失败!<br/>请检查数据库文件完整性以及是否被其他程序占用!</font>";
		QMessageBox::critical(nullptr, "数据库错误", msg, "确定");//错误提示
		return false;//初始化失败
	}
	return Utils::SQLExcecute_Multiple({//执行数据库初始化语句并返回执行结果
		QLatin1String("PRAGMA foreign_keys = off"),
		QLatin1String("BEGIN TRANSACTION"),
		QLatin1String("CREATE TABLE IF NOT EXISTS Recipe(name nvarchar(50) NOT NULL, ID char(14) NOT NULL PRIMARY KEY) WITHOUT ROWID"),
		QLatin1String("CREATE TABLE IF NOT EXISTS Ingredient (RecipeID char (14) NOT NULL, name nvarchar (50) NOT NULL, amount nvarchar (20) NOT NULL, unit nvarchar (50) NOT NULL, PRIMARY KEY (RecipeID, name, unit), FOREIGN KEY (RecipeID) REFERENCES Recipe (ID) ON DELETE CASCADE ON UPDATE CASCADE) WITHOUT ROWID"),
		QLatin1String("COMMIT TRANSACTION"),
		QLatin1String("PRAGMA foreign_keys = on")
	});
}

bool Utils::initDBConnection() {
	return conn.init();//初始化数据库
}
QSqlDatabase& Utils::getDBObj(){
	return conn.getDBObj();//返回数据库实体
}

bool Utils::SQLInsert(const QString& tableName, const QStringList& header, const QStringList& value) {//执行SQL插入语句
	if (!header.size() || (header.size() != value.size()))//确保表头和值的个数均不为0且相等
		return false;
	auto itHeader = header.begin();
	auto itValue = value.begin();
	auto sql_ins = QString("INSERT INTO [%1] ([%2]").arg(tableName, *itHeader);//SQL插入语句前半部分
	auto sql_val = QString(") VALUES ('%1'").arg(prepareDataForSQL(*itValue));//SQL插入语句后半部分
	for (++itValue, ++itHeader; itValue != value.end(); ++itValue, ++itHeader) {
		sql_ins.append(QString(", [%1]").arg(*itHeader));//附加表头
		sql_val.append(QString(", '%1'").arg(prepareDataForSQL(*itValue)));//附加值
	}
	return Utils::SQLExcecute(sql_ins.append(sql_val).append(')'));//拼接SQL语句，执行并返回结果表
}
bool Utils::SQLInsert_Multiple(const QString& tableName, const QStringList& header, const QList<QStringList>& value) {//执行SQL批量插入语句
	if (!value.size() || (header.size() != value[0].size()))//确保表头和值的个数均不为0且相等
		return false;
	auto itHeader = header.begin();
	auto itValueList = value.begin();
	auto itValue = itValueList->begin();
	auto sql = QString("INSERT INTO [%1] select '%2' as [%3]").arg(tableName, prepareDataForSQL(*itValue), *itHeader);
	for (++itValue, ++itHeader; itValue != itValueList->end(); ++itValue, ++itHeader) {
		sql.append(QString(", '%1' as [%2]").arg(prepareDataForSQL(*itValue), *itHeader));
	}
	while (++itValueList != value.end()) {
		itValue = itValueList->begin();
		sql.append(QString(" UNION select '%1'").arg(prepareDataForSQL(*itValue)));
		for(++itValue; itValue != itValueList->end(); ++itValue)
			sql.append(QString(", '%1'").arg(prepareDataForSQL(*itValue)));
	}
	return Utils::SQLExcecute(sql);
}
QList<QStringList> Utils::SQLQuery(const QString& sql) {//执行SQL查询语句
	QList<QStringList> result;
	QSqlQuery q(sql, getDBObj());//准备查询
	q.exec();//执行查询
	const int columnCount = q.record().count();//查询结果列数
	if (columnCount<1)//列数为0？（意味着查询结果为空）
		return result;//返回空表
	for (int count = 0, i; q.next(); ++count) {
		result.append(QStringList());//增加新行
		for (i = 0; i < columnCount; ++i)
			result[count].append(q.value(i).toString());//填入查询结果
	}
	return result;//返回结果表
}
bool Utils::SQLExcecute(const QString& sql) {//执行除查询以外的其他SQL语句
	QSqlQuery q(getDBObj());//准备执行
	if (q.exec(sql))//执行顺利？
		return true;
	auto msg = q.lastError().databaseText() % "\n" % q.lastError().driverText();
	QMessageBox::critical(nullptr, "执行错误", msg, "确定");//弹出错误提示
	return false;
}
bool Utils::SQLExcecute_Multiple(const QStringList& sql) {//执行多条除查询以外的其他SQL语句
	QSqlQuery q(getDBObj());//准备执行
	for (auto it = sql.begin(); it != sql.end(); ++it)
		if (!q.exec(*it)) {//执行出错？
			auto msg = q.lastError().databaseText() % "\n" % q.lastError().driverText();
			QMessageBox::critical(nullptr, "执行错误", msg, "确定");//弹出错误提示
			return false;//中止后续执行
		}
	return true;
}

double Utils::round(double number, const unsigned int& precisionAfterPoint) {
	long long integerpart = number;//整数部分
	number -= integerpart;
	unsigned int i;
	for (i=0; i < precisionAfterPoint; ++i)//小数点右移 precisionAfterPoint 位
		number *= 10;
	number = (long long)(number + 0.5);//四舍五入取整
	for (i=0; i < precisionAfterPoint; ++i)//小数点移回去
		number /= 10;
	return integerpart + number;
}

bool Utils::isContentInvalid(const QString& content) {
	if (content.isNull())//为空？
		return true;
	auto trimmed = QString(content);
	trimmed.remove(QRegExp("\\s"));
	if (trimmed.isEmpty())//只有空白字符？
		return true;
	return trimmed.contains("`");//含有该字符？（有可能影响到SQL查询语句或插入语句的执行）
}
bool Utils::isContentInvalid(const QStringList& content) {
	for (auto it = content.begin(); it != content.end(); ++it)
		if (isContentInvalid(*it))
			return true;
	return false;
}
bool Utils::isContentInvalid(const QList<QStringList>& content) {
	for (auto it = content.begin(); it != content.end(); ++it)
		if (isContentInvalid(*it))
			return true;
	return false;
}

QList<QStringList> Utils::fuzzySearch(const QString& keywordText, const QString& dataBaseName, const QStringList& dataBaseHeader) {
	if (isContentInvalid(keywordText))//输入框文字不合法？
		return QList<QStringList>();//返回空表
	auto keywords = QString(keywordText).split(" ", QString::SkipEmptyParts);//按空格切分关键词
	auto sql = QString("SELECT * FROM [%1]").arg(dataBaseName);//创建准备执行的SQL查询语句
	const auto headerSize = dataBaseHeader.size();//表头列数
	if (headerSize > 0) {//表头至少一列，逐表头逐关键词添加额外查询条件
		auto itHeader = dataBaseHeader.begin();
		auto itKeywords = keywords.begin();
		sql.append(QString(" WHERE [%1] like '%%2%'").arg(*itHeader, prepareDataForSQL(*itKeywords)));
		for (++itKeywords; itKeywords != keywords.end(); ++itKeywords)
			sql.append(QString(" OR [%1] like '%%2%'").arg(*itHeader, prepareDataForSQL(*itKeywords)));
		for (++itHeader; itHeader != dataBaseHeader.end(); ++itHeader)
			for (itKeywords = keywords.begin(); itKeywords != keywords.end(); ++itKeywords)
				sql.append(QString(" OR [%1] like '%%2%'").arg(*itHeader, prepareDataForSQL(*itKeywords)));
	}
	return Utils::SQLQuery(sql);//执行并返回结果
}

QString Utils::amountCalc(const QString& amountStr, const QString& prepareNumStr) {//处理数量 1×2
	const auto amount = amountStr.toDouble();
	const auto prepareNum = prepareNumStr.toDouble();
	const auto newAmount_double = (amount * prepareNum) + 0.000055;
	const auto newAmount_int = (double)((long long)(newAmount_double + 0.5));
	const auto newAmount_round = round(newAmount_double, 3);//精度:小数点后三位
	const auto DELTA = (newAmount_double - newAmount_int) * (newAmount_double - newAmount_int);
	const auto format = (DELTA < 0.00000025) ? "%.0f" : "%.3f";//精度:0.0005
	return QString::asprintf(format, newAmount_round);
}
QString Utils::amountCalc(const QString& originalAmountStr, const QString& newAmountStr, const QString& newPrepareNumStr) {//处理数量 1+(2×3)
	const auto originalAmount = originalAmountStr.toDouble();
	const auto newAmount = newAmountStr.toDouble();
	const auto newPrepareNum = newPrepareNumStr.toDouble();
	const auto newAmount_double = originalAmount + (newAmount * newPrepareNum) + 0.000055;
	const auto newAmount_round = round(newAmount_double, 3);//精度:小数点后三位
	const auto newAmount_int = (double)((long long)(newAmount_double + 0.5));
	const auto DELTA = (newAmount_double - newAmount_int) * (newAmount_double - newAmount_int);
	const auto format = (DELTA < 0.00000025) ? "%.0f" : "%.3f";//精度:0.0005
	return QString::asprintf(format, newAmount_round);
}
