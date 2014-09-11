#include <QtWidgets>
#include "mainwindow.hpp"

#include <simulations/mazeworld.hpp>
#include <simulations/flatworld.hpp>
#include <pfcpp/mcl.hpp>
#include <pfcpp/utils.hpp>


struct /*MainWindow::*/PrivateDataMaze
{
    mazeworld::Maze maze;
    std::vector<mazeworld::Movement> path;
    mazeworld::Position start_pos;
    size_t step;
    mcl::MCL<mazeworld::Position> mcl;  

    std::vector<std::vector<QGraphicsRectItem*>> cells;
    std::vector<QGraphicsEllipseItem*> path_markers;

    void init_scene(QGraphicsScene& scene, QGraphicsView& view)
    {
        std::tie(maze, start_pos, path) = mazeworld::generate_maze(56, 56);

        std::vector<mazeworld::Position> states;
        for (int i = 0; i < maze.width(); i++)
        {
            for(int j = 0; j < maze.height(); j++)
            {
                if (maze.cell({i,j}) == mazeworld::State::Empty)
                {
                    states.push_back({i,j});
                }
            }
        }

        mcl = mcl::MCL<mazeworld::Position>(states);


        step = 0;
        scene.clear();
        cells.clear();
        path_markers.clear();

        auto dcell = std::min(view.width(), view.height()) / (maze.width() + 1);    
        auto start = start_pos;

        cells.resize(maze.width());
        for (int i = 0; i < maze.width(); i++)
        {
            cells[i].resize(maze.height());
            for(int j = 0; j < maze.height(); j++)
            {
                cells[i][j] = scene.addRect(QRectF(i*dcell, (maze.height()-j)*dcell, dcell, dcell),
                                             QPen(),
                                             QBrush(maze.cell({i,j}) == mazeworld::State::Wall ? QColor("black") : QColor("white")));
            }
        }

        for (auto& m: path)
        {
            path_markers.push_back(scene.addEllipse(QRectF(start.x*dcell + dcell/4, (maze.height()-start.y)*dcell + dcell/4, dcell/2, dcell/2),
                                                    QPen(),
                                                    QBrush(QColor(0,0,255,60))));
            start = start + m;
        }
    }

    void update_view()
    {
        auto m = path[step];
        start_pos = start_pos + m;
        mcl([&](mazeworld::Position p){ return mcl::weight_function(maze.measure_from(start_pos, true),
                                                        maze.measure_from(p));},
            [m](mazeworld::Position p){return p + m;});

        step++;

        path_markers[step]->setBrush(QBrush(QColor(0,255,0,255)));
        for (int i = 0; i < maze.width(); i++)
        {
            for(int j = 0; j < maze.height(); j++)
            {
                if (maze.cell({i,j}) == mazeworld::State::Empty)
                    cells[i][j]->setBrush(QBrush(QColor("white")));
            }
        }

        auto particles = mcl.particles;
        mcl::scale(particles);

        for (auto& p: particles)
        {
            if (maze.cell(p.state) == mazeworld::State::Empty)
                cells[p.state.x][p.state.y]->setBrush(QBrush(QColor(255,0,0,int(p.weight*255))));

        }
    }

};




struct MainWindow::PrivateData
{
    flatworld::Scene flat_scene;
    std::vector<flatworld::Point> path;
    flatworld::Point start_pos;
    size_t step;
    mcl::MCL<flatworld::Point> mcl;

    std::vector<QGraphicsEllipseItem*> particle_markers;
    QGraphicsEllipseItem* position_marker;
    std::vector<QGraphicsLineItem*> rays_markers;

    mcl::GaussianNoise<double> movement_noise;

    PrivateData()
    : movement_noise(5)
    {

    }

    template<typename T>
    QPointF to_q_point(const T& x)
    {
        return {boost::geometry::get<0>(x), -boost::geometry::get<1>(x)};
    }

    void init_scene(QGraphicsScene& scene, QGraphicsView& view)
    {
        start_pos = {-640, -350};
        path = {{0, 50},{0, 50},{0, 50},{0, 50},{0, 50},{0, 50},{0, 50},
                {50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},{50, 0},
                {0, 50},{0, 50},{0, 50},{0, 50},{0, 50},{0, 50},{0, 50}};
        flat_scene = flatworld::generate_scene();
        step = 0;
        scene.clear();        
        particle_markers.clear();

        for(auto& poly: flat_scene.objects())
        {
            QPolygonF qpoly;
            boost::geometry::for_each_point(poly,[&](auto x){qpoly.push_back(to_q_point(x));});
            scene.addPolygon(qpoly, QPen(), QBrush(QColor(100,100,100,255)));
        }

        std::vector<decltype(flat_scene.random_state())> states;
        for (int i = 0; i < 1000; ++i)
        {
            auto p = flat_scene.random_state();
            if (!flat_scene.empty(p))
                continue;
            states.push_back(p);
        }


        mcl = mcl::MCL<flatworld::Point>(states);

        for (auto& p: states)
        {
            auto qp = to_q_point(p);
            particle_markers.push_back(scene.addEllipse(QRectF(qp.x() - 3, qp.y() - 3, 6, 6),
                                                        QPen(QColor(0,0,255,60)),
                                                        QBrush(QColor(0,0,255,60))));
        }

        std::cout << "Scence rect: " << scene.sceneRect().width() << " x " << scene.sceneRect().height() << std::endl;
        view.fitInView(scene.sceneRect());     
    }

    void update_view(QGraphicsScene& scene)
    {
        auto m = path[step];
        start_pos = flatworld::move_point(start_pos, m);        
        mcl([&](auto p){ return mcl::weight_function(flat_scene.measure_from(start_pos),
                                                     flat_scene.measure_from(p));},
            [&](auto p){ return flatworld::move_point(flatworld::move_point(p, m), {movement_noise(), movement_noise()});});

        step++;

        auto qp = to_q_point(start_pos);
        scene.addEllipse(QRectF(qp.x() - 3, qp.y() - 3, 6, 6),
                         QPen(QColor(255,0,0,60)),
                         QBrush(QColor(255,0,0,60)));

        auto particles = mcl.particles;
        for (size_t i = 0; i < particle_markers.size() && particles.size(); ++i)
        {
            auto qp = to_q_point(particles[i].state);
            particle_markers[i]->setRect(QRectF(qp.x() - 3, qp.y() - 3, 6, 6));
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