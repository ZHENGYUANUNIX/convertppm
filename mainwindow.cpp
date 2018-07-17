#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    ui->buttonConvert->setDisabled(true);
    ui->buttonStopConvert->setDisabled(true);
    ui->progressBar->setValue(70);
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

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
        ui->labelPath->setText(QStringLiteral("地图文件:") + fileName);
        ui->buttonConvert->setDisabled(false);
    }
    ui->progressBar->setValue(0);
}


void MainWindow::on_buttonConvert_clicked()
{
    ui->buttonStopConvert->setDisabled(false);
    thread.start();
}

void MainWindow::on_thread_started()
{

}

void MainWindow::on_thread_finished()
{

}




const char *MainWindow::__strstr(const char *src, const char *needle)
{
    const char *p1 = nullptr, *p2 = nullptr;
    p1 = src;
    p2 = needle;
    while (src != needle && *needle != '\0')
    {
        if (*src++ != *needle++)
        {
            needle = p2;
            src = ++p1;
        }
    }
    if (*needle == '\0')
    {
        return p1;
    }

    return NULL;
}
