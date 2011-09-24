/ *! \author    Behzad Tabibian
  *
  */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "img_label.h"
#include <QMouseEvent>
#include <iostream>
#include <QTextEdit>
#include <QtGui>
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene =new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    connect(ui->action_TB_Open,SIGNAL(triggered()),this,SLOT(on_action_TB_Open_clicked()));
    connect(ui->actionOpenTB,SIGNAL(triggered()),this,SLOT(on_action_TB_Open_clicked()));
    connect(ui->action_Save,SIGNAL(triggered()),this,SLOT(on_action_Save_clicked()));
    connect(ui->action_Add_Object,SIGNAL(triggered()),this,SLOT(on_action_Add_Object_clicked()));
    connect(ui->actionAdd_ObjectTB,SIGNAL(triggered()),this,SLOT(on_action_Add_Object_clicked()));
    ui->graphicsView->installEventFilter(this);
    QIcon icon_open(":/open.png");
    QIcon icon_add(":/add.png");
    ui->actionOpenTB->setIcon(icon_open);
    ui->actionAdd_ObjectTB->setIcon(icon_add);

}
void MainWindow::on_action_TB_Open_clicked()
{
    current_img = QFileDialog::getOpenFileName(this, tr("Open File"),
                                               "",
                                               tr("Files (*.*)"));
    displayImage(current_img);

}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type t=event->type();

    if (t == QEvent::MouseButtonPress  ) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if((mod==ADD_LABEL) && (mouseEvent->button()==Qt::LeftButton))
            drawVertex(mouseEvent->pos());
        else if((mod==ADD_LABEL) && (mouseEvent->button()==Qt::RightButton))
        {mod=NONE;
            DisplayBox(mouseEvent->pos());
        }
    } else {
        // standard event processing
    }
    return QObject::eventFilter(obj, event);
}
void MainWindow::DisplayBox(QPointF point)
{
    QString name=QInputDialog::getText(this,"Object Name","Object Name",QLineEdit::Normal);
    this->get_current_label()->setName(name);

}
void MainWindow::drawVertex(QPoint point)
{
    img_label* lbl= get_current_label();
    std::cout<<point.x()<<","<<point.y()<<std::endl;
    lbl->addVertex(ui->graphicsView->mapToScene(point));
    scene->update();
}

void MainWindow::on_action_Add_Object_clicked()
{
    img_label* lbl=new img_label();
    this->set_current_label(lbl);
    scene->addItem(lbl);
    this->labels.push_back(lbl);
    mod= ADD_LABEL;
}
img_label* MainWindow::get_current_label()
{
    return current_lbl;
}
void MainWindow::set_current_label(img_label* lbl)
{
    this->current_lbl = lbl;
}

void MainWindow::displayImage(QString file_name)
{

    pixmap.load(file_name);
    scene->addPixmap(pixmap);
    pixmap.scaled(QSize(100,200));
    ui->graphicsView->show();
}
void MainWindow::on_action_Save_clicked()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}
