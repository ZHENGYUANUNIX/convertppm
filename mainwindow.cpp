#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    ui->buttonConvert->setDisabled(true);
    ui->buttonStopConvert->setDisabled(true);
    ui->buttonStopConvert->setDisabled(true);
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);

    connect(&thread, &ConvertThread::started, this, &MainWindow::on_thread_started);
    connect(&thread, &ConvertThread::finished, this, &MainWindow::on_thread_finished);
    connect(&thread, &ConvertThread::progress, ui->progressBar, &QProgressBar::setValue);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonLoadMap_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    QStringLiteral("加载地图文件"),
                                                    QCoreApplication::applicationDirPath(),
                                                    QStringLiteral("地图文件(*.smap)"));
    if (!fileName.isEmpty())
    {
        thread.setSMapPath(fileName);
        ui->labelPath->setText(QStringLiteral("地图文件:") + fileName);
        ui->buttonConvert->setDisabled(false);
    }
}


void MainWindow::on_buttonConvert_clicked()
{
    thread.start();
}

void MainWindow::on_thread_started()
{
    ui->buttonStopConvert->setDisabled(false);
    ui->buttonLoadMap->setDisabled(true);
    ui->buttonConvert->setDisabled(true);
    ui->buttonConvert->setText(QStringLiteral("转换中..."));
    ui->progressBar->setMaximum(0);
}

void MainWindow::on_thread_finished()
{
    ui->buttonConvert->setText(QStringLiteral("开始转换"));
    ui->buttonConvert->setDisabled(false);
    //QMessageBox::information(this, QStringLiteral("SMap转PPM"), QStringLiteral("数据转换成功, 文件保存在") + thread.getSMapPath().replace(QString(".smap"), QString(".PPM")), QStringLiteral("确定"));
    QMessageBox::information(this, QStringLiteral("SMap转PPM"), QStringLiteral("数据转换完成."), QStringLiteral("确定"));
    ui->buttonLoadMap->setDisabled(false);
    ui->buttonStopConvert->setDisabled(true);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);

}

void MainWindow::on_buttonStopConvert_clicked()
{
    thread.setStop();
    ui->progressBar->setMaximum(100);
}
