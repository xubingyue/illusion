#include "stdafx.h"
#include "IllusionWidget.h"

IllusionWidget::IllusionWidget(QWidget *parent)
	: QSplitter(parent)
{
	proto_2_ils_map_ = Biko_Read_Config::instance()->get_proto_illusion_map();
	setup_ui();
}

IllusionWidget::~IllusionWidget()
{

}


void IllusionWidget::setup_ui()
{
	//注意Qt的或者扶对象，布局对象，的销毁会回收子对象，
	this->setContentsMargins(5, 5, 5, 5);
	this->setOrientation(Qt::Horizontal);

	//配置的棋盘滚动区域

	proto_dir_tree_ = new QTreeWidget();
	proto_dir_tree_->setGeometry(QRect(0, 0, 1000, 600));

	this->addWidget(proto_dir_tree_);

	show_illusion_tab_ = new QTabWidget();
	show_illusion_tab_->setTabPosition(QTabWidget::North);
	this->addWidget(show_illusion_tab_);

	this->setStretchFactor(0, 4);
	this->setStretchFactor(1, 6);

	proto_dir_tree_->setColumnCount(2);
	QStringList headers;
	headers << QString::fromLocal8Bit("项目字段")
		<< QString::fromLocal8Bit("数值");
	proto_dir_tree_->setHeaderLabels(headers);
	proto_dir_tree_->setColumnWidth(0, 259);
	proto_dir_tree_->setColumnWidth(1, 200);

	connect(proto_dir_tree_,
			&QTreeWidget::itemDoubleClicked,
			this,
			&IllusionWidget::item_double_clicked);

	show_illusion_tab_->setTabsClosable(true);
	//!
	show_readme_ = new QTextEdit(show_illusion_tab_);
	show_illusion_tab_->addTab(show_readme_,
							   QString::fromLocal8Bit("README文件"));
	
	

}

void IllusionWidget::select_all()
{
	int root_count = proto_dir_tree_->topLevelItemCount();
	for (int i=0;i<root_count;++i)
	{
		QTreeWidgetItem *root = proto_dir_tree_->topLevelItem(i);
		QTreeWidgetItem *father = root->child(0);
		father->setCheckState(0, Qt::Checked);
	}
}

void IllusionWidget::select_none()
{
	int root_count = proto_dir_tree_->topLevelItemCount();
	for (int i = 0; i < root_count; ++i)
	{
		QTreeWidgetItem *root = proto_dir_tree_->topLevelItem(i);
		QTreeWidgetItem *father = root->child(0);
		father->setCheckState(0, Qt::Unchecked);
	}
}

void IllusionWidget::selected_item(QStringList & selected_message)
{
	int root_count = proto_dir_tree_->topLevelItemCount();
	for (int i = 0; i < root_count; ++i)
	{
		QTreeWidgetItem *root = proto_dir_tree_->topLevelItem(i);
		QTreeWidgetItem *father = root->child(0);
		if (father->checkState(0) == Qt::Checked)
		{
			selected_message.append(father->text(1));
		}
	}
}


//!
void IllusionWidget::loead_illusion()
{
	if (proto_2_ils_map_->size() <= 0)
	{
		return;
	}
	proto_dir_tree_->clear();
	QStringList root_list,child_list;
	//!
	for (auto iter = proto_2_ils_map_->begin(); 
		 iter != proto_2_ils_map_->end();
		 ++iter)
	{
		root_list.clear();
		root_list.append(QString::fromLocal8Bit("Proto 文件："));
		root_list.append(iter->first);
		QTreeWidgetItem *root = new QTreeWidgetItem(root_list, ITEM_PROTO_FILE);
		proto_dir_tree_->addTopLevelItem(root);
		root->setExpanded(true);

		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			QTreeWidgetItem *father = NULL;
			child_list.clear();
			child_list.append(QString::fromLocal8Bit("Table Message 名称:"));
			child_list.append(iter->second[i]->table_message_name_);
			father = new QTreeWidgetItem(child_list, ITEM_PROTO_TABLE_MESSAGE);
			root->addChild(father);
			father->setExpanded(true);
			father->setFlags(father->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
			if (iter->second[i]->excelcfg_is_newer_)
			{
				father->setCheckState(0, Qt::Checked);
			}
			else
			{
				father->setCheckState(0, Qt::Unchecked);
			}
			

			QTreeWidgetItem *child = NULL;
			child_list.clear();
			child_list.append(QString::fromLocal8Bit("Line Message 名称:"));
			child_list.append(iter->second[i]->line_message_name_);
			child = new QTreeWidgetItem(child_list, ITEM_PROTO_LINE_MESSAGE);
			father->addChild(child);

			child_list.clear();
			child_list.append(QString::fromLocal8Bit("EXCEL文件名称:"));
			child_list.append(iter->second[i]->excel_file_name_);
			child = new QTreeWidgetItem(child_list, ITEM_EXCEL_FILE);
			father->addChild(child);

			child_list.clear();
			child_list.append(QString::fromLocal8Bit("EXCEL SHEET:"));
			child_list.append(iter->second[i]->excel_sheet_name_);
			child = new QTreeWidgetItem(child_list, ITEM_EXCEL_SHEET);
			father->addChild(child);

			child_list.clear();
			child_list.append(QString::fromLocal8Bit("输出文件名称:"));
			child_list.append(iter->second[i]->outer_file_name_);
			child = new QTreeWidgetItem(child_list, ITEM_OUTER_FILE);
			father->addChild(child);

			child_list.clear();
			child_list.append(QString::fromLocal8Bit("字段数量（包括展开RPEATED）:"));
			child_list.append(QString::number(iter->second[i]->column_field_count_));
			child = new QTreeWidgetItem(child_list);
			father->addChild(child);

			child_list.clear();
			child_list.append(QString::fromLocal8Bit("对应的EXCEL文件是否存在:"));
			if (iter->second[i]->exist_excel_file_)
			{
				child_list.append(QString::fromLocal8Bit("是"));
			}
			else
			{
				child_list.append(QString::fromLocal8Bit("否"));
			}
			child = new QTreeWidgetItem(child_list);
			father->addChild(child);

			child_list.clear();
			child_list.append(QString::fromLocal8Bit("对应的EXCEL文件是否更新:"));
			if (iter->second[i]->excelcfg_is_newer_)
			{
				child_list.append(QString::fromLocal8Bit("是"));
			}
			else
			{
				child_list.append(QString::fromLocal8Bit("否"));
			}
			child = new QTreeWidgetItem(child_list);
			father->addChild(child);
		}
	}
}


//!
void IllusionWidget::item_double_clicked(QTreeWidgetItem* item, int colum)
{
	if (NULL == item)
	{
		return;
	}
	
	TREE_ITEM_TYPE item_type = static_cast<TREE_ITEM_TYPE>( item->type());
	switch(item_type)
	{
	case ITEM_PROTO_FILE:
		break;
	case ITEM_PROTO_LINE_MESSAGE:
		break;
	case ITEM_EXCEL_SHEET:
		break;
	case ITEM_OUTER_FILE:
		break;
	default:
		break;
	}
}


