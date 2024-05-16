#include "ROItableWidget.h"

ROItableWidget::ROItableWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
}

ROItableWidget::~ROItableWidget()
{}

QItemSelectionModel* ROItableWidget::getSelectionModel()
{
	return ui.tableWidget->selectionModel();
}

