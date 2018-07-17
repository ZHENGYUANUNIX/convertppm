#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QFile>
#include <iostream>
#include <stdlib.h>

#define CM_PRECISION_100 100
#define PIXEL_BLACK_ROBOT_SIZE 5

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    ui->buttonConvert->setDisabled(true);
    ui->buttonStopConvert->setDisabled(true);
    ui->progressBar->setStyleSheet("QProgressBar{border:none;background:#E5E5E5;border-radius:2px;}"
                                   "QProgressBar::chunk{background:#009DD6;border-radius:2px;}");
    ui->progressBar->setValue(70);
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

}

MainWindow::~MainWindow()
{
    delete ui;
    if (ptrPixelDate)
    {
        delete ptrPixelDate;
        ptrPixelDate = nullptr;
    }
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
    if (ptrPixelDate)
    {
        delete ptrPixelDate;
        ptrPixelDate = nullptr;
    }
    ui->progressBar->setValue(0);
}

void MainWindow::on_buttonConvert_clicked()
{
    QFile file(ui->labelPath->text().section(QStringLiteral("地图文件:"), 1, -1));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream readIn(&file);
        stringMapDate = readIn.readAll().toLatin1();
        file.close();
        importPoint();
    }
    else {
        ui->labelMessage->setText(QStringLiteral("地图文件打开失败."));
        return;
    }
}

void MainWindow::importPoint()
{
    createPPMDate();
    const char* ptrPos = stringMapDate.data() + stringMapDate.indexOf(QChar('['));
    const char* ptrPosListEnd = stringMapDate.data() + stringMapDate.indexOf(QChar(']'));
    const char* pX = nullptr;
    const char* pY = nullptr;
    char dataTmp[10] = {0};
    while (ptrPos + 2 * sizeof(char) <= ptrPosListEnd)
    {
        QPointF point;
        pX = strstr(ptrPos, "\"x\":") + 4;
        pY = strstr(ptrPos, "\"y\":") - 1;
        strncpy(dataTmp, pX, (pY - pX)/sizeof(char));
        point.setX(atof(dataTmp));
        pY += 5;     // ,"y":
        ptrPos = pY;
        pX = strstr(ptrPos, "}");
        strncpy(dataTmp, pY, (pX - pY)/sizeof(char));
        point.setY(atof(dataTmp));
        ptrPos = pX;
        vector.append(point);
        setPiexlBlack(point.rx() * CM_PRECISION_100, point.ry() * CM_PRECISION_100);
    }
    savePPM();
}

void MainWindow::createPPMDate()
{
    getMapSize();
    qDebug() << QStringLiteral("宽度:") + QString::number(widthPPM) + QStringLiteral(", 高度:") + QString::number(heightPPM);
    ptrPixelDate = new Color[lengthDate];
    memset((char *)ptrPixelDate, 0x11, sizeof(Color) * lengthDate);
    qDebug() << QStringLiteral("数据的长度为") + QString::number(strlen((char*)ptrPixelDate));
    std::cout << (char*)ptrPixelDate << std::endl;
}

void MainWindow::setPiexlBlack(const int x, const int y)
{
    for (int i = -PIXEL_BLACK_ROBOT_SIZE; i <= PIXEL_BLACK_ROBOT_SIZE; i++)
    {
        for (int j = -PIXEL_BLACK_ROBOT_SIZE; j <= PIXEL_BLACK_ROBOT_SIZE; j++)
        {
            if (x > 0 && y > 0)
            {
                if (x < widthPPM && y < heightPPM)
                {
                    setValue(x + i, y + j);
                }
            }
        }
    }
}

void MainWindow::savePPM()
{
    QString fileName = QCoreApplication::applicationDirPath() + QString("/map.ppm");
    FILE *fp;
    fp = fopen(fileName.toLatin1(), "wb+");
    if (fp == nullptr)
    {
        qDebug() << QStringLiteral("文件创建失败.");
        return;
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", widthPPM, heightPPM);
    fprintf(fp, "%d\n", 255);  // RGB marker

    fwrite(ptrPixelDate, heightPPM * widthPPM + 1000, sizeof(Color), fp);
    fclose(fp);
    qDebug() << QStringLiteral("PPM文件保存.");
    if (ptrPixelDate)
    {
        delete ptrPixelDate;
        ptrPixelDate = nullptr;
    }
}

void MainWindow::getMapSize()
{
    QString stringMinPox = QString(stringMapDate).section(QString("\"minPos\":{"), 1, -1).section(QChar('}'), 0, 0);
    QString stringMaxPox = QString(stringMapDate).section(QString("\"maxPos\":{"), 1, -1).section(QChar('}'), 0, 0);
    float minX = stringMinPox.section(QString("\"x\":"), 1, -1).section(QChar(','), 0, 0).toFloat();
    float minY = stringMinPox.section(QString("\"y\":"), 1, -1).section(QChar('}'), 0, 0).toFloat();
    float maxX = stringMaxPox.section(QString("\"x\":"), 1, -1).section(QChar(','), 0, 0).toFloat();
    float maxY = stringMaxPox.section(QString("\"y\":"), 1, -1).section(QChar('}'), 0, 0).toFloat();
    pointMin.setX(minX);
    pointMin.setY(minY);
    pointMax.setX(maxX);
    pointMax.setY(maxY);
    widthPPM = qAbs(maxX - minX) * CM_PRECISION_100;
    heightPPM = qAbs(maxY - minY) * CM_PRECISION_100;
    lengthDate = widthPPM * heightPPM;

    qDebug() << QStringLiteral("pixel width=") + QString::number(widthPPM) + QStringLiteral(", pixel height=") + QString::number(heightPPM);
    qDebug() << QStringLiteral("data length=") + QString::number(lengthDate);
}

void MainWindow::setValue(const int xPixel, const int yPixel)
{
    memset(ptrPixelDate + ((yPixel - 1) * widthPPM + (xPixel - 1)), 0, sizeof(Color));
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
