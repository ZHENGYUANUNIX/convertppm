#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "convertthread.h"
#include <QMainWindow>
#include <QObject>


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
    void on_buttonLoadMap_clicked();
    void on_buttonConvert_clicked();
    void on_buttonStopConvert_clicked();
    void on_thread_started();
    void on_thread_finished();

private:
    ConvertThread thread;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
