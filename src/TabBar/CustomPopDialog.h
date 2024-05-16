#pragma once

#include <QDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
/**
 * @brief A window containing tab pop-up pages
 *
 * Override mouse release event
 *
 * Override keyboard ESC key events
 */

class CustomPopDialog  : public QDialog
{
	Q_OBJECT

public:
	CustomPopDialog(QWidget *parent = nullptr);
	~CustomPopDialog();

public:
	void setContentWidget(QWidget* page);
	QWidget* getContentWidget();
	bool isDialogClose = false;
protected:
	bool event(QEvent* event) override;
	// Rewrite the keyboard event keyPressEvent, and press the esc key to close the window
	void keyPressEvent(QKeyEvent* event) override;

private slots:
	void mousePointByTimer();

signals:
	void dragRelease(const QPoint& globalPos);
	void PopDialogClose(QWidget* page);
	void mouseGlobalPoint(QPoint point, QDialog*);
private:
	QWidget* m_pWidget = nullptr;
	bool m_isPress = false;
	QTimer* m_timer;
};
