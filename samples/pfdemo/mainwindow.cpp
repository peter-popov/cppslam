#include "mainwindow.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QTime>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../../mazeworld.hpp"
#include "../../mcl.hpp"


struct MainWindow::PrivateData
{
    mazeworld::Maze maze;
    std::vector<mazeworld::Movement> path;
    mazeworld::Position start_pos;
    size_t step;
    mcl::MCL<mazeworld::Position> mcl;
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
    std::tie(d->maze, d->start_pos, d->path) = mazeworld::generate_maze(56, 56);

    std::vector<mazeworld::Position> states;
    for (int i = 0; i < d->maze.width(); i++)
    {
        for(int j = 0; j < d->maze.height(); j++)
        {
            if (d->maze.cell({i,j}) == mazeworld::State::Empty)
            {
                states.push_back({i,j});
            }
        }
    }

    d->mcl = mcl::MCL<mazeworld::Position>(states);

    init_scene();

    d->step = 0;
    update_view();

    //connect(&timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    //this->timer.start(1000);
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
    auto m = d->path[d->step];
    d->start_pos = d->start_pos + m;
    d->mcl([&](mazeworld::Position p){ return mcl::weight_function(d->maze.measure_from(d->start_pos),
                                                    d->maze.measure_from(p));},
           [m](mazeworld::Position p){return p + m;});

    d->step++;
    update_view();
}


void MainWindow::init_scene()
{
    scene->clear();
    m_cells.clear();
    m_path_markers.clear();

    auto maze = d->maze;
    auto start = d->start_pos;
    auto path = d->path;

    auto dcell = std::min(view->width(), view->height()) / (maze.width() + 1);    


    m_cells.resize(maze.width());
    for (int i = 0; i < maze.width(); i++)
    {
        m_cells[i].resize(maze.height());
        for(int j = 0; j < maze.height(); j++)
        {
            m_cells[i][j] = scene->addRect(QRectF(i*dcell, (maze.height()-j)*dcell, dcell, dcell),
                                           QPen(),
                                           QBrush(maze.cell({i,j}) == mazeworld::State::Wall ? QColor("black") : QColor("white")));
        }
    }

    for (auto& m: path)
    {
        m_path_markers.push_back(scene->addEllipse(QRectF(start.x*dcell + dcell/4, (maze.height()-start.y)*dcell + dcell/4, dcell/2, dcell/2),
                                                 QPen(),
                                                 QBrush(QColor(0,0,255,60))));
        start = start + m;
    }
}

void MainWindow::update_view()
{
    m_path_markers[d->step]->setBrush(QBrush(QColor(0,255,0,255)));
    for (int i = 0; i < d->maze.width(); i++)
    {
        for(int j = 0; j < d->maze.height(); j++)
        {
            if (d->maze.cell({i,j}) == mazeworld::State::Empty)
                m_cells[i][j]->setBrush(QBrush(QColor("white")));
        }
    }

    auto particles = d->mcl.particles;
    mcl::scale(particles);

    for (auto& p: particles)
    {
        if (d->maze.cell(p.state) == mazeworld::State::Empty)
            m_cells[p.state.x][p.state.y]->setBrush(QBrush(QColor(255,0,0,int(p.weight*255))));

    }
    view->update();
}

void MainWindow::on_buttonStep_clicked()
{
    DoStep();
}
