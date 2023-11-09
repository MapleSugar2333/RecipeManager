#include "Utils.h"

QString Utils::getAppDataPath() {//��ȡӦ������·��
	return QApplication::applicationDirPath() % "/RecipeManager/";
}
QString Utils::createID() {//����ǰʱ��ת��Ϊ��ΪID�ĸ�ʽ ʾ��:20230801120000
	return QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
}
QString Utils::getTimeFromID(const QString& ID) {//����ΪID��ʱ���ʽת��Ϊ�û��׶��ĸ�ʽ ʾ��:2023.08.01 12:00:00
	return QString("%1.%2.%3 %4:%5:%6").arg(ID.mid(0,4),ID.mid(4,2),ID.mid(6,2),ID.mid(8,2),ID.mid(10,2),ID.mid(12,2));
}

Utils::DBConnection Utils::conn = DBConnection();//���ݿ�����ʵ��
Utils::DBConnection::~DBConnection(){
	db.close();//�ر����ݿ�����
}
QSqlDatabase& Utils::DBConnection::getDBObj(){
	return db;//�������ݿ�ʵ��
}

bool Utils::DBConnection::init() {//��ʼ�����ݿ�
	auto path = Utils::getAppDataPath();//��ȡӦ�������ļ���·��
	if (!QDir(path).exists())//Ӧ�������ļ��в����ڣ�
		if (!QDir().mkdir(path)) {//Ӧ�������ļ��д���ʧ�ܣ�
			QString msg = "<font size='9'>�����ļ��д���ʧ��!<br/>�����ļ����Ƿ���������ռ��!</font>";
			QMessageBox::critical(nullptr, "��ʼ������", msg, "ȷ��");//������ʾ
			return false;//��ʼ��ʧ��
		}
	db.setDatabaseName(path % "RecipeManager.db");//�������ݿ�����
	if (!db.open()) {//���ݿ��ļ�����/��ʧ�ܣ������ݿⲻ���ڵ�����»����Զ��������ݿⲢ�򿪣�������ֱ�Ӵ򿪣�
		QString msg = "<font size='9'>���ݿ��ʧ��!<br/>�������ݿ��ļ��������Լ��Ƿ���������ռ��!</font>";
		QMessageBox::critical(nullptr, "���ݿ����", msg, "ȷ��");//������ʾ
		return false;//��ʼ��ʧ��
	}
	return Utils::SQLExcecute_Multiple({//ִ�����ݿ��ʼ����䲢����ִ�н��
		QLatin1String("PRAGMA foreign_keys = off"),
		QLatin1String("BEGIN TRANSACTION"),
		QLatin1String("CREATE TABLE IF NOT EXISTS Recipe(name nvarchar(50) NOT NULL, ID char(14) NOT NULL PRIMARY KEY) WITHOUT ROWID"),
		QLatin1String("CREATE TABLE IF NOT EXISTS Ingredient (RecipeID char (14) NOT NULL, name nvarchar (50) NOT NULL, amount nvarchar (20) NOT NULL, unit nvarchar (50) NOT NULL, PRIMARY KEY (RecipeID, name, unit), FOREIGN KEY (RecipeID) REFERENCES Recipe (ID) ON DELETE CASCADE ON UPDATE CASCADE) WITHOUT ROWID"),
		QLatin1String("COMMIT TRANSACTION"),
		QLatin1String("PRAGMA foreign_keys = on")
	});
}

bool Utils::initDBConnection() {
	return conn.init();//��ʼ�����ݿ�
}
QSqlDatabase& Utils::getDBObj(){
	return conn.getDBObj();//�������ݿ�ʵ��
}

bool Utils::SQLInsert(const QString& tableName, const QStringList& header, const QStringList& value) {//ִ��SQL�������
	if (!header.size() || (header.size() != value.size()))//ȷ����ͷ��ֵ�ĸ�������Ϊ0�����
		return false;
	auto itHeader = header.begin();
	auto itValue = value.begin();
	auto sql_ins = QString("INSERT INTO [%1] ([%2]").arg(tableName, *itHeader);//SQL�������ǰ�벿��
	auto sql_val = QString(") VALUES ('%1'").arg(prepareDataForSQL(*itValue));//SQL��������벿��
	for (++itValue, ++itHeader; itValue != value.end(); ++itValue, ++itHeader) {
		sql_ins.append(QString(", [%1]").arg(*itHeader));//���ӱ�ͷ
		sql_val.append(QString(", '%1'").arg(prepareDataForSQL(*itValue)));//����ֵ
	}
	return Utils::SQLExcecute(sql_ins.append(sql_val).append(')'));//ƴ��SQL��䣬ִ�в����ؽ����
}
bool Utils::SQLInsert_Multiple(const QString& tableName, const QStringList& header, const QList<QStringList>& value) {//ִ��SQL�����������
	if (!value.size() || (header.size() != value[0].size()))//ȷ����ͷ��ֵ�ĸ�������Ϊ0�����
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
QList<QStringList> Utils::SQLQuery(const QString& sql) {//ִ��SQL��ѯ���
	QList<QStringList> result;
	QSqlQuery q(sql, getDBObj());//׼����ѯ
	q.exec();//ִ�в�ѯ
	const int columnCount = q.record().count();//��ѯ�������
	if (columnCount<1)//����Ϊ0������ζ�Ų�ѯ���Ϊ�գ�
		return result;//���ؿձ�
	for (int count = 0, i; q.next(); ++count) {
		result.append(QStringList());//��������
		for (i = 0; i < columnCount; ++i)
			result[count].append(q.value(i).toString());//�����ѯ���
	}
	return result;//���ؽ����
}
bool Utils::SQLExcecute(const QString& sql) {//ִ�г���ѯ���������SQL���
	QSqlQuery q(getDBObj());//׼��ִ��
	if (q.exec(sql))//ִ��˳����
		return true;
	auto msg = q.lastError().databaseText() % "\n" % q.lastError().driverText();
	QMessageBox::critical(nullptr, "ִ�д���", msg, "ȷ��");//����������ʾ
	return false;
}
bool Utils::SQLExcecute_Multiple(const QStringList& sql) {//ִ�ж�������ѯ���������SQL���
	QSqlQuery q(getDBObj());//׼��ִ��
	for (auto it = sql.begin(); it != sql.end(); ++it)
		if (!q.exec(*it)) {//ִ�г���
			auto msg = q.lastError().databaseText() % "\n" % q.lastError().driverText();
			QMessageBox::critical(nullptr, "ִ�д���", msg, "ȷ��");//����������ʾ
			return false;//��ֹ����ִ��
		}
	return true;
}

double Utils::round(double number, const unsigned int& precisionAfterPoint) {
	long long integerpart = number;//��������
	number -= integerpart;
	unsigned int i;
	for (i=0; i < precisionAfterPoint; ++i)//С�������� precisionAfterPoint λ
		number *= 10;
	number = (long long)(number + 0.5);//��������ȡ��
	for (i=0; i < precisionAfterPoint; ++i)//С�����ƻ�ȥ
		number /= 10;
	return integerpart + number;
}

bool Utils::isContentInvalid(const QString& content) {
	if (content.isNull())//Ϊ�գ�
		return true;
	auto trimmed = QString(content);
	trimmed.remove(QRegExp("\\s"));
	if (trimmed.isEmpty())//ֻ�пհ��ַ���
		return true;
	return trimmed.contains("`");//���и��ַ������п���Ӱ�쵽SQL��ѯ�����������ִ�У�
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
	if (isContentInvalid(keywordText))//��������ֲ��Ϸ���
		return QList<QStringList>();//���ؿձ�
	auto keywords = QString(keywordText).split(" ", QString::SkipEmptyParts);//���ո��зֹؼ���
	auto sql = QString("SELECT * FROM [%1]").arg(dataBaseName);//����׼��ִ�е�SQL��ѯ���
	const auto headerSize = dataBaseHeader.size();//��ͷ����
	if (headerSize > 0) {//��ͷ����һ�У����ͷ��ؼ�����Ӷ����ѯ����
		auto itHeader = dataBaseHeader.begin();
		auto itKeywords = keywords.begin();
		sql.append(QString(" WHERE [%1] like '%%2%'").arg(*itHeader, prepareDataForSQL(*itKeywords)));
		for (++itKeywords; itKeywords != keywords.end(); ++itKeywords)
			sql.append(QString(" OR [%1] like '%%2%'").arg(*itHeader, prepareDataForSQL(*itKeywords)));
		for (++itHeader; itHeader != dataBaseHeader.end(); ++itHeader)
			for (itKeywords = keywords.begin(); itKeywords != keywords.end(); ++itKeywords)
				sql.append(QString(" OR [%1] like '%%2%'").arg(*itHeader, prepareDataForSQL(*itKeywords)));
	}
	return Utils::SQLQuery(sql);//ִ�в����ؽ��
}

QString Utils::amountCalc(const QString& amountStr, const QString& prepareNumStr) {//�������� 1��2
	const auto amount = amountStr.toDouble();
	const auto prepareNum = prepareNumStr.toDouble();
	const auto newAmount_double = (amount * prepareNum) + 0.000055;
	const auto newAmount_int = (double)((long long)(newAmount_double + 0.5));
	const auto newAmount_round = round(newAmount_double, 3);//����:С�������λ
	const auto DELTA = (newAmount_double - newAmount_int) * (newAmount_double - newAmount_int);
	const auto format = (DELTA < 0.00000025) ? "%.0f" : "%.3f";//����:0.0005
	return QString::asprintf(format, newAmount_round);
}
QString Utils::amountCalc(const QString& originalAmountStr, const QString& newAmountStr, const QString& newPrepareNumStr) {//�������� 1+(2��3)
	const auto originalAmount = originalAmountStr.toDouble();
	const auto newAmount = newAmountStr.toDouble();
	const auto newPrepareNum = newPrepareNumStr.toDouble();
	const auto newAmount_double = originalAmount + (newAmount * newPrepareNum) + 0.000055;
	const auto newAmount_round = round(newAmount_double, 3);//����:С�������λ
	const auto newAmount_int = (double)((long long)(newAmount_double + 0.5));
	const auto DELTA = (newAmount_double - newAmount_int) * (newAmount_double - newAmount_int);
	const auto format = (DELTA < 0.00000025) ? "%.0f" : "%.3f";//����:0.0005
	return QString::asprintf(format, newAmount_round);
}
