#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsView>
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
    void init_scene();
    void update_view();
    void DoStep();

private:

    QGraphicsView* view;
    QGraphicsScene *scene;    
    QTimer timer;

    std::vector<std::vector<QGraphicsRectItem*>> m_cells;
    std::vector<QGraphicsEllipseItem*> m_path_markers;

private:
    struct PrivateData;
    std::unique_ptr<PrivateData> d;
};

#endif // MAINWINDOW_H
