#pragma once

#include <QtWidgets/QWidget>
#include "ui_RecipeManager.h"
#include "Utils.h"

#define MAIN_PAGE_INDEX				0
#define MANAGE_PAGE_INDEX			1
#define DETAIL_PAGE_INDEX			2
#define INGREDIENT_PAGE_INDEX		3
#define PREPARE_MAIN_PAGE_INDEX		4
#define PREPARE_DETAIL_PAGE_INDEX	5
#define PREPARE_RESULT_PAGE_INDEX	6
#define SETTINGS_PAGE_INDEX			7

#define MAIN_PAGE_SLOTS \
	void on_viewButton_Released(); \
	void on_manageButton_Released(); \
	void on_prepareButton_Released();

#define MANAGE_PAGE_SLOTS \
	void on_backButton_ManagePage_Released(); \
	void on_searchButton_ManagePage_Released(); \
	void on_addButton_ManagePage_Released(); \
	void on_deleteButton_ManagePage_Released(); \
	void on_detailButton_Released();

#define DETAIL_PAGE_SLOTS \
	void on_backButton_DetailPage_Released(); \
	void on_searchButton_DetailPage_Released(); \
	void on_addButton_DetailPage_Released(); \
	void on_deleteButton_DetailPage_Released(); \
	void on_saveButton_DetailPage_Released(); \
	void on_ingredientButton_Released(); \
	void on_boxContent_DetailPage_Changed();

#define INGREDIENT_PAGE_SLOTS \
	void on_backButton_IngredientPage_Released(); \
	void on_okButton_IngredientPage_Released(); \
	void on_cancelButton_IngredientPage_Released(); \
	void on_amountComboBox_Changed(int); \
	void on_boxContent_IngredientPage_Changed();

#define PREPARE_MAIN_PAGE_SLOTS \
	void on_backButton_PrepareMainPage_Released(); \
	void on_searchButton_PrepareMainPage_Released(); \
	void on_addButton_PrepareMainPage_Released(); \
	void on_deleteButton_PrepareMainPage_Released(); \
	void on_prepareDetailButton_Released(); \
	void on_prepareResultButton_Released();

#define PREPARE_DETAIL_PAGE_SLOTS \
	void on_backButton_PrepareDetailPage_Released(); \
	void on_searchButton_PrepareDetailPage_Released(); \
	void on_okButton_PrepareDetailPage_Released(); \
	void on_cancelButton_PrepareDetailPage_Released();

#define PREPARE_RESULT_PAGE_SLOTS \
	void on_backButton_PrepareResultPage_Released();

#define SETTINGS_PAGE_SLOTS \


class RecipeManager : public QWidget
{
    Q_OBJECT

public:
    RecipeManager(QWidget *parent = nullptr);
	~RecipeManager() {};

private:
    Ui::RecipeManagerClass ui;
	bool ReadOnly = true;//ֻ��ģʽ
	bool isManagePageInited = false;//���׹���ҳ���ʼ�����
	bool isDetailPageInited = false;//��������ҳ���ʼ�����
	bool isPrepareMainPageInited = false;//������ҳ���ʼ�����
	bool isPrepareDetailPageInited = false;//��������ҳ���ʼ�����
	bool needToModify_Manage = false;//��Ҫ�޸Ĳ��׹���ҳ����
	bool needToModify_Detail = false;//��Ҫ�޸�ʳ�Ĺ���ҳ����
	bool needToModify_Prepare = false;//��Ҫ�޸ı�����ҳ����
	bool needToCalc = false;//��Ҫ���¼���ʳ����������
	bool AddMode_Recipe = false;//�������ģʽ
	bool AddMode_Ingredient = false;//ʳ�����ģʽ
	bool AddMode_Prepare = false;//�������ģʽ

	QTableWidgetItem tableItemTemplate = QTableWidgetItem();//�����������ģ��

	void setupManagePage();//��ʼ�����׹���ҳ��
	void setupDetailPage();//��ʼ��ʳ�Ĺ���ҳ��
	void setupIngredientPage();//��ʼ��ʳ�����/�޸�ҳ��
	void setupPrepareMainPage();//��ʼ��������ҳ��
	void setupPrepareDetailPage();//��ʼ������ѡ��ҳ��

	void exitIngredient();//��ʳ�����/�޸�ҳ�淵��
	void exitPrepareDetail();//�ӱ�������ҳ�淵��

	void calcPrepare();//����ʳ������

private slots://�ۺ���
	void on_pagesCurrentIndex_Changed(int index);
	MAIN_PAGE_SLOTS
		MANAGE_PAGE_SLOTS
		DETAIL_PAGE_SLOTS
		INGREDIENT_PAGE_SLOTS
		PREPARE_MAIN_PAGE_SLOTS
		PREPARE_DETAIL_PAGE_SLOTS
		PREPARE_RESULT_PAGE_SLOTS
		SETTINGS_PAGE_SLOTS
};
