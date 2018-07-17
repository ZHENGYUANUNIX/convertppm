#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointF>
#include <QVector>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    class Color
    {
      char Red;
      char Green;
      char Blue;

    };
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void savePPM();
    void importPoint();

private slots:
    void on_buttonLoadMap_clicked();
    void on_buttonConvert_clicked();

private:
    void createPPMDate();
    void setPiexlBlack(const int x, const int y);
    void getMapSize();
    void setValue(const int xPixel, const int yPixel);


    const char* __strstr(const char* src, const char* needle);

    Ui::MainWindow *ui;
    QVector<QPointF> vector;
    QByteArray stringMapDate;
    QPointF pointMin;
    QPointF pointMax;

    int heightPPM;
    int widthPPM;
    int lengthDate;
    Color *ptrPixelDate {nullptr};

};

#endif // MAINWINDOW_H
