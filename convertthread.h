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
      char Red;
      char Green;
      char Blue;

    };
public:
    ConvertThread();
    ~ConvertThread();

    void convertStop();
public:
    QString pathSMap;
protected:
    void run() Q_DECL_OVERRIDE;

private:

    bool m_stop {false};
    QVector<QPointF> vector;
    QByteArray stringSMapDate;
    QPointF pointMin;
    QPointF pointMax;

    int heightPPM;
    int widthPPM;
    int lengthDate;
    Color *ptrPPMDate {nullptr};

    void getSMapSize();
    void createPPMDate();
    void importSMap();
    void savePPM();
    void setPointRoundValue(const int row, const int column);
    void setPointValue(const int row, const int column);
signals:
    void progress(int value);
};

#endif // CONVERTTHREAD_H
