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
	bool ReadOnly = true;//只读模式
	bool isManagePageInited = false;//菜谱管理页面初始化情况
	bool isDetailPageInited = false;//菜谱详情页面初始化情况
	bool isPrepareMainPageInited = false;//备菜主页面初始化情况
	bool isPrepareDetailPageInited = false;//备菜详情页面初始化情况
	bool needToModify_Manage = false;//需要修改菜谱管理页数据
	bool needToModify_Detail = false;//需要修改食材管理页数据
	bool needToModify_Prepare = false;//需要修改备菜主页数据
	bool needToCalc = false;//需要重新计算食材用量数据
	bool AddMode_Recipe = false;//菜谱添加模式
	bool AddMode_Ingredient = false;//食材添加模式
	bool AddMode_Prepare = false;//备菜添加模式

	QTableWidgetItem tableItemTemplate = QTableWidgetItem();//创建表格内容模板

	void setupManagePage();//初始化菜谱管理页面
	void setupDetailPage();//初始化食材管理页面
	void setupIngredientPage();//初始化食材添加/修改页面
	void setupPrepareMainPage();//初始化备菜主页面
	void setupPrepareDetailPage();//初始化备菜选择页面

	void exitIngredient();//从食材添加/修改页面返回
	void exitPrepareDetail();//从备菜详情页面返回

	void calcPrepare();//计算食材用量

private slots://槽函数
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
