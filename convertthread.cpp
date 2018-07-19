#include "convertthread.h"
#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QFile>
#include <iostream>
#include <stdlib.h>

#define CM_PRECISION_100 100
#define PIXEL_BLACK_ROBOT_SIZE 10

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

void ConvertThread::setSMapPath(QString &stringPath)
{
    pathSMap = stringPath;
}

QString ConvertThread::getSMapPath()
{
    return pathSMap;
}

void ConvertThread::setStop()
{
    m_stop = true;
}

void ConvertThread::run()
{
    m_stop = false;
    if (readSMapDate())
    {
        getSMapSize();
        createPPMDate();
        if (!importSMap())
        {
            clearDate();
            exit(-1);
        }
    }
    savePPM();
    savePPMDate();
    clearDate();
    return;
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
    widthPPM = qAbs((maxX - minX) * CM_PRECISION_100);
    heightPPM = qAbs((maxY - minY) * CM_PRECISION_100);
    lengthDate = widthPPM * heightPPM;
    baseX = qAbs(minX * CM_PRECISION_100);
    baseY = qAbs(minY * CM_PRECISION_100);

    qDebug() << QStringLiteral("PPM宽度=") + QString::number(widthPPM) + QStringLiteral(", PPM高度=") + QString::number(heightPPM);
    qDebug() << QStringLiteral("PPM数据长度=") + QString::number(lengthDate);
}

void ConvertThread::createPPMDate()
{
    ptrPPMDate = new Color[lengthDate];
    memset(ptrPPMDate, 0xFF, sizeof(Color) * lengthDate);
}

bool ConvertThread::importSMap()
{
    int posStart = stringSMapDate.indexOf(QString("\"normalPosList\""));
    const char* ptrPos = stringSMapDate.data() + posStart;
    const char* ptrPosListEnd = stringSMapDate.data() +  stringSMapDate.indexOf(QChar(']'), posStart);
    const char* pX = nullptr;
    const char* pY = nullptr;
    char dataTmp[10] = {0};
    while (ptrPos + sizeof(char) < ptrPosListEnd)
    {
        if (m_stop)
        {
            return false;
        }
        QPointF point;
        pX = strstr(ptrPos, "\"x\":") + 4;
        pY = strstr(ptrPos, "\"y\":") - 1;
        // smap地图中有异常数据, 例如{"y":5.86}, 没有x数据的点, 需要调整到下一个数据点
        if (pX > pY)
        {
            pX -= 4 * sizeof(char);
            pY = pX;
            ptrPos = pX;
            continue;
        }
        strncpy(dataTmp, pX, (pY - pX)/sizeof(char));
        point.setX(atof(dataTmp) * CM_PRECISION_100 + baseX);
        pY += 5;     // ,"y":
        ptrPos = pY;
        pX = strstr(ptrPos, "}");
        strncpy(dataTmp, pY, (pX - pY)/sizeof(char));
        point.setY(atof(dataTmp) * CM_PRECISION_100 + baseY);
        ptrPos = pX;
        //vector.append(point);
        qDebug() << QString::number(point.rx()) + QString("-") + QString::number(point.ry());
        qDebug() << QString::number(baseX) + QString("-") + QString::number(baseY);
        qDebug() << QString::number(qAbs(point.rx() * CM_PRECISION_100) + baseX) +
                                    QString("=") +
                                    QString::number(qAbs(point.ry() * CM_PRECISION_100) + baseY);

        setPointRoundValue(point.rx(), point.ry());
    }

    return true;
}

void ConvertThread::savePPM()
{
#if defined(Q_OS_WIN32)
    QString fileName = pathSMap.section(QChar('\\'), -1, -1).replace(QString(".smap"), QString(".ppm"), Qt::CaseInsensitive);
#elif defined(Q_OS_LINUX)
    QString fileName("./smap.ppm");
#endif
    FILE *fp;
    fp = fopen(fileName.toLatin1(), "wb+");
    if (fp == nullptr)
    {
        qDebug() << fileName + QStringLiteral("文件创建失败.");
        return;
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", widthPPM, heightPPM);
    fprintf(fp, "%d\n", 255);  // RGB marker
    if (ptrPPMDate)
    {
        fwrite(ptrPPMDate, lengthDate, sizeof(Color), fp);
    }
    fclose(fp);
    qDebug() << QStringLiteral("PPM文件保存.");
}

void ConvertThread::savePPMDate()
{
#if defined(Q_OS_WIN32)
    QString fileName = pathSMap.section(QChar('\\'), -1, -1).replace(QString(".smap"), QString(".dat"), Qt::CaseInsensitive);
#elif define(Q_OS_LINUX)
    QString fileName("./smap.dat");
#endif
    FILE *fp;
    fp = fopen(fileName.toLatin1(), "wb+");
    if (fp == nullptr)
    {
        qDebug() << fileName + QStringLiteral("文件创建失败.");
        return;
    }

    fprintf(fp, "minPosX:%.3f;\nminPosY:%.3f;\nwidthPPM:%d;\nheightPPM:%d;\n以上数据为规划路径需要,请勿手动修改.", pointMin.rx(), pointMin.ry(), widthPPM, heightPPM);
    fclose(fp);
    qDebug() << QStringLiteral("ppm dat文件保存.");
}

void ConvertThread::setPointRoundValue(const int row, const int column)
{
    qDebug() << QString::number(row) + QString("*") + QString::number(column);
    // 因为smap地图是散点，需要连接起来，以便规划路径，考虑到机器人的尺寸，将散点周围的点描黑，形成连接线
    // 具体PIXEL_BLACK_ROBOT_SIZE给定多少，根据实际情况调整
    for (int i = -PIXEL_BLACK_ROBOT_SIZE; i <= PIXEL_BLACK_ROBOT_SIZE; i++)
    {
        for (int j = -PIXEL_BLACK_ROBOT_SIZE; j <= PIXEL_BLACK_ROBOT_SIZE; j++)
        {
            setPointValue(row + i, column + j);
        }
    }
    // 散点地图
    // setPointValue(row, column);
}

void ConvertThread::setPointValue(const int row, const int column)
{
    if (row > 0 && column > 0)
    {
        if (row < widthPPM && column < heightPPM)
        {
            *(ptrPPMDate + (column - 1) * widthPPM + (row - 1)) = black;
        }
    }
}

bool ConvertThread::readSMapDate()
{
    QFile file(pathSMap);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream readIn(&file);
        stringSMapDate = readIn.readAll().toLatin1();
        file.close();
        return true;
    }
    else {
        qDebug() << QStringLiteral("SMAP地图读取失败.");
        return false;
    }
}

void ConvertThread::clearDate()
{
    if (ptrPPMDate)
    {
        delete ptrPPMDate;
        ptrPPMDate = nullptr;
    }
    //vector.clear();
    stringSMapDate.clear();
}

