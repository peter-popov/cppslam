#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <memory>
#include "SimulationModel.hpp"

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
    std::unique_ptr<SimulationModel> simulation;
    std::vector<QGraphicsEllipseItem*> particle_markers;
    std::vector<QGraphicsLineItem*> particle_markers_dir;
    
    QGraphicsEllipseItem* position_marker;
    std::vector<QGraphicsLineItem*> rays_markers;

};

#endif // MAINWINDOW_H
