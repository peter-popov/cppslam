#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <memory>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void TimerEvent();
    void on_buttonStart_clicked();
    void on_buttonStep_clicked();

private:
    void DoStep();

private:

    QGraphicsView* view;
    QGraphicsScene *scene;    
    QTimer timer;

private:
    struct PrivateData;
    std::unique_ptr<PrivateData> d;
};

#endif // MAINWINDOW_H
