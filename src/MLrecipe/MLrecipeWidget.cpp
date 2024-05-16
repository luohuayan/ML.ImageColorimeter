#include "MLrecipeWidget.h"
#include <QtWidgets/qfiledialog.h>
MLrecipeWidget::MLrecipeWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

MLrecipeWidget::~MLrecipeWidget()
{
}
void MLrecipeWidget::on_loadRcipe_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Select Recipe", "", "(*.json *.txt *.recipe)");
	if (!fileName.isEmpty())
	{
		//TODO
		ui.recipePath->setText(fileName);
	}
}