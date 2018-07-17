#include "convertthread.h"
#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QFile>
#include <iostream>
#include <stdlib.h>

#define CM_PRECISION_100 100
#define PIXEL_BLACK_ROBOT_SIZE 5

ConvertThread::ConvertThread()
{

}

ConvertThread::~ConvertThread()
{
    if (ptrPPMDate)
    {
        delete ptrPPMDate;
        ptrPPMDate = nullptr;
    }
}

void ConvertThread::run()
{
    m_stop = false;
    getSMapSize();
    createPPMDate();
    importSMap();
    savePPM();
}

void ConvertThread::getSMapSize()
{
    QString stringMinPox = QString(stringSMapDate).section(QString("\"minPos\":{"), 1, -1).section(QChar('}'), 0, 0);
    QString stringMaxPox = QString(stringSMapDate).section(QString("\"maxPos\":{"), 1, -1).section(QChar('}'), 0, 0);
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

void ConvertThread::createPPMDate()
{
    qDebug() << QStringLiteral("宽度:") + QString::number(widthPPM) + QStringLiteral(", 高度:") + QString::number(heightPPM);
    ptrPPMDate = new Color[lengthDate];
    memset((char *)ptrPPMDate, 0x11, sizeof(Color) * lengthDate);
    qDebug() << QStringLiteral("数据的长度为") + QString::number(strlen((char*)ptrPPMDate));
    std::cout << (char*)ptrPPMDate << std::endl;
}

void ConvertThread::importSMap()
{
    const char* ptrPos = stringSMapDate.data() + stringSMapDate.indexOf(QChar('['));
    const char* ptrPosListEnd = stringSMapDate.data() + stringSMapDate.indexOf(QChar(']'));
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
        setPointRoundValue(point.rx() * CM_PRECISION_100, point.ry() * CM_PRECISION_100);
    }
}

void ConvertThread::savePPM()
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

    fwrite(ptrPPMDate, heightPPM * widthPPM + 1000, sizeof(Color), fp);
    fclose(fp);
    qDebug() << QStringLiteral("PPM文件保存.");
    if (ptrPPMDate)
    {
        delete ptrPPMDate;
        ptrPPMDate = nullptr;
    }
    vector.clear();
}

void ConvertThread::setPointRoundValue(const int row, const int column)
{
    for (int i = -PIXEL_BLACK_ROBOT_SIZE; i <= PIXEL_BLACK_ROBOT_SIZE; i++)
    {
        for (int j = -PIXEL_BLACK_ROBOT_SIZE; j <= PIXEL_BLACK_ROBOT_SIZE; j++)
        {
            if (row > 0 && column > 0)
            {
                if (row < widthPPM && column < heightPPM)
                {
                    setPointValue(row + i, column + j);
                }
            }
        }
    }
}

void ConvertThread::setPointValue(const int row, const int column)
{
    memset(ptrPPMDate + ((column - 1) * widthPPM + (row - 1)), 0, sizeof(Color));
}

