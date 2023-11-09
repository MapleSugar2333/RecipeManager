#pragma once
#pragma execution_character_set("UTF-8")

#include "ui_RecipeManager.h"
#include <QtSql>
#include <QMessageBox>
#include <QDateTime>

class Utils {//���߰���
private:
	class DBConnection {//���ݿ������ࣨ�������߰���ʹ�ã�
	private:
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//���ݿ�ʵ��
	public:
		DBConnection() {};
		~DBConnection();
		bool init();//��ʼ�����ݿ�
		inline QSqlDatabase& getDBObj();//��ȡ���ݿ�ʵ��
	};
	static DBConnection conn;//���ݿ�����ʵ��

public:
	static inline QString getAppDataPath();//��ȡ��������·��
	static QString createID();//���ݵ�ǰʱ�䴴��ID
	static QString getTimeFromID(const QString& ID);//����ΪID��ʱ���ʽת��Ϊ�û��׶��ĸ�ʽ

	static bool initDBConnection();//��ʼ�����ݿ�����
	static QSqlDatabase& getDBObj();//��ȡ���ݿ�ʵ��

	static inline QString prepareDataForSQL(const QString& original) {
		return QString(original).replace("'", "''");//����׼�������ݿ⽻��������
	}
	static bool SQLInsert(const QString& tableName, const QStringList& header, const QStringList& value);//ִ��SQL�������
	static bool SQLInsert_Multiple(const QString& tableName, const QStringList& header, const QList<QStringList>& value);//ִ��SQL�����������
	static QList<QStringList> SQLQuery(const QString& sql);//ִ��SQL��ѯ���
	static bool SQLExcecute(const QString& sql);//ִ�г���ѯ���������SQL���
	static bool SQLExcecute_Multiple(const QStringList& sql);//ִ�ж�������ѯ���������SQL���

	static double round(double number, const unsigned int& precisionAfterPoint);//С�����nλ��������

	static bool isContentInvalid(const QString& content);//���ݺϹ��Լ�飬��ͬ
	static bool isContentInvalid(const QStringList& content);
	static bool isContentInvalid(const QList<QStringList>& content);

	static QList<QStringList> fuzzySearch(const QString& keywordText, const QString& dataBaseName, const QStringList& dataBaseHeader);//��ؼ��ʶ����Բ���

	static QString amountCalc(const QString&, const QString&);//�������� 1��2
	static QString amountCalc(const QString&, const QString&, const QString&);//�������� 1+(2��3)
};
