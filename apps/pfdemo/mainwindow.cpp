#include <QtWidgets>
#include "mainwindow.hpp"

#include <simulations/mazeworld.hpp>
#include <simulations/flatworld.hpp>
#include <map/shapes_map.hpp>
#include <pfcpp/mcl.hpp>
#include <pfcpp/utils.hpp>


using namespace pfcpp::maps;
using namespace flatworld;


struct MainWindow::PrivateData
{

    struct Control
    {
        double v;
        double w;    
    };

    pfcpp::maps::ShapesMap map;
    std::vector<Control> path;
    flatworld::Pose start_pos;
    size_t step;
    mcl::MCL<flatworld::Pose> mcl;



    std::vector<QGraphicsEllipseItem*> particle_markers;
    std::vector<QGraphicsLineItem*> particle_markers_dir;
    
    QGraphicsEllipseItem* position_marker;
    std::vector<QGraphicsLineItem*> rays_markers;

    mcl::VelocityMotionModelSampler movement_model;
    
    PrivateData()
    {

    }

    QPoint to_q_point(const ShapesMap::Point& x)
    {
        return {boost::geometry::get<0>(x), -boost::geometry::get<1>(x)};
    }

    QPoint to_q_point(const ShapesMap::Position& x)
    {
        return {std::get<0>(x), -std::get<1>(x)};
    }

    QPoint to_q_point(const flatworld::Pose& p)
    {
        return {p.x, -p.y};
    }


    void init_scene(QGraphicsScene& scene, QGraphicsView& view)
    {
        start_pos = {-640, -350, 1.570796};
        path = {{50, 0},
                {50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},
                {50, -1.570796},
                {50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},
                {50, 1.570796},
                {50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0}};
        map = flatworld::generate_scene();
        step = 0;
        scene.clear();        
        particle_markers.clear();

        for(auto& poly: map.objects)
        {
            QPolygonF qpoly;
            boost::geometry::for_each_point(poly,[&](auto x){qpoly.push_back(to_q_point(x));});
            scene.addPolygon(qpoly, QPen(), QBrush(QColor(100,100,100,255)));
        }

        std::vector<Pose> states;
        for (int i = 0; i < 3000; ++i)
        {
            auto p = random_pose(map);
            if (map.is_occupied(std::make_tuple(p.x,p.y)))
                continue;
            states.push_back(p);
        }


        mcl = mcl::MCL<flatworld::Pose>(states, {{0.05,0.5,0.002,0.1,0.0,0.0}});

        for (auto& p: states)
        {
            auto qp = to_q_point(p);
            particle_markers.push_back(scene.addEllipse(QRectF(qp.x() - 3, qp.y() - 3, 6, 6),
                                                        QPen(QColor(0,0,255,60)),
                                                        QBrush(QColor(0,0,255,60))));

            auto qp_e = QPointF{p.x + 10*cos(p.direction), -p.y - 10*sin(p.direction)};
            particle_markers_dir.push_back(scene.addLine(QLineF(qp, qp_e),
                                                     QPen(QColor(0,0,255,60))));
        }

        std::vector<ShapesMap::Position> sense_points;
        std::tie(std::ignore, sense_points) = measurement_with_coords(start_pos, map);
        for (size_t i = 0; i < sense_points.size(); ++i)
        {
            rays_markers.push_back(scene.addLine(QLine(to_q_point(start_pos), to_q_point(sense_points[i])),
                                            QPen(QColor(0,255,0,130), 2, Qt::DotLine)));
        }

        view.fitInView(scene.sceneRect());     
    }

    void update_view(QGraphicsScene& scene)
    {

        auto m = path[step];
        start_pos = movement_model(start_pos, m);        
        mcl([&](auto p){ return mcl::weight_function(flatworld::measurement(start_pos, map),
                                                     flatworld::measurement(p, map));},
            m);

        std::vector<ShapesMap::Position> sense_points;
        std::tie(std::ignore, sense_points) = measurement_with_coords(start_pos, map);
        for (size_t i = 0; i < sense_points.size() && i < rays_markers.size(); ++i)
        {
            rays_markers[i]->setLine(QLineF(to_q_point(start_pos), to_q_point(sense_points[i])));
        }

        step++;

        auto qp = to_q_point(start_pos);
        scene.addEllipse(QRectF(qp.x() - 3, qp.y() - 3, 6, 6),
                         QPen(QColor(255,0,0,60)),
                         QBrush(QColor(255,0,0,60)));

        auto particles = mcl.particles;
        for (size_t i = 0; i < particle_markers.size() && particles.size(); ++i)
        {                
            auto ps = particles[i].state;
            auto qp = to_q_point(ps);
            particle_markers[i]->setRect(QRectF(qp.x() - 3, qp.y() - 3, 6, 6));
            auto qp_e = QPoint{static_cast<int>(ps.x + 10*cos(ps.direction)), 
                               static_cast<int>(-ps.y - 10*sin(ps.direction))};
        
            particle_markers_dir[i]->setLine(QLine(qp, qp_e));
        }
    }
};


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    d(new PrivateData())
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
    
    setWindowTitle("MCL demo");
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_buttonStart_clicked()
{
    d->init_scene(*scene, *view);
    d->update_view(*scene);
    view->update();
}


void MainWindow::TimerEvent()
{
    if (d->step + 1 < d->path.size())
    {
        DoStep();
    }
    else
    {
        timer.stop();
    }
}

void MainWindow::DoStep()
{    
    d->update_view(*scene);
    view->update();
}


void MainWindow::on_buttonStep_clicked()
{
    DoStep();
}