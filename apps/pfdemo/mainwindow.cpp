#include <QtWidgets>
#include "mainwindow.hpp"

#include <fstream>



MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    particles_count(1000)
{
    scene = new QGraphicsScene;
    view = new QGraphicsView;
    view->setScene(scene);
    view->setMinimumSize(400,400);
    

    auto buttons = new QWidget;
    auto start_button = new QPushButton("Start");
    auto step_button = new QPushButton("Step");
    auto particles_spinbox = new QSpinBox();
    particles_spinbox->setRange(100, 20000);
    particles_spinbox->setSingleStep(100);
    particles_spinbox->setSuffix(" particles");
    particles_spinbox->setValue(particles_count);    
    auto buttons_layout = new QVBoxLayout;
    buttons_layout->addWidget(start_button);
    buttons_layout->addWidget(step_button);
    buttons_layout->addWidget(particles_spinbox);
    buttons->setLayout(buttons_layout);

    connect(start_button, SIGNAL(clicked()), this, SLOT(on_buttonStart_clicked()));
    connect(step_button, SIGNAL(clicked()), this, SLOT(on_buttonStep_clicked()));
    QObject::connect(particles_spinbox, SIGNAL(valueChanged(int)), this, SLOT(on_particlesCount_changed(int)));

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
    simulation.reset( new SimulationModel(SimulationConfig(particles_count, 12),
                                     std::make_tuple(20, -30, 0.0),
                                     {{5, 0}, {5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},
                                      {5, -1.570796},
                                      {5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0},{5, 0}},
                                     "../data/indoor_1.csv"));   

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

        particle_markers.push_back(scene->addEllipse(QRectF(qp.x() - 0.3, qp.y() - 0.3, 0.6, 0.6),
                                                    QPen(QColor(0,0,255,60)),
                                                    QBrush(QColor(0,0,255,60))));

        auto qp_e = QPointF{qp.x() + 1 * cos(dir), qp.y() + 1*sin(dir)};
        particle_markers_dir.push_back(scene->addLine(QLineF(qp, qp_e),
                                                     QPen(QColor(0,0,255,50))));
    }

    QPointF pos;
    std::tie(pos, std::ignore) = simulation->current_position();
    auto sense_points = simulation->measurment_end_points();
    for (auto sp: sense_points)
    {
        rays_markers.push_back(scene->addLine(QLineF(pos, sp),
                                              QPen(QColor(0,255,0,150), 0.5, Qt::DotLine)));
    }
 

    position_marker = scene->addEllipse(QRectF(pos.x() - 0.3, pos.y() - 0.3, 0.6, 0.6),
                                                    QPen(QColor(255,0,0)),
                                                    QBrush(QColor(255,0,0,160)));

    auto rect = scene->sceneRect();
    view->fitInView(rect);
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
        particle_markers[i]->setRect(QRectF(qp.x() - 0.3, qp.y() - 0.3, 0.6, 0.6));
        particle_markers_dir[i]->setLine(QLineF(qp, QPointF{qp.x() + 1 * cos(dir), qp.y() + 1 * sin(dir)}));
    }

    QPointF pos;
    std::tie(pos, std::ignore) = simulation->current_position();
    auto sense_points = simulation->measurment_end_points();    
    for (size_t i = 0; i < sense_points.size(); ++i)
        rays_markers[i]->setLine(QLineF(pos, sense_points[i]));


    position_marker->setRect(QRectF(pos.x() - 0.5, pos.y() - 0.5, 1, 1));
    
    view->update();
}


void MainWindow::on_buttonStep_clicked()
{
    DoStep();
}

void MainWindow::on_particlesCount_changed(int new_value)
{
    particles_count = new_value;
}