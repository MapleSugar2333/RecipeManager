#include "RecipeManager.h"

RecipeManager::RecipeManager(QWidget* parent) :QWidget(parent) {
	ui.setupUi(this);
	tableItemTemplate.setTextAlignment(Qt::AlignCenter);//���ñ������Ĭ�Ͼ���
	//�����ֶ�����ʳ�Ĺ���ҳ�������ƿ�����ָı��źźͲۺ���
	QObject::connect(ui.nameBox_DetailPage, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_DetailPage_Changed()));
	//�����ֶ�����ʳ�����/ɾ��ҳ�������������ָı��źźͲۺ���
	QObject::connect(ui.nameBox_IngredientPage, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_IngredientPage_Changed()));
	QObject::connect(ui.amountBox, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_IngredientPage_Changed()));
	QObject::connect(ui.unitBox, SIGNAL(textChanged(QString)), this, SLOT(on_boxContent_IngredientPage_Changed()));
}


void RecipeManager::setupManagePage() {
	int i, j;
	auto table = ui.recipeTable;
	table->setSortingEnabled(false);//��ʱ�ر�����
	if (!isManagePageInited) {//���׹���ҳ��δ��ʼ����
		ui.searchBox_ManagePage->setText("");//����������
		ui.addButton_ManagePage->setHidden(ReadOnly);//ֻ��ģʽ��������Ӱ�ť������ģʽ����ʾ
		ui.deleteButton_ManagePage->setHidden(ReadOnly);//ֻ��ģʽ������ɾ����ť������ģʽ����ʾ
		table->hideColumn(1);//����ID��
		table->setColumnWidth(0, 600);//���������п��
		table->setColumnWidth(2, 300);//����ʱ���п��
		auto content = Utils::SQLQuery("select * from [Recipe]");//��ȡ���ױ��е���������
		const auto contentSize = content.size();//�������
		table->setRowCount(contentSize);//���ñ������
		if (contentSize) //��������0��
			for (i = 0; i < contentSize; ++i) {
				content[i].append(Utils::getTimeFromID(content[i][1]));//��ͨ��ID�õ���ʱ�䣨�����һ�У����������ױ��е�ĩβ
				for (j = 0; j < 3; ++j) {
					tableItemTemplate.setText(content[i][j]);//ģ����������
					table->setItem(i, j, tableItemTemplate.clone());//�������������ͼ
				}
			}
		table->update();//ˢ����ͼ
		isManagePageInited = true;//���ó�ʼ����ʶ
	}
	table->setSortingEnabled(true);//���¿�������
	auto header = table->horizontalHeader();
	header->setSortIndicator(2, Qt::AscendingOrder);//�����������
	header->setSortIndicatorShown(true);//��ʾ�����ͷ
	table->setCurrentCell(0, 0);//���õ�ǰѡ�е���
	ui.deleteButton_ManagePage->setEnabled(table->rowCount());//����ģʽ�²��ױ��������ݲ�����ɾ����ť����֮�����
	ui.detailButton->setEnabled(table->rowCount());//���ױ��������ݲ�������ϸ��Ϣ��ť����֮�����
}

void RecipeManager::setupDetailPage() {
	int i, j;
	auto table = ui.ingredientTable;
	table->setSortingEnabled(false);//��ʱ�ر�����
	if (!isDetailPageInited) {//ʳ�Ĺ���ҳ��δ��ʼ����
		ui.searchBox_DetailPage->setText("");//����������
		ui.footer_DetailPage->setHidden(AddMode_Recipe);//��Ӳ���ģʽ�����صײ��ؼ�������ģʽ����ʾ
		table->setHidden(AddMode_Recipe);//��Ӳ���ģʽ������ʳ�ı�����ģʽ����ʾ
		table->hideColumn(0);//����ID��
		ui.header2Right_DetailPage->setHidden(ReadOnly || AddMode_Recipe);//ֻ��ģʽ����Ӳ���ģʽ������ʳ�Ĳ�����ť������ģʽ����ʾ
		ui.header2Left_DetailPage->setFixedWidth(ReadOnly ? 940 : 590);//ֻ��ģʽ�½��ڶ��пؼ����������Ҳ�������ģʽ�¸�ԭ
		ui.header2Left_DetailPage->setHidden(AddMode_Recipe);//��Ӳ���ģʽ�����������ؼ���ֻ��ģʽ�����ģʽ����ʾ
		ui.header1Right_DetailPage->setHidden(ReadOnly);//ֻ��ģʽ�����ر��水ť������ģʽ����ʾ
		ui.header1Left_DetailPage->setFixedWidth(ReadOnly ? 940 : 710);//ֻ��ģʽ�½���һ�пؼ����������Ҳ�������ģʽ�¸�ԭ
		ui.nameBox_DetailPage->setDisabled(ReadOnly);//ֻ��ģʽ�Ž�ֹ���ƿյ����룬��֮������
		if (!AddMode_Recipe) {//����ʳ��
			auto recipeTable = ui.recipeTable;
			ui.nameBox_DetailPage->setText(recipeTable->item(recipeTable->currentRow(), 0)->text());//���ƿ���������ױ���ѡ���еĲ�������
			needToModify_Manage = false;//������Ҫ�޸ı�ʶ
			ui.timeBox->setText(recipeTable->item(recipeTable->currentRow(), 2)->text());//ʱ�����������ױ���ѡ���еĲ���ʱ��
			auto sql = QString("select * from [Ingredient] where [RecipeID] = '%1'").arg(recipeTable->item(recipeTable->currentRow(), 1)->text());
			auto content = Utils::SQLQuery(sql);//��ȡʳ�ı���IDΪѡ���е���������
			const auto contentSize = content.size();//�������
			table->setRowCount(contentSize);//���ñ������
			if (contentSize) //��������0��
				for (i = 0; i < contentSize; ++i)
					for (j = 0; j < 4; ++j) {
						tableItemTemplate.setText(content[i][j]);//ģ����������
						table->setItem(i, j, tableItemTemplate.clone());//�������ͼ
					}
			table->update();//ˢ����ͼ
		}
		isDetailPageInited = true;//���ó�ʼ����ʶ
	}
	table->setSortingEnabled(true);//���¿�������
	auto header = table->horizontalHeader();
	header->setSortIndicator(1, Qt::AscendingOrder);//�����������
	header->setSortIndicatorShown(true);//��ʾ�����ͷ
	table->setCurrentCell(0, 0);//���õ�ǰѡ�е���
	ui.deleteButton_DetailPage->setEnabled(table->rowCount());//����ģʽ��ʳ�ı��������ݲ�����ɾ����ť����֮�����
	ui.ingredientButton->setEnabled(table->rowCount());//����������ݲ�������ϸ��Ϣ��ť����֮�����
}

void RecipeManager::setupIngredientPage() {
	auto table = ui.ingredientTable;
	ui.nameBox_IngredientPage->setText(AddMode_Ingredient ? "" : table->item(table->currentRow(), 1)->text());//����ģʽ���ó�ʼֵ����ͬ
	ui.amountComboBox->setCurrentIndex(AddMode_Ingredient ? 0 : table->item(table->currentRow(), 2)->text() == "����");
	needToModify_Detail = false;//������Ҫ�޸ı�ʶ
}

void RecipeManager::setupPrepareMainPage() {
	auto table = ui.prepareTable;
	table->setSortingEnabled(false);//��ʱ�ر�����
	if (!isPrepareMainPageInited) {//������ҳ��δ��ʼ����
		ui.searchBox_PrepareMainPage->setText("");//����������
		Utils::SQLExcecute_Multiple({
			QLatin1String("DROP TABLE IF EXISTS Prepare"),//���������������ɾ��
			QLatin1String("CREATE TABLE Prepare(ID char(14) NOT NULL PRIMARY KEY REFERENCES Recipe (ID) ON DELETE CASCADE ON UPDATE CASCADE, prepareNum integer not null) WITHOUT ROWID"),//���´���������
			QLatin1String("INSERT INTO Prepare SELECT ID,0 FROM Recipe")//��������
			});
		table->hideColumn(1);//����ID��
		table->setColumnWidth(0, 500);//���������п��
		table->setColumnWidth(2, 250);//����ʱ���п��
		table->setColumnWidth(3, 150);//���������п��
		isPrepareMainPageInited = true;//���ó�ʼ����ʶ
	}
	else if (needToModify_Prepare) {//��Ҫ�޸�
		int i, j;
		ui.searchBox_PrepareMainPage->setText("");//����������
		const auto sql = QLatin1String("select [name], [ID], [prepareNum] from [Recipe] join [Prepare] using (ID) where [prepareNum] != 0");
		auto content = Utils::SQLQuery(sql);//��ȡ���˱����������
		const auto contentSize = content.size();//�������
		table->setRowCount(contentSize);//���ñ������
		if (contentSize) {//��������0��
			for (i = 0; i < contentSize; ++i) {
				content[i].insert(2, Utils::getTimeFromID(content[i][1]));//��ͨ��ID�õ���ʱ�䣨�����һ�У�������ID�к�
				for (j = 0; j < 4; ++j) {
					tableItemTemplate.setText(content[i][j]);//ģ����������
					table->setItem(i, j, tableItemTemplate.clone());//�������ͼ
				}
			}
			table->setCurrentCell(0, 0);//����ѡ����
		}
		needToModify_Prepare = false;//������Ҫ�޸ı�ʶ
	}
	table->update();//ˢ����ͼ
	table->setSortingEnabled(true);//���¿�������
	auto header = table->horizontalHeader();
	header->setSortIndicator(2, Qt::AscendingOrder);//�����������
	header->setSortIndicatorShown(true);//��ʾ�����ͷ
	ui.deleteButton_PrepareMainPage->setEnabled(table->rowCount());//���������ݲ�����ɾ����ť����֮�����
	ui.prepareDetailButton->setEnabled(table->rowCount());//����������ݲ����ñ༭��ť����֮�����
	ui.prepareResultButton->setEnabled(table->rowCount());//����������ݲ�����ʳ���������㰴ť����֮�����
}

void RecipeManager::setupPrepareDetailPage() {
	int i, j;
	auto table = ui.prepareDetailTable;
	auto sql = QLatin1String("select [name], [ID], [prepareNum] from [Recipe] join [Prepare] using (ID) where [prepareNum] = 0");
	QList<QStringList> content;
	table->setSortingEnabled(false);//��ʱ�ر�����
	if (!isPrepareDetailPageInited) {//��������ҳδ��ʼ����
		ui.searchBox_PrepareDetailPage->setText("");//����������
		table->hideColumn(1);//����ID��
		table->setColumnWidth(0, 500);//���������п��
		table->setColumnWidth(2, 250);//����ʱ���п��
		table->setColumnWidth(3, 150);//���������п��
		content = Utils::SQLQuery(sql);//��ȡ���˱����������
		isPrepareDetailPageInited = true;//���ó�ʼ����ʶ
	}
	const auto contentSize = content.size();//�������
	table->setRowCount(contentSize);//���ñ������
	if (contentSize) {//��������0��
		for (i = 0; i < contentSize; ++i) {
			content[i].insert(2, Utils::getTimeFromID(content[i][1]));//��ͨ��ID�õ���ʱ�䣨�����һ�У�������ID�к�
			for (j = 0; j < 4; ++j) {
				tableItemTemplate.setText(content[i][j]);//ģ����������
				table->setItem(i, j, tableItemTemplate.clone());//�������ͼ
			}
		}
		table->setCurrentCell(0, 0);//���õ�ǰѡ�е���
	}
	if (!AddMode_Prepare) {//������
		table->setRowCount(contentSize + 1);//���ñ������
		auto prepareTable = ui.prepareTable;
		const auto curRow = prepareTable->currentRow();
		for (i = 0; i < 4; ++i)
			table->setItem(contentSize, i, prepareTable->item(curRow, i)->clone());//�������ͼ
		table->setCurrentCell(contentSize, 0);//���赱ǰѡ�е���
	}
	table->setColumnHidden(3, AddMode_Prepare);//���ģʽ������������
	table->update();//ˢ����ͼ
	table->setSortingEnabled(true);//���¿�������
	auto header = table->horizontalHeader();
	header->setSortIndicator(2, Qt::AscendingOrder);//�����������
	header->setSortIndicatorShown(true);//��ʾ�����ͷ
	ui.amountSpinBox->setValue(table->rowCount() ? table->item(table->currentRow(), 3)->text().toInt() : 0);//��������
	ui.okButton_PrepareDetailPage->setEnabled(table->rowCount());//����������ݲ�����ȷ�����/�޸İ�ť����֮�����
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
	ReadOnly = true;//����Ϊֻ��ģʽ
	ui.pages->setCurrentIndex(MANAGE_PAGE_INDEX);//��ת�����׹������
}
void RecipeManager::on_manageButton_Released() {
	ReadOnly = false;//����Ϊ���޸�ģʽ
	ui.pages->setCurrentIndex(MANAGE_PAGE_INDEX);//��ת�����׹������
}
void RecipeManager::on_prepareButton_Released() {
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//��ת������������
}


void RecipeManager::on_backButton_ManagePage_Released() {
	isManagePageInited = false;//���ó�ʼ����ʶ
	ui.recipeTable->setRowCount(0);
	ui.pages->setCurrentIndex(MAIN_PAGE_INDEX);//�ص�������
}
void RecipeManager::on_searchButton_ManagePage_Released() {
	//����
}
void RecipeManager::on_addButton_ManagePage_Released() {
	AddMode_Recipe = true;//����Ϊ���ģʽ
	ui.pages->setCurrentIndex(DETAIL_PAGE_INDEX);//��ת��ʳ�Ĺ������
}
void RecipeManager::on_deleteButton_ManagePage_Released() {
	const auto msg = "<font size='9'>ȷ��Ҫɾ���ò�����</font>";
	if (!QMessageBox::warning(nullptr, "ɾ��ȷ��", msg, "ȷ��", "ȡ��", "", 1)) {//ɾ������
		auto table = ui.recipeTable;
		const auto i = table->currentRow();
		const auto sql = QString("delete from [Recipe] where [ID] = '%1'").arg(table->item(i, 1)->text());
		Utils::SQLExcecute(sql);
		isManagePageInited = false;//���ó�ʼ����ʶ
		setupManagePage();
		table->setCurrentCell(i - (i > 0), 0);
	}
}
void RecipeManager::on_detailButton_Released() {
	AddMode_Recipe = false;//����Ϊ����ģʽ
	ui.pages->setCurrentIndex(DETAIL_PAGE_INDEX);//��ת��ʳ�Ĺ������
}


void RecipeManager::on_backButton_DetailPage_Released() {
	const auto msg = "<font size='9'>ȷ���������и��Ĳ��˳���</font>";
	if (ReadOnly || !needToModify_Manage || QMessageBox::question(nullptr, "�˳�ȷ��", msg, "ȡ��", "ȷ��", "", 0)) {
		ui.nameBox_DetailPage->setText("");//������ƿ�
		ui.ingredientTable->setRowCount(0);//���ñ��״̬
		isDetailPageInited = false;//���ó�ʼ����ʶ
		ui.pages->setCurrentIndex(MANAGE_PAGE_INDEX);//�ص����׹������
	}
}
void RecipeManager::on_searchButton_DetailPage_Released() {
	//����
}
void RecipeManager::on_addButton_DetailPage_Released() {
	AddMode_Ingredient = true;//����Ϊ���ģʽ
	ui.pages->setCurrentIndex(INGREDIENT_PAGE_INDEX);//��ת��ʳ�����/�༭����
}
void RecipeManager::on_deleteButton_DetailPage_Released() {
	const auto msg = "<font size='9'>ȷ��Ҫɾ����ʳ����</font>";
	if (!QMessageBox::warning(nullptr, "ɾ��ȷ��", msg, "ȷ��", "ȡ��", "", 1)) {//ɾ��ʳ��
		auto table = ui.ingredientTable;
		const auto i = table->currentRow();
		const auto RecipeID = table->item(i, 0)->text();
		const auto name = Utils::prepareDataForSQL(table->item(i, 1)->text());
		const auto unit = Utils::prepareDataForSQL(table->item(i, 3)->text());
		const auto sql = QString("delete from [Ingredient] where (RecipeID, name, unit) = ('%1', '%2', '%3')");
		Utils::SQLExcecute(sql.arg(RecipeID, name, unit));
		isDetailPageInited = false;//���ó�ʼ����ʶ
		setupDetailPage();
		table->setCurrentCell(i - (i > 0), 0);
	}
}
void RecipeManager::on_saveButton_DetailPage_Released() {
	const auto name = ui.nameBox_DetailPage->text();
	if (Utils::isContentInvalid(name))
		return (void)QMessageBox::critical(nullptr, "���ݴ���", "<font size='9'>���Ʋ���Ϊ���Ҳ��ܺ������·Ƿ��ַ�<br/>`</font>", "ȷ��");
	auto table = ui.recipeTable;
	if (AddMode_Recipe) {//��Ӳ���ģʽ��
		const auto ID = Utils::createID();
		const auto i = table->rowCount();
		Utils::SQLInsert(QLatin1String("Recipe"), { QLatin1String("ID"), QLatin1String("name") }, { ID, name });
		isManagePageInited = false;//���ó�ʼ����ʶ
		setupManagePage();
		table->setCurrentCell(i, 2);
		AddMode_Recipe = false;//������Ҫ���ӱ�ʶ
		ui.header2Left_DetailPage->show();//�ָ������ؼ���ʾ
		ui.header2Right_DetailPage->show();//�ָ�ʳ�Ĳ�����ť��ʾ
		ui.ingredientTable->show();//�ָ�ʳ�ı���ʾ
		ui.timeBox->setText(table->currentItem()->text());//����ʱ��������
		ui.footer_DetailPage->show();//�ָ��ײ��ؼ���ʾ
	}
	else {//�������ģʽ
		const auto ID = table->item(table->currentRow(), 1)->text();
		Utils::SQLExcecute(QString("update [Recipe] set [name] = '%1' where [ID] = '%2'").arg(Utils::prepareDataForSQL(name), ID));
		table->item(table->currentRow(), 0)->setText(name);//�ڱ���ͼ���޸Ķ�Ӧ�е�����
	}
	table->update();//ˢ����ͼ
	needToModify_Manage = false;//������Ҫ�޸ı�ʶ
}
void RecipeManager::on_ingredientButton_Released() {
	AddMode_Ingredient = false;//����Ϊ����ģʽ
	ui.pages->setCurrentIndex(INGREDIENT_PAGE_INDEX);//��ת��ʳ�����/�༭����
}
void RecipeManager::on_boxContent_DetailPage_Changed() {
	needToModify_Manage = true;//������Ҫ�޸ı�ʶ
}


void RecipeManager::exitIngredient() {//��ʳ�����/�޸�ҳ�淵��
	needToModify_Detail = false;//������Ҫ�޸ı�ʶ
	ui.amountComboBox->setCurrentIndex(-1);//���ø�ѡ��
	ui.pages->setCurrentIndex(DETAIL_PAGE_INDEX);//�ص�ʳ�Ĺ������
}

void RecipeManager::on_backButton_IngredientPage_Released() {
	if (needToModify_Detail) {//��Ҫ�޸�
		const auto msg = "<font size='9'>ȷ���������и��Ĳ��˳���</font>";
		if (QMessageBox::question(nullptr, "�˳�ȷ��", msg, "ȡ��", "ȷ��", "", 0))
			exitIngredient();//��ʳ�����/�޸�ҳ�淵��
	}
	else
		exitIngredient();//��ʳ�����/�޸�ҳ�淵��
}
void RecipeManager::on_amountComboBox_Changed(int suitableAmount) {
	if (~suitableAmount) {//������(!=-1)
		auto table = ui.ingredientTable;
		if (suitableAmount) {//ѡ��"����"
			ui.amountBox->setText("����");
			ui.unitBox->setText("-");
		}
		else if (AddMode_Ingredient) {//ѡ��"����"��Ϊ���ʳ��ģʽ
			ui.amountBox->setText("");
			ui.unitBox->setText("");
		}
		else {//ѡ��"����"��Ϊ�༭ʳ��ģʽ
			const auto historyAmount = table->item(table->currentRow(), 2)->text();
			if (historyAmount == "����") {//ԭʼ��¼Ϊ����
				ui.amountBox->setText("");
				ui.unitBox->setText("");
			}
			else {//ԭʼ��¼Ϊ����
				ui.amountBox->setText(historyAmount);
				ui.unitBox->setText(table->item(table->currentRow(), 3)->text());
			}
		}
		ui.amountBox->setDisabled(suitableAmount);//ѡ������ʱ���ñ༭������ʱ���ã���ͬ
		ui.unitBox->setDisabled(suitableAmount);
	}
}
void RecipeManager::on_okButton_IngredientPage_Released() {
	if (!needToModify_Detail)
		return exitIngredient();//����Ҫ�޸���ֱ���˳�
	const auto name = ui.nameBox_IngredientPage->text();
	if (Utils::isContentInvalid(name))
		return (void)QMessageBox::critical(nullptr, "���ݴ���", "<font size='9'>���Ʋ���Ϊ���Ҳ��ܺ������·Ƿ��ַ�<br/>`</font>", "ȷ��");
	auto amount = ui.amountBox->text();
	if (Utils::isContentInvalid(amount))
		return (void)QMessageBox::critical(nullptr, "���ݴ���", "<font size='9'>��������Ϊ���Ҳ��ܺ������·Ƿ��ַ�<br/>`</font>", "ȷ��");
	const auto unitBox = ui.unitBox->text();
	QString unit;
	if (amount == "����")
		unit = QLatin1String("-");
	else {
		unit = unitBox;
		if (Utils::isContentInvalid(unitBox))
			return (void)QMessageBox::critical(nullptr, "���ݴ���", "<font size='9'>��λ����Ϊ���Ҳ��ܺ������·Ƿ��ַ�<br/>`</font>", "ȷ��");
		const auto amount_double = amount.toDouble() + 0.000055;
		const auto amount_int = (double)((long long)(amount_double + 0.5));
		const auto amount_round = Utils::round(amount_double, 3);//����:С�������λ
		if (amount_int < 1)
			if (amount_round < 0.0005)
				return (void)QMessageBox::critical(nullptr, "��ֵ����", "<font size='9'>������Ϸ��Ҵ���0�İ���������(֧��С��)��</font>", "ȷ��");
		const auto DELTA = (amount_double - amount_int) * (amount_double - amount_int);
		const auto format = (DELTA < 0.00000025) ? "%.0f" : "%.3f";//ȡ����ֵ:0.0005
		amount.sprintf(format, amount_round);
	}
	auto table = ui.ingredientTable;
	const auto recipeTable = ui.recipeTable;
	const auto RecipeID = recipeTable->item(recipeTable->currentRow(), 1)->text();
	if (table->rowCount()) {//���״��ڲ�ֹһ��ʳ������
		const auto nameSQL = Utils::prepareDataForSQL(name);
		const auto unitSQL = Utils::prepareDataForSQL(unitBox);
		QString oldName, oldUnit;
		if (!AddMode_Ingredient) {
			oldName = Utils::prepareDataForSQL(table->item(table->currentRow(), 1)->text());
			oldUnit = Utils::prepareDataForSQL(table->item(table->currentRow(), 3)->text());
		}
		auto sql = QString("select * from [Ingredient] where (RecipeID, name, unit) = ('%1', '%2', '%3')").arg(RecipeID, nameSQL, unitSQL);
		auto result = Utils::SQLQuery(sql);
		if (!result.isEmpty() && (AddMode_Ingredient || oldName != nameSQL || oldUnit != unitSQL)) //���������ݳ�ͻ
			return (void)QMessageBox::critical(nullptr, "�Ѿ�����", "<font size='9'>�Ѿ�����ʳ��������������ƺ͵�λ��ͬ��</font>", "ȷ��");
		if (AddMode_Ingredient) //���ʳ�ģ�
			AddMode_Ingredient = false;//������Ҫ���ӱ�ʶ
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
	isDetailPageInited = false;//���ó�ʼ����ʶ
	setupDetailPage();
	table->update();
	table->setCurrentCell(i, 0);
	exitIngredient();//��ʳ�����/�޸�ҳ�淵��
}
void RecipeManager::on_cancelButton_IngredientPage_Released() {
	on_backButton_IngredientPage_Released();
}
void RecipeManager::on_boxContent_IngredientPage_Changed() {
	needToModify_Detail = true;//������Ҫ�޸ı�ʶ
}


void RecipeManager::on_backButton_PrepareMainPage_Released() {
	const auto msg = "<font size='9'>ȷ��������ʳ���������˳���</font>";
	if (!(needToCalc && QMessageBox::question(nullptr, "�˳�ȷ��", msg, "ȷ��", "ȡ��", "", 1))) {
		isPrepareMainPageInited = false;//���ó�ʼ��״̬
		needToCalc = false;//����ʳ��������Ҫ����״̬
		ui.prepareTable->setRowCount(0);//��ձ��˱�
		ui.prepareResultTable->setRowCount(0);//��ձ��˽����
		Utils::SQLExcecute("DROP TABLE IF EXISTS Prepare");//ɾ�����˱�
		ui.pages->setCurrentIndex(MAIN_PAGE_INDEX);//�ص�������
	}
}
void RecipeManager::on_searchButton_PrepareMainPage_Released() {
	//����
}
void RecipeManager::on_addButton_PrepareMainPage_Released() {
	AddMode_Prepare = true;//����Ϊ���ģʽ
	ui.pages->setCurrentIndex(PREPARE_DETAIL_PAGE_INDEX);//��ת�����˹������
}
void RecipeManager::on_deleteButton_PrepareMainPage_Released() {
	const auto msg = "<font size='9'>ȷ����׼���ò�����</font>";
	if (!QMessageBox::warning(nullptr, "ɾ��ȷ��", msg, "ȷ��", "ȡ��", "", 1)) {//ɾ������
		auto table = ui.prepareTable;
		const auto i = table->currentRow();
		const auto sql = QString("update [Prepare] set [prepareNum] = 0 where [ID] = '%1'").arg(table->item(i, 1)->text());
		Utils::SQLExcecute(sql);
		needToModify_Prepare = true;//�����޸ı�ʶ
		setupPrepareMainPage();//ˢ�±��
		table->setCurrentCell(i - (i > 0), 0);//��������ѡ����
	}
}
void RecipeManager::on_prepareDetailButton_Released() {
	AddMode_Prepare = false;//����Ϊ����ģʽ
	ui.pages->setCurrentIndex(PREPARE_DETAIL_PAGE_INDEX);//��ת�����˹������
}
void RecipeManager::on_prepareResultButton_Released() {
	calcPrepare();//����ʳ������
	ui.pages->setCurrentIndex(PREPARE_RESULT_PAGE_INDEX);//��ת�����˽������
}


void RecipeManager::exitPrepareDetail() {
	needToModify_Prepare = false;//������Ҫ�޸ı�ʶ
	isPrepareDetailPageInited = false;//���ó�ʼ����ʶ
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//��ת������������
}
void RecipeManager::on_backButton_PrepareDetailPage_Released() {
	const auto amount = ui.amountSpinBox->value();
	if (AddMode_Prepare && !amount) //���ģʽ������Ϊ0
		return exitPrepareDetail();//�ӱ�������ҳ�淵��
	if (!AddMode_Prepare) {//�༭ģʽ
		const auto amountStr = QString::number(amount);
		auto table = ui.prepareDetailTable;
		const auto ID = table->item(table->currentRow(), 1)->text();
		auto mainTable = ui.prepareTable;
		auto originalID = mainTable->item(mainTable->currentRow(), 1)->text();
		if (originalID == ID) {//ѡ�񱸲˵�ID����
			auto originalAmount = mainTable->item(mainTable->currentRow(), 3)->text();
			if (originalAmount == amountStr)
				return exitPrepareDetail();//�ӱ�������ҳ�淵��
		}
		if (!amount)
			return exitPrepareDetail();//�ӱ�������ҳ�淵��
	}
	const auto msg = "<font size='9'>ȷ���������и��Ĳ��˳���</font>";
	if (QMessageBox::question(nullptr, "�˳�ȷ��", msg, "ȡ��", "ȷ��", "", 0))
		exitPrepareDetail();//�ӱ�������ҳ�淵��
}
void RecipeManager::on_searchButton_PrepareDetailPage_Released() {
	//����
}
void RecipeManager::on_okButton_PrepareDetailPage_Released() {
	const auto amount = ui.amountSpinBox->value();
	if (!AddMode_Prepare || amount) {//�༭ģʽ/���ģʽ��׼��������Ϊ0
		const auto amountStr = QString::number(amount);
		auto table = ui.prepareDetailTable;
		const auto ID = table->item(table->currentRow(), 1)->text();
		if (AddMode_Prepare) {//���ģʽ��׼��������Ϊ0
			const auto sql = QString("update [Prepare] set [prepareNum] = %1 where [ID] = '%2'").arg(amountStr, ID);
			Utils::SQLExcecute(sql);
			needToModify_Prepare = true;//������Ҫ�޸ı�ʶ
		}
		else {//�༭ģʽ
			auto mainTable = ui.prepareTable;
			auto originalID = mainTable->item(mainTable->currentRow(), 1)->text();
			if (originalID != ID) {//ѡ�񱸲˵�ID�����˸���
				if (amount) {//������Ϊ0
					const auto msg = "<font size='9'>���ƺ���������׼���Ĳˣ�ȷ����׼��ԭ���Ĳ���</font>";
					if (QMessageBox::warning(nullptr, "���ı���ȷ��", msg, "ȷ��", "ȡ��", "", 1))
						return;//ȡ������
					Utils::SQLExcecute_Multiple({
						QString("update [Prepare] set [prepareNum] = 0 where [ID] = '%1'").arg(originalID),
						QString("update [Prepare] set [prepareNum] = %1 where [ID] = '%2'").arg(amountStr, ID)
						});
					needToModify_Prepare = true;//������Ҫ�޸ı�ʶ
				}
			}
			else if (amount) {//�޸ĵ�������Ϊ0
				const auto sql = QString("update [Prepare] set [prepareNum] = %1 where [ID] = '%2'").arg(amountStr, ID);
				Utils::SQLExcecute(sql);
				auto mainTable = ui.prepareTable;
				mainTable->item(mainTable->currentRow(), 3)->setText(amountStr);//�޸ı��������Ӧ�����ʾֵ
			}
			else //�޸�����Ϊ0,��ɾ��������ͬ
				on_deleteButton_PrepareMainPage_Released();
		}
	}
	isPrepareDetailPageInited = false;//���ó�ʼ����ʶ
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//�ص�����������
}
void RecipeManager::on_cancelButton_PrepareDetailPage_Released() {
	on_backButton_PrepareDetailPage_Released();
}

void RecipeManager::calcPrepare() {//����ʳ������
	const auto sql1_1 = QString("with I as (select * from [Ingredient] where [amount] != \"����\"), ");
	const auto sql1_2 = QLatin1String("P as (select [ID] as [RecipeID], [prepareNum] from [Prepare] where [prepareNum] > 0) ");
	const auto sql1_3 = QLatin1String("select [name], [amount], [prepareNum], [unit] from [I] join [P] using (RecipeID) order by [name], [unit]");
	const auto sql1 = sql1_1 % (sql1_2 % sql1_3);
	const auto sql2_1 = QString("with I as (select [RecipeID] as [ID], [name] from [Ingredient] where [amount] = \"����\"), ");
	const auto sql2_2 = QLatin1String("P as (select * from [Prepare] where [prepareNum] > 0) ");
	const auto sql2_3 = QLatin1String("select distinct [name] from [I] join [P] using (ID) order by [name]");
	const auto sql2 = sql2_1 % (sql2_2 % sql2_3);
	auto preciseOriginal = Utils::SQLQuery(sql1);//����ԭ�б�
	auto moderate = Utils::SQLQuery(sql2);//�����б�
	//�����б����ֵ�������±�
	auto precise = QList<QStringList>();
	int i, j;
	if (preciseOriginal.size()) {//����ԭ��Ϊ���б�
		auto itOriginalOuter = preciseOriginal.constBegin();
		auto itOriginalInner = itOriginalOuter->constBegin();
		precise.append({//�����ʼԪ��
			*itOriginalInner,
			Utils::amountCalc(*(++itOriginalInner), *(++itOriginalInner)),
			*(++itOriginalInner)
			});
		i = 0;//�����б�����±�
		while (++itOriginalOuter != preciseOriginal.constEnd()) {
			itOriginalInner = itOriginalOuter->constBegin();
			if ((precise[i][0] == *itOriginalInner) && (precise[i][2] == itOriginalOuter->constLast())) {
				precise[i][1] = Utils::amountCalc(precise[i][1], *(++itOriginalInner), *(++itOriginalInner));
				continue;//ʳ�����ƺ͵�λ��ͬ���±����һ�������ۼ�
			}
			precise.append({//ʳ�����ƻ�λ��ͬ���±��������
				*itOriginalInner,
				Utils::amountCalc(*(++itOriginalInner), *(++itOriginalInner)),
				*(++itOriginalInner)
				});
			++i;//ͬʱ����±�����1
		}
	}
	//�������������
	const auto preciseSize = precise.size();
	const auto moderateSize = moderate.size();
	auto table = ui.prepareResultTable;
	table->setSortingEnabled(false);//��ʱ�ر�����
	const auto totalSize = preciseSize + moderateSize;//����������
	if (totalSize) {//������������Ϊ0��
		table->setRowCount(totalSize);//���ý���������
		i = -1;
		if (preciseSize) //������������Ϊ0��
			while (++i < preciseSize)
				for (j = 0; j < 3; ++j) {
					tableItemTemplate.setText(precise[i][j]);//ģ����������
					table->setItem(i, j, tableItemTemplate.clone());//�������ͼ
				}
		if (moderateSize) //������������Ϊ0��
			for (auto& name : moderate) {
				tableItemTemplate.setText(name.constFirst());//ģ���������ݣ���ͬ
				table->setItem(i, 0, tableItemTemplate.clone());//�������ͼ����ͬ
				tableItemTemplate.setText("����");
				table->setItem(i, 1, tableItemTemplate.clone());
				tableItemTemplate.setText(QLatin1String("-"));
				table->setItem(i, 2, tableItemTemplate.clone());
				++i;
			}
		table->setCurrentCell(0, 0);//����ѡ����
	}
	table->update();//ˢ����ͼ
	table->setSortingEnabled(true);//���¿�������
	auto header = table->horizontalHeader();
	header->setSortIndicator(0, Qt::AscendingOrder);//�����������
	header->setSortIndicatorShown(true);//��ʾ�����ͷ
}
void RecipeManager::on_backButton_PrepareResultPage_Released() {
	ui.pages->setCurrentIndex(PREPARE_MAIN_PAGE_INDEX);//�ص�����������
}