/*! \author    Behzad Tabibian
 *  \date      Sep. 24, 2011
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "img_label.h"
#include <QMouseEvent>
#include <iostream>
#include <QTextEdit>
#include <QtGui>
#include <math.h>
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene =new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    connect(ui->action_TB_Open,SIGNAL(triggered()),this,SLOT(on_action_TB_Open_clicked()));
    connect(ui->action_Save,SIGNAL(triggered()),this,SLOT(on_action_Save_clicked()));
    connect(ui->action_Add_Object,SIGNAL(triggered()),this,SLOT(on_action_Add_Object_clicked()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(on_action_Delete_clicked()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(on_action_ZoomIn_clicked()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(on_action_ZoomOut_clicked()));
    scene->installEventFilter(this);
    ui->graphicsView->installEventFilter(this);
    set_current_label(0);
    mod=NONE;
    numberOfZooms=0;
    wheelDeltaInDegrees=0;
}
void MainWindow::updateState()
{

}

void MainWindow::on_action_ZoomIn_clicked()
{

    if (numberOfZooms >= MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomIn->setEnabled(false);
    } else {
        ui->graphicsView->scale(2.0, 2.0);
        numberOfZooms++;
    }

    if (numberOfZooms > -MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomOut->setEnabled(true);
    }

    if (numberOfZooms == 0) {
        updateStatus(tr("Zoom Level: 100%"));
    } else if (numberOfZooms > 0) {
        updateStatus(tr("Zoom Level: ")+QString::number(pow(2.0, (double)numberOfZooms)*100)+"%");
    } else {
        updateStatus(tr("Zoom Level: ")+QString::number(100/pow(2.0, (double)-numberOfZooms))+"%");
    }

}

void MainWindow::on_action_ZoomOut_clicked()
{

    if (numberOfZooms <= -MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomOut->setEnabled(false);
    } else {
        ui->graphicsView->scale(0.5, 0.5);
        numberOfZooms--;
    }

    if (numberOfZooms < MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomIn->setEnabled(true);
    }

    if (numberOfZooms == 0) {
        updateStatus(tr("Zoom Level: 100%"));
    } else if (numberOfZooms > 0) {
        updateStatus(tr("Zoom Level: ")+QString::number(pow(2.0, (double)numberOfZooms)*100)+"%");
    } else {
        updateStatus(tr("Zoom Level: ")+QString::number(100/pow(2.0, (double)-numberOfZooms))+"%");
    }

}

void MainWindow::on_action_Delete_clicked()
{

    if(get_current_label())
    {
        img_label* curr=get_current_label();
        scene->removeItem(curr);
        labels.remove(labels.indexOf(curr));
        set_current_label(0);
    }
}

void MainWindow::on_action_TB_Open_clicked()
{
    current_img = QFileDialog::getOpenFileName(this, tr("Open File"),
                                               "",
                                               tr("Files (*.*)"));
    displayImage(current_img);

}

void MainWindow::wheelEvent(QWheelEvent *event)
{

    wheelDeltaInDegrees += (event->delta() / 8);

    if (wheelDeltaInDegrees >= MOUSE_WHEEL_SENSITIVITY) { // wheel moved forward
        on_action_ZoomIn_clicked();
        wheelDeltaInDegrees = 0;
    } else if (wheelDeltaInDegrees <= -MOUSE_WHEEL_SENSITIVITY) { // wheel moved backward
        on_action_ZoomOut_clicked();
        wheelDeltaInDegrees = 0;
    }

}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type t=event->type();
    if(t==QEvent::GraphicsSceneMouseRelease)
    {
        if(mod==NONE)
        {
            if(scene->selectedItems().count()>0)
                set_current_label(static_cast<img_label*>(scene->selectedItems().at(0)));
            else
                set_current_label(0);
        }
    }
    if (t == QEvent::MouseButtonPress  ) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if((mod==ADD_LABEL) && (mouseEvent->button()==Qt::LeftButton))
            drawVertex(mouseEvent->pos());
        else if((mod==ADD_LABEL) && (mouseEvent->button()==Qt::RightButton))
        {
            CompleteEdit();
        }


    } else {

        // standard event processing
    }
    return QObject::eventFilter(obj, event);
}
void MainWindow::CompleteEdit()
{
    bool ok=false;
    QString name=QInputDialog::getText(this,"Object Name","Object Name",QLineEdit::Normal,"",&ok);
    if(ok)
    {
        mod=NONE;
        if(get_current_label()!=0)
            get_current_label()->endEdit();
        else
            return;
        this->get_current_label()->setName(name);
        updateStatus(name+tr(" Added to list of labels"));
    }

}
void MainWindow::drawVertex(QPoint point)
{
    img_label* lbl= get_current_label();
    lbl->addVertex(ui->graphicsView->mapToScene(point));
    scene->update();

}

void MainWindow::on_action_Add_Object_clicked()
{
    if(mod!=NONE)
        CompleteEdit();
    img_label* lbl=new img_label();
    this->set_current_label(lbl);
    scene->addItem(lbl);
    this->labels.push_back(lbl);
    mod= ADD_LABEL;
    lbl->startEdit();
    updateStatus(tr("Click on screen to add vertex for a new object, Right Click to finish"));
}
void MainWindow::updateStatus(QString msg)
{
    statusBar()->showMessage(msg);
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

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}
