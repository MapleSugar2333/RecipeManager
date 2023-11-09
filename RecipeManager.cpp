#include "RecipeManager.h"

RecipeManager::RecipeManager(QWidget* parent) :QWidget(parent) {
	ui.setupUi(this);
	tableItemTemplate.setTextAlignment(Qt::AlignCenter);//设置表格内容默认居中
	//以下手动链接食材管理页菜谱名称框的文字改变信号和槽函数
	QObject::connect(ui.nameBox_DetailPage, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_DetailPage_Changed()));
	//以下手动链接食材添加/删除页三个输入框的文字改变信号和槽函数
	QObject::connect(ui.nameBox_IngredientPage, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_IngredientPage_Changed()));
	QObject::connect(ui.amountBox, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_IngredientPage_Changed()));
	QObject::connect(ui.unitBox, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_IngredientPage_Changed()));
}


void RecipeManager::setupManagePage() {
	int i, j;
	auto table = ui.recipeTable;
	table->setSortingEnabled(false);//临时关闭排序
	if (!isManagePageInited) {//菜谱管理页面未初始化？
		ui.searchBox_ManagePage->setText("");//清理搜索框
		ui.addButton_ManagePage->setHidden(ReadOnly);//只读模式下隐藏添加按钮，管理模式下显示
		ui.deleteButton_ManagePage->setHidden(ReadOnly);//只读模式下隐藏删除按钮，管理模式下显示
		table->hideColumn(1);//隐藏ID列
		table->setColumnWidth(0, 600);//设置名称列宽度
		table->setColumnWidth(2, 300);//设置时间列宽度
		auto content = Utils::SQLQuery("select * from [Recipe]");//获取菜谱表中的所有数据
		const auto contentSize = content.size();//结果行数
		table->setRowCount(contentSize);//设置表格行数
		if (contentSize) //行数大于0？
			for (i = 0; i < contentSize; ++i) {
				content[i].append(Utils::getTimeFromID(content[i][1]));//将通过ID得到的时间（新起的一列）附加至菜谱表行的末尾
				for (j = 0; j < 3; ++j) {
					tableItemTemplate.setText(content[i][j]);//模板填入内容
					table->setItem(i, j, tableItemTemplate.clone());//将副本填入表视图
				}
			}
		table->update();//刷新视图
		isManagePageInited = true;//设置初始化标识
	}
	table->setSortingEnabled(true);//重新开启排序
	auto header = table->horizontalHeader();
	header->setSortIndicator(2, Qt::AscendingOrder);//设置排序的列
	header->setSortIndicatorShown(true);//显示排序箭头
	table->setCurrentCell(0, 0);//设置当前选中的行
	ui.deleteButton_ManagePage->setEnabled(table->rowCount());//管理模式下菜谱表中有数据才启用删除按钮，反之则禁用
	ui.detailButton->setEnabled(table->rowCount());//菜谱表中有数据才启用详细信息按钮，反之则禁用
}

void RecipeManager::setupDetailPage() {
	int i, j;
	auto table = ui.ingredientTable;
	table->setSortingEnabled(false);//临时关闭排序
	if (!isDetailPageInited) {//食材管理页面未初始化？
		ui.searchBox_DetailPage->setText("");//清理搜索框
		ui.footer_DetailPage->setHidden(AddMode_Recipe);//添加菜谱模式下隐藏底部控件，管理模式下显示
		table->setHidden(AddMode_Recipe);//添加菜谱模式下隐藏食材表，管理模式下显示
		table->hideColumn(0);//隐藏ID列
		ui.header2Right_DetailPage->setHidden(ReadOnly || AddMode_Recipe);//只读模式或添加菜谱模式下隐藏食材操作按钮，管理模式下显示
		ui.header2Left_DetailPage->setFixedWidth(ReadOnly ? 940 : 590);//只读模式下将第二行控件扩宽填满右部，管理模式下复原
		ui.header2Left_DetailPage->setHidden(AddMode_Recipe);//添加菜谱模式下隐藏搜索控件，只读模式或管理模式下显示
		ui.header1Right_DetailPage->setHidden(ReadOnly);//只读模式下隐藏保存按钮，管理模式下显示
		ui.header1Left_DetailPage->setFixedWidth(ReadOnly ? 940 : 710);//只读模式下将第一行控件扩宽填满右部，管理模式下复原
		ui.nameBox_DetailPage->setDisabled(ReadOnly);//只读模式才禁止名称空的填入，反之则启用
		if (!AddMode_Recipe) {//管理食材
			auto recipeTable = ui.recipeTable;
			ui.nameBox_DetailPage->setText(recipeTable->item(recipeTable->currentRow(), 0)->text());//名称空中填入菜谱表中选中行的菜谱名称
			needToModify_Manage = false;//重置需要修改标识
			ui.timeBox->setText(recipeTable->item(recipeTable->currentRow(), 2)->text());//时间空中填入菜谱表中选中行的菜谱时间
			auto sql = QString("select * from [Ingredient] where [RecipeID] = '%1'").arg(recipeTable->item(recipeTable->currentRow(), 1)->text());
			auto content = Utils::SQLQuery(sql);//获取食材表中ID为选中行的所有数据
			const auto contentSize = content.size();//结果行数
			table->setRowCount(contentSize);//设置表格行数
			if (contentSize) //行数大于0？
				for (i = 0; i < contentSize; ++i)
					for (j = 0; j < 4; ++j) {
						tableItemTemplate.setText(content[i][j]);//模板填入内容
						table->setItem(i, j, tableItemTemplate.clone());//填入表视图
					}
			table->update();//刷新视图
		}
		isDetailPageInited = true;//设置初始化标识
	}
	table->setSortingEnabled(true);//重新开启排序
	auto header = table->horizontalHeader();
	header->setSortIndicator(1, Qt::AscendingOrder);//设置排序的列
	header->setSortIndicatorShown(true);//显示排序箭头
	table->setCurrentCell(0, 0);//设置当前选中的行
	ui.deleteButton_DetailPage->setEnabled(table->rowCount());//管理模式下食材表中有数据才启用删除按钮，反之则禁用
	ui.ingredientButton->setEnabled(table->rowCount());//表格中有数据才启用详细信息按钮，反之则禁用
}

void RecipeManager::setupIngredientPage() {
	auto table = ui.ingredientTable;
	ui.nameBox_IngredientPage->setText(AddMode_Ingredient ? "" : table->item(table->currentRow(), 1)->text());//根据模式设置初始值，下同
	ui.amountComboBox->setCurrentIndex(AddMode_Ingredient ? 0 : table->item(table->currentRow(), 2)->text() == "适量");
	needToModify_Detail = false;//重置需要修改标识
}

void RecipeManager::setupPrepareMainPage() {
	auto table = ui.prepareTable;
	table->setSortingEnabled(false);//临时关闭排序
	if (!isPrepareMainPageInited) {//备菜主页面未初始化？
		ui.searchBox_PrepareMainPage->setText("");//清理搜索框
		Utils::SQLExcecute_Multiple({
			QLatin1String("DROP TABLE IF EXISTS Prepare"),//若有数量表残留，删除
			QLatin1String("CREATE TABLE Prepare(ID char(14) NOT NULL PRIMARY KEY REFERENCES Recipe (ID) ON DELETE CASCADE ON UPDATE CASCADE, prepareNum integer not null) WITHOUT ROWID"),//重新创建数量表
			QLatin1String("INSERT INTO Prepare SELECT ID,0 FROM Recipe")//填入数据
			});
		table->hideColumn(1);//隐藏ID列
		table->setColumnWidth(0, 500);//设置名称列宽度
		table->setColumnWidth(2, 250);//设置时间列宽度
		table->setColumnWidth(3, 150);//设置数量列宽度
		isPrepareMainPageInited = true;//设置初始化标识
	}
	else if (needToModify_Prepare) {//需要修改
		int i, j;
		ui.searchBox_PrepareMainPage->setText("");//清理搜索框
		const auto sql = QLatin1String("select [name], [ID], [prepareNum] from [Recipe] join [Prepare] using (ID) where [prepareNum] != 0");
		auto content = Utils::SQLQuery(sql);//获取备菜表的所有数据
		const auto contentSize = content.size();//结果行数
		table->setRowCount(contentSize);//设置表格行数
		if (contentSize) {//行数大于0？
			for (i = 0; i < contentSize; ++i) {
				content[i].insert(2, Utils::getTimeFromID(content[i][1]));//将通过ID得到的时间（新起的一列）附加至ID列后
				for (j = 0; j < 4; ++j) {
					tableItemTemplate.setText(content[i][j]);//模板填入内容
					table->setItem(i, j, tableItemTemplate.clone());//填入表视图
				}
			}
			table->setCurrentCell(0, 0);//重置选中项
		}
		needToModify_Prepare = false;//重置需要修改标识
	}
	table->update();//刷新视图
	table->setSortingEnabled(true);//重新开启排序
	auto header = table->horizontalHeader();
	header->setSortIndicator(2, Qt::AscendingOrder);//设置排序的列
	header->setSortIndicatorShown(true);//显示排序箭头
	ui.deleteButton_PrepareMainPage->setEnabled(table->rowCount());//表中有数据才启用删除按钮，反之则禁用
	ui.prepareDetailButton->setEnabled(table->rowCount());//表格中有数据才启用编辑按钮，反之则禁用
	ui.prepareResultButton->setEnabled(table->rowCount());//表格中有数据才启用食材用量计算按钮，反之则禁用
}

void RecipeManager::setupPrepareDetailPage() {
	int i, j;
	auto table = ui.prepareDetailTable;
	auto sql = QLatin1String("select [name], [ID], [prepareNum] from [Recipe] join [Prepare] using (ID) where [prepareNum] = 0");
	QList<QStringList> content;
	table->setSortingEnabled(false);//临时关闭排序
	if (!isPrepareDetailPageInited) {//备菜详情页未初始化？
		ui.searchBox_PrepareDetailPage->setText("");//清理搜索框
		table->hideColumn(1);//隐藏ID列
		table->setColumnWidth(0, 500);//设置名称列宽度
		table->setColumnWidth(2, 250);//设置时间列宽度
		table->setColumnWidth(3, 150);//设置数量列宽度
		content = Utils::SQLQuery(sql);//获取备菜表的所有数据
		isPrepareDetailPageInited = true;//设置初始化标识
	}
	const auto contentSize = content.size();//结果行数
	table->setRowCount(contentSize);//设置表格行数
	if (contentSize) {//行数大于0？
		for (i = 0; i < contentSize; ++i) {
			content[i].insert(2, Utils::getTimeFromID(content[i][1]));//将通过ID得到的时间（新起的一列）附加至ID列后
			for (j = 0; j < 4; ++j) {
				tableItemTemplate.setText(content[i][j]);//模板填入内容
				table->setItem(i, j, tableItemTemplate.clone());//填入表视图
			}
		}
		table->setCurrentCell(0, 0);//设置当前选中的行
	}
	if (!AddMode_Prepare) {//管理备菜
		table->setRowCount(contentSize + 1);//设置表格行数
		auto prepareTable = ui.prepareTable;
		const auto curRow = prepareTable->currentRow();
		for (i = 0; i < 4; ++i)
			table->setItem(contentSize, i, prepareTable->item(curRow, i)->clone());//填入表视图
		table->setCurrentCell(contentSize, 0);//重设当前选中的行
	}
	table->setColumnHidden(3, AddMode_Prepare);//添加模式下隐藏数量列
	table->update();//刷新视图
	table->setSortingEnabled(true);//重新开启排序
	auto header = table->horizontalHeader();
	header->setSortIndicator(2, Qt::AscendingOrder);//设置排序的列
	header->setSortIndicatorShown(true);//显示排序箭头
	ui.amountSpinBox->setValue(table->rowCount() ? table->item(table->currentRow(), 3)->text().toInt() : 0);//设置数量
	ui.okButton_PrepareDetailPage->setEnabled(table->rowCount());//表格中有数据才启用确定添加/修改按钮，反之则禁用
}


void RecipeManager::on_pagesCurrentIndex_Changed(int currentIndex) {
	switch (currentIndex) {
	case MANAGE_PAGE_INDEX:
		setupManagePage();
		break;
	case DETAIL_PAGE_INDEX:
		setupDetailPage();
		break;
	case INGREDIENT_PAGE_INDEX:
		setupIngredientPage();
		break;
	case PREPARE_MAIN_PAGE_INDEX:
		setupPrepareMainPage();
		break;
	case PREPARE_DETAIL_PAGE_INDEX:
		setupPrepareDetailPage();
		break;
	default:
		break;
	}
}


void RecipeManager::on_viewButton_Released() {
	ReadOnly = true;//设置为只读模式
	ui.pages->setCurrentIndex(MANAGE_PAGE_INDEX);//跳转到菜谱管理界面
}
void RecipeManager::on_manageButton_Released() {
	ReadOnly = false;//设置为可修改模式
	ui.pages->setCurrentIndex(MANAGE_PAGE_INDEX);//跳转到菜谱管理界面
}
void RecipeManager::on_prepareButton_Released() {
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//跳转到备菜主界面
}


void RecipeManager::on_backButton_ManagePage_Released() {
	isManagePageInited = false;//重置初始化标识
	ui.recipeTable->setRowCount(0);
	ui.pages->setCurrentIndex(MAIN_PAGE_INDEX);//回到主界面
}
void RecipeManager::on_searchButton_ManagePage_Released() {
	//搜索
}
void RecipeManager::on_addButton_ManagePage_Released() {
	AddMode_Recipe = true;//设置为添加模式
	ui.pages->setCurrentIndex(DETAIL_PAGE_INDEX);//跳转到食材管理界面
}
void RecipeManager::on_deleteButton_ManagePage_Released() {
	const auto msg = "<font size='9'>确定要删除该菜谱吗？</font>";
	if (!QMessageBox::warning(nullptr, "删除确认", msg, "确定", "取消", "", 1)) {//删除菜谱
		auto table = ui.recipeTable;
		const auto i = table->currentRow();
		const auto sql = QString("delete from [Recipe] where [ID] = '%1'").arg(table->item(i, 1)->text());
		Utils::SQLExcecute(sql);
		isManagePageInited = false;//重置初始化标识
		setupManagePage();
		table->setCurrentCell(i - (i > 0), 0);
	}
}
void RecipeManager::on_detailButton_Released() {
	AddMode_Recipe = false;//设置为管理模式
	ui.pages->setCurrentIndex(DETAIL_PAGE_INDEX);//跳转到食材管理界面
}


void RecipeManager::on_backButton_DetailPage_Released() {
	const auto msg = "<font size='9'>确定放弃所有更改并退出吗？</font>";
	if (ReadOnly || !needToModify_Manage || QMessageBox::question(nullptr, "退出确认", msg, "取消", "确定", "", 0)) {
		ui.nameBox_DetailPage->setText("");//清空名称框
		ui.ingredientTable->setRowCount(0);//重置表格状态
		isDetailPageInited = false;//重置初始化标识
		ui.pages->setCurrentIndex(MANAGE_PAGE_INDEX);//回到菜谱管理界面
	}
}
void RecipeManager::on_searchButton_DetailPage_Released() {
	//搜索
}
void RecipeManager::on_addButton_DetailPage_Released() {
	AddMode_Ingredient = true;//设置为添加模式
	ui.pages->setCurrentIndex(INGREDIENT_PAGE_INDEX);//跳转到食材添加/编辑界面
}
void RecipeManager::on_deleteButton_DetailPage_Released() {
	const auto msg = "<font size='9'>确定要删除该食材吗？</font>";
	if (!QMessageBox::warning(nullptr, "删除确认", msg, "确定", "取消", "", 1)) {//删除食材
		auto table = ui.ingredientTable;
		const auto i = table->currentRow();
		const auto RecipeID = table->item(i, 0)->text();
		const auto name = Utils::prepareDataForSQL(table->item(i, 1)->text());
		const auto unit = Utils::prepareDataForSQL(table->item(i, 3)->text());
		const auto sql = QString("delete from [Ingredient] where (RecipeID, name, unit) = ('%1', '%2', '%3')");
		Utils::SQLExcecute(sql.arg(RecipeID, name, unit));
		isDetailPageInited = false;//重置初始化标识
		setupDetailPage();
		table->setCurrentCell(i - (i > 0), 0);
	}
}
void RecipeManager::on_saveButton_DetailPage_Released() {
	const auto name = ui.nameBox_DetailPage->text();
	if (Utils::isContentInvalid(name))
		return (void)QMessageBox::critical(nullptr, "内容错误", "<font size='9'>名称不能为空且不能含有以下非法字符<br/>`</font>", "确定");
	auto table = ui.recipeTable;
	if (AddMode_Recipe) {//添加菜谱模式？
		const auto ID = Utils::createID();
		const auto i = table->rowCount();
		Utils::SQLInsert(QLatin1String("Recipe"), { QLatin1String("ID"), QLatin1String("name") }, { ID, name });
		isManagePageInited = false;//重置初始化标识
		setupManagePage();
		table->setCurrentCell(i, 2);
		AddMode_Recipe = false;//重置需要增加标识
		ui.header2Left_DetailPage->show();//恢复搜索控件显示
		ui.header2Right_DetailPage->show();//恢复食材操作按钮显示
		ui.ingredientTable->show();//恢复食材表显示
		ui.timeBox->setText(table->currentItem()->text());//设置时间栏文字
		ui.footer_DetailPage->show();//恢复底部控件显示
	}
	else {//管理菜谱模式
		const auto ID = table->item(table->currentRow(), 1)->text();
		Utils::SQLExcecute(QString("update [Recipe] set [name] = '%1' where [ID] = '%2'").arg(Utils::prepareDataForSQL(name), ID));
		table->item(table->currentRow(), 0)->setText(name);//在表视图中修改对应行的数据
	}
	table->update();//刷新视图
	needToModify_Manage = false;//重置需要修改标识
}
void RecipeManager::on_ingredientButton_Released() {
	AddMode_Ingredient = false;//设置为管理模式
	ui.pages->setCurrentIndex(INGREDIENT_PAGE_INDEX);//跳转到食材添加/编辑界面
}
void RecipeManager::on_boxContent_DetailPage_Changed() {
	needToModify_Manage = true;//设置需要修改标识
}


void RecipeManager::exitIngredient() {//从食材添加/修改页面返回
	needToModify_Detail = false;//重置需要修改标识
	ui.amountComboBox->setCurrentIndex(-1);//重置复选框
	ui.pages->setCurrentIndex(DETAIL_PAGE_INDEX);//回到食材管理界面
}

void RecipeManager::on_backButton_IngredientPage_Released() {
	if (needToModify_Detail) {//需要修改
		const auto msg = "<font size='9'>确定放弃所有更改并退出吗？</font>";
		if (QMessageBox::question(nullptr, "退出确认", msg, "取消", "确定", "", 0))
			exitIngredient();//从食材添加/修改页面返回
	}
	else
		exitIngredient();//从食材添加/修改页面返回
}
void RecipeManager::on_amountComboBox_Changed(int suitableAmount) {
	if (~suitableAmount) {//非重置(!=-1)
		auto table = ui.ingredientTable;
		if (suitableAmount) {//选中"适量"
			ui.amountBox->setText("适量");
			ui.unitBox->setText("-");
		}
		else if (AddMode_Ingredient) {//选中"定量"且为添加食材模式
			ui.amountBox->setText("");
			ui.unitBox->setText("");
		}
		else {//选中"定量"且为编辑食材模式
			const auto historyAmount = table->item(table->currentRow(), 2)->text();
			if (historyAmount == "适量") {//原始记录为适量
				ui.amountBox->setText("");
				ui.unitBox->setText("");
			}
			else {//原始记录为定量
				ui.amountBox->setText(historyAmount);
				ui.unitBox->setText(table->item(table->currentRow(), 3)->text());
			}
		}
		ui.amountBox->setDisabled(suitableAmount);//选中适量时禁用编辑，定量时启用，下同
		ui.unitBox->setDisabled(suitableAmount);
	}
}
void RecipeManager::on_okButton_IngredientPage_Released() {
	if (!needToModify_Detail)
		return exitIngredient();//不需要修改则直接退出
	const auto name = ui.nameBox_IngredientPage->text();
	if (Utils::isContentInvalid(name))
		return (void)QMessageBox::critical(nullptr, "内容错误", "<font size='9'>名称不能为空且不能含有以下非法字符<br/>`</font>", "确定");
	auto amount = ui.amountBox->text();
	if (Utils::isContentInvalid(amount))
		return (void)QMessageBox::critical(nullptr, "内容错误", "<font size='9'>数量不能为空且不能含有以下非法字符<br/>`</font>", "确定");
	const auto unitBox = ui.unitBox->text();
	QString unit;
	if (amount == "适量")
		unit = QLatin1String("-");
	else {
		unit = unitBox;
		if (Utils::isContentInvalid(unitBox))
			return (void)QMessageBox::critical(nullptr, "内容错误", "<font size='9'>单位不能为空且不能含有以下非法字符<br/>`</font>", "确定");
		const auto amount_double = amount.toDouble() + 0.000055;
		const auto amount_int = (double)((long long)(amount_double + 0.5));
		const auto amount_round = Utils::round(amount_double, 3);//精度:小数点后三位
		if (amount_int < 1)
			if (amount_round < 0.0005)
				return (void)QMessageBox::critical(nullptr, "数值错误", "<font size='9'>请输入合法且大于0的阿拉伯数字(支持小数)！</font>", "确定");
		const auto DELTA = (amount_double - amount_int) * (amount_double - amount_int);
		const auto format = (DELTA < 0.00000025) ? "%.0f" : "%.3f";//取整阈值:0.0005
		amount.sprintf(format, amount_round);
	}
	auto table = ui.ingredientTable;
	const auto recipeTable = ui.recipeTable;
	const auto RecipeID = recipeTable->item(recipeTable->currentRow(), 1)->text();
	if (table->rowCount()) {//菜谱存在不止一条食材数据
		const auto nameSQL = Utils::prepareDataForSQL(name);
		const auto unitSQL = Utils::prepareDataForSQL(unitBox);
		QString oldName, oldUnit;
		if (!AddMode_Ingredient) {
			oldName = Utils::prepareDataForSQL(table->item(table->currentRow(), 1)->text());
			oldUnit = Utils::prepareDataForSQL(table->item(table->currentRow(), 3)->text());
		}
		auto sql = QString("select * from [Ingredient] where (RecipeID, name, unit) = ('%1', '%2', '%3')").arg(RecipeID, nameSQL, unitSQL);
		auto result = Utils::SQLQuery(sql);
		if (!result.isEmpty() && (AddMode_Ingredient || oldName != nameSQL || oldUnit != unitSQL)) //与现有数据冲突
			return (void)QMessageBox::critical(nullptr, "已经存在", "<font size='9'>已经存在食材与所填项的名称和单位相同！</font>", "确定");
		if (AddMode_Ingredient) //添加食材？
			AddMode_Ingredient = false;//重置需要增加标识
		else {
			sql = QString("delete from [Ingredient] where (RecipeID, name, unit) = ('%2', '%3', '%4')");
			Utils::SQLExcecute(sql.arg(RecipeID, oldName, oldUnit));
		}
	}
	const auto i = table->currentRow() + AddMode_Ingredient;
	Utils::SQLInsert(
		QLatin1String("Ingredient"),
		{ QLatin1String("RecipeID"), QLatin1String("name"), QLatin1String("amount"), QLatin1String("unit") },
		{ RecipeID, name, amount, unit }
	);
	isDetailPageInited = false;//重置初始化标识
	setupDetailPage();
	table->update();
	table->setCurrentCell(i, 0);
	exitIngredient();//从食材添加/修改页面返回
}
void RecipeManager::on_cancelButton_IngredientPage_Released() {
	on_backButton_IngredientPage_Released();
}
void RecipeManager::on_boxContent_IngredientPage_Changed() {
	needToModify_Detail = true;//设置需要修改标识
}


void RecipeManager::on_backButton_PrepareMainPage_Released() {
	const auto msg = "<font size='9'>确定不计算食材用量并退出吗？</font>";
	if (!(needToCalc && QMessageBox::question(nullptr, "退出确认", msg, "确定", "取消", "", 1))) {
		isPrepareMainPageInited = false;//重置初始化状态
		needToCalc = false;//重置食材用量需要计算状态
		ui.prepareTable->setRowCount(0);//清空备菜表
		ui.prepareResultTable->setRowCount(0);//清空备菜结果表
		Utils::SQLExcecute("DROP TABLE IF EXISTS Prepare");//删除备菜表
		ui.pages->setCurrentIndex(MAIN_PAGE_INDEX);//回到主界面
	}
}
void RecipeManager::on_searchButton_PrepareMainPage_Released() {
	//搜索
}
void RecipeManager::on_addButton_PrepareMainPage_Released() {
	AddMode_Prepare = true;//设置为添加模式
	ui.pages->setCurrentIndex(PREPARE_DETAIL_PAGE_INDEX);//跳转到备菜管理界面
}
void RecipeManager::on_deleteButton_PrepareMainPage_Released() {
	const auto msg = "<font size='9'>确定不准备该菜谱吗？</font>";
	if (!QMessageBox::warning(nullptr, "删除确认", msg, "确定", "取消", "", 1)) {//删除菜谱
		auto table = ui.prepareTable;
		const auto i = table->currentRow();
		const auto sql = QString("update [Prepare] set [prepareNum] = 0 where [ID] = '%1'").arg(table->item(i, 1)->text());
		Utils::SQLExcecute(sql);
		needToModify_Prepare = true;//设置修改标识
		setupPrepareMainPage();//刷新表格
		table->setCurrentCell(i - (i > 0), 0);//重新设置选中项
	}
}
void RecipeManager::on_prepareDetailButton_Released() {
	AddMode_Prepare = false;//设置为管理模式
	ui.pages->setCurrentIndex(PREPARE_DETAIL_PAGE_INDEX);//跳转到备菜管理界面
}
void RecipeManager::on_prepareResultButton_Released() {
	calcPrepare();//计算食材用量
	ui.pages->setCurrentIndex(PREPARE_RESULT_PAGE_INDEX);//跳转到备菜结果界面
}


void RecipeManager::exitPrepareDetail() {
	needToModify_Prepare = false;//重置需要修改标识
	isPrepareDetailPageInited = false;//重置初始化标识
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//跳转到备菜主界面
}
void RecipeManager::on_backButton_PrepareDetailPage_Released() {
	const auto amount = ui.amountSpinBox->value();
	if (AddMode_Prepare && !amount) //添加模式下数量为0
		return exitPrepareDetail();//从备菜详情页面返回
	if (!AddMode_Prepare) {//编辑模式
		const auto amountStr = QString::number(amount);
		auto table = ui.prepareDetailTable;
		const auto ID = table->item(table->currentRow(), 1)->text();
		auto mainTable = ui.prepareTable;
		auto originalID = mainTable->item(mainTable->currentRow(), 1)->text();
		if (originalID == ID) {//选择备菜的ID不变
			auto originalAmount = mainTable->item(mainTable->currentRow(), 3)->text();
			if (originalAmount == amountStr)
				return exitPrepareDetail();//从备菜详情页面返回
		}
		if (!amount)
			return exitPrepareDetail();//从备菜详情页面返回
	}
	const auto msg = "<font size='9'>确定放弃所有更改并退出吗？</font>";
	if (QMessageBox::question(nullptr, "退出确认", msg, "取消", "确定", "", 0))
		exitPrepareDetail();//从备菜详情页面返回
}
void RecipeManager::on_searchButton_PrepareDetailPage_Released() {
	//搜索
}
void RecipeManager::on_okButton_PrepareDetailPage_Released() {
	const auto amount = ui.amountSpinBox->value();
	if (!AddMode_Prepare || amount) {//编辑模式/添加模式下准备数量不为0
		const auto amountStr = QString::number(amount);
		auto table = ui.prepareDetailTable;
		const auto ID = table->item(table->currentRow(), 1)->text();
		if (AddMode_Prepare) {//添加模式且准备数量不为0
			const auto sql = QString("update [Prepare] set [prepareNum] = %1 where [ID] = '%2'").arg(amountStr, ID);
			Utils::SQLExcecute(sql);
			needToModify_Prepare = true;//设置需要修改标识
		}
		else {//编辑模式
			auto mainTable = ui.prepareTable;
			auto originalID = mainTable->item(mainTable->currentRow(), 1)->text();
			if (originalID != ID) {//选择备菜的ID发生了更改
				if (amount) {//数量不为0
					const auto msg = "<font size='9'>您似乎决定更改准备的菜，确定不准备原来的菜吗？</font>";
					if (QMessageBox::warning(nullptr, "更改备菜确认", msg, "确定", "取消", "", 1))
						return;//取消更改
					Utils::SQLExcecute_Multiple({
						QString("update [Prepare] set [prepareNum] = 0 where [ID] = '%1'").arg(originalID),
						QString("update [Prepare] set [prepareNum] = %1 where [ID] = '%2'").arg(amountStr, ID)
						});
					needToModify_Prepare = true;//设置需要修改标识
				}
			}
			else if (amount) {//修改的数量不为0
				const auto sql = QString("update [Prepare] set [prepareNum] = %1 where [ID] = '%2'").arg(amountStr, ID);
				Utils::SQLExcecute(sql);
				auto mainTable = ui.prepareTable;
				mainTable->item(mainTable->currentRow(), 3)->setText(amountStr);//修改备菜主表对应项的显示值
			}
			else //修改数量为0,与删除流程相同
				on_deleteButton_PrepareMainPage_Released();
		}
	}
	isPrepareDetailPageInited = false;//重置初始化标识
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//回到备菜主界面
}
void RecipeManager::on_cancelButton_PrepareDetailPage_Released() {
	on_backButton_PrepareDetailPage_Released();
}

void RecipeManager::calcPrepare() {//计算食材用量
	const auto sql1_1 = QString("with I as (select * from [Ingredient] where [amount] != \"适量\"), ");
	const auto sql1_2 = QLatin1String("P as (select [ID] as [RecipeID], [prepareNum] from [Prepare] where [prepareNum] > 0) ");
	const auto sql1_3 = QLatin1String("select [name], [amount], [prepareNum], [unit] from [I] join [P] using (RecipeID) order by [name], [unit]");
	const auto sql1 = sql1_1 % (sql1_2 % sql1_3);
	const auto sql2_1 = QString("with I as (select [RecipeID] as [ID], [name] from [Ingredient] where [amount] = \"适量\"), ");
	const auto sql2_2 = QLatin1String("P as (select * from [Prepare] where [prepareNum] > 0) ");
	const auto sql2_3 = QLatin1String("select distinct [name] from [I] join [P] using (ID) order by [name]");
	const auto sql2 = sql2_1 % (sql2_2 % sql2_3);
	auto preciseOriginal = Utils::SQLQuery(sql1);//定量原列表
	auto moderate = Utils::SQLQuery(sql2);//适量列表
	//定量列表计算值并填入新表
	auto precise = QList<QStringList>();
	int i, j;
	if (preciseOriginal.size()) {//定量原表不为空列表
		auto itOriginalOuter = preciseOriginal.constBegin();
		auto itOriginalInner = itOriginalOuter->constBegin();
		precise.append({//填入初始元素
			*itOriginalInner,
			Utils::amountCalc(*(++itOriginalInner), *(++itOriginalInner)),
			*(++itOriginalInner)
			});
		i = 0;//定量列表最大下标
		while (++itOriginalOuter != preciseOriginal.constEnd()) {
			itOriginalInner = itOriginalOuter->constBegin();
			if ((precise[i][0] == *itOriginalInner) && (precise[i][2] == itOriginalOuter->constLast())) {
				precise[i][1] = Utils::amountCalc(precise[i][1], *(++itOriginalInner), *(++itOriginalInner));
				continue;//食材名称和单位相同，新表最后一行数量累加
			}
			precise.append({//食材名称或单位不同，新表插入新行
				*itOriginalInner,
				Utils::amountCalc(*(++itOriginalInner), *(++itOriginalInner)),
				*(++itOriginalInner)
				});
			++i;//同时最大下标增加1
		}
	}
	//结果表填入数据
	const auto preciseSize = precise.size();
	const auto moderateSize = moderate.size();
	auto table = ui.prepareResultTable;
	table->setSortingEnabled(false);//临时关闭排序
	const auto totalSize = preciseSize + moderateSize;//结果表的项数
	if (totalSize) {//结果表的项数不为0？
		table->setRowCount(totalSize);//设置结果表的项数
		i = -1;
		if (preciseSize) //定量表行数不为0？
			while (++i < preciseSize)
				for (j = 0; j < 3; ++j) {
					tableItemTemplate.setText(precise[i][j]);//模板填入内容
					table->setItem(i, j, tableItemTemplate.clone());//填入表视图
				}
		if (moderateSize) //适量表行数不为0？
			for (auto& name : moderate) {
				tableItemTemplate.setText(name.constFirst());//模板填入内容，下同
				table->setItem(i, 0, tableItemTemplate.clone());//填入表视图，下同
				tableItemTemplate.setText("适量");
				table->setItem(i, 1, tableItemTemplate.clone());
				tableItemTemplate.setText(QLatin1String("-"));
				table->setItem(i, 2, tableItemTemplate.clone());
				++i;
			}
		table->setCurrentCell(0, 0);//设置选中项
	}
	table->update();//刷新视图
	table->setSortingEnabled(true);//重新开启排序
	auto header = table->horizontalHeader();
	header->setSortIndicator(0, Qt::AscendingOrder);//设置排序的列
	header->setSortIndicatorShown(true);//显示排序箭头
}
void RecipeManager::on_backButton_PrepareResultPage_Released() {
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//回到备菜主界面
}