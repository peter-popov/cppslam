#include <QtWidgets>
#include "mainwindow.hpp"

#include <fstream>



MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    scene = new QGraphicsScene;
    view = new QGraphicsView;
    view->setScene(scene);
    view->setMinimumSize(400,400);
    

    auto buttons = new QWidget;
    auto start_button = new QPushButton("Start");
    auto step_button = new QPushButton("Step");    
    auto buttons_layout = new QVBoxLayout;
    buttons_layout->addWidget(start_button);
    buttons_layout->addWidget(step_button);
    buttons->setLayout(buttons_layout);

    connect(start_button, SIGNAL(clicked()), this, SLOT(on_buttonStart_clicked()));
    connect(step_button, SIGNAL(clicked()), this, SLOT(on_buttonStep_clicked()));

    auto layout = new QHBoxLayout;
    layout->addWidget(buttons, 0, Qt::AlignTop);
    layout->addWidget(view);
    setLayout(layout);    
    
    setWindowTitle("ParticleFilter demo");


}

MainWindow::~MainWindow()
{
}

void MainWindow::on_buttonStart_clicked()
{
    simulation.reset( new SimulationModel(SimulationConfig(2000,12),
                                     std::make_tuple(-250, -150, 0.0),
                                     {{50, 0}, {50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},
                                      {50, 1.570796},
                                      {50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0}},
                                     "default"));   

    scene->clear();
    particle_markers.clear();
    rays_markers.clear();
    particle_markers_dir.clear();

    auto map_features = simulation->export_map();
    for(auto& poly: map_features)
        scene->addPolygon(poly, QPen(), QBrush(QColor(100,100,100,255)));
    
    
    for (size_t i = 0; i < simulation->particle_count(); ++i)
    {
        QPointF qp;
        double dir;
        std::tie(qp, dir) = simulation->particle(i);

        particle_markers.push_back(scene->addEllipse(QRectF(qp.x() - 3, qp.y() - 3, 6, 6),
                                                    QPen(QColor(0,0,255,60)),
                                                    QBrush(QColor(0,0,255,60))));

        auto qp_e = QPointF{qp.x() + 10 * cos(dir), qp.y() + 10*sin(dir)};
        particle_markers_dir.push_back(scene->addLine(QLineF(qp, qp_e),
                                                     QPen(QColor(0,0,255,50))));
    }

    QPointF pos;
    std::tie(pos, std::ignore) = simulation->current_position();
    auto sense_points = simulation->measurment_end_points();
    for (auto sp: sense_points)
    {
        rays_markers.push_back(scene->addLine(QLineF(pos, sp),
                                              QPen(QColor(0,255,0,150), 2, Qt::DotLine)));
    }
 

    position_marker = scene->addEllipse(QRectF(pos.x() - 3, pos.y() - 3, 6, 6),
                                                    QPen(QColor(255,0,0)),
                                                    QBrush(QColor(255,0,0,160)));

    auto rect = scene->sceneRect();
    rect.moveBottomLeft(rect.bottomLeft() + QPointF{-10, -10});
    rect.moveTopRight(rect.topRight() + QPointF{10, 10});
    scene->setSceneRect(rect);    
    view->update();
}


void MainWindow::TimerEvent()
{
    DoStep();
}

void MainWindow::DoStep()
{    
    simulation->update();

    for (size_t i = 0; i < simulation->particle_count(); ++i)
    {
        QPointF qp;
        double dir;
        std::tie(qp, dir) = simulation->particle(i);
        particle_markers[i]->setRect(QRectF(qp.x() - 3, qp.y() - 3, 6, 6));
        particle_markers_dir[i]->setLine(QLineF(qp, QPointF{qp.x() + 10 * cos(dir), qp.y() + 10 * sin(dir)}));
    }

    QPointF pos;
    std::tie(pos, std::ignore) = simulation->current_position();
    auto sense_points = simulation->measurment_end_points();    
    for (size_t i = 0; i < sense_points.size(); ++i)
        rays_markers[i]->setLine(QLineF(pos, sense_points[i]));


    position_marker->setRect(QRectF(pos.x() - 3, pos.y() - 3, 6, 6));
    
    view->update();
}


void MainWindow::on_buttonStep_clicked()
{
    DoStep();
}