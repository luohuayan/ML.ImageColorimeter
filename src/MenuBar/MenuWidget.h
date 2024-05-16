#pragma once
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
class QMainWindow;

class MenuWidget : public QMenuBar {
    Q_OBJECT
public:
    MenuWidget(QWidget* parent = nullptr);
    ~MenuWidget();
signals:
    void sigTestModeChaged();
    void sigRightBarZoom();
    void sigFullScreen();
    void sigModeStyle(int);
    void sigTabModeStyle(int,bool);
 private slots:
    void updateTestMode(QString);
    void onUpdateTabMode(QWidget*);

 private:
    QLabel* mode;
    QAction* tab1Action;
    QAction* tab2Action;
    QAction* tab3Action;
    QAction* tab4Action;
};