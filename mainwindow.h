#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PClasses.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();


    void on_textEdit_selectionChanged();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    Sat_Solver *solution;
};

#endif // MAINWINDOW_H
