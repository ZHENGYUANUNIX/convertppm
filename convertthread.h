#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include <QThread>
#include <QPointF>
#include <QVector>

class ConvertThread : public QThread
{
    Q_OBJECT
    class Color
    {
        unsigned char Red {0};
        unsigned char Green {0};
        unsigned char Blue {0};
    };
public:
    ConvertThread();
    ~ConvertThread();

    void convertStop();
    void setSMapPath(QString &stringPath);
    QString getSMapPath();
    void setStop();

protected:
    void run() Q_DECL_OVERRIDE;

private:

    bool m_stop {false};
    //QVector<QPointF> vector;
    QByteArray stringSMapDate;
    QPointF pointMin;
    QPointF pointMax;
    QString pathSMap;

    int baseX;
    int baseY;
    int heightPPM;
    int widthPPM;
    int lengthDate;
    Color *ptrPPMDate {nullptr};
    Color black;

    void getSMapSize();
    void createPPMDate();
    bool importSMap();
    void savePPM();
    void savePPMDate();
    void setPointRoundValue(const int row, const int column);
    void setPointValue(const int row, const int column);
    bool readSMapDate();
    void clearDate();

signals:
    void progress(int value);
};

#endif // CONVERTTHREAD_H
