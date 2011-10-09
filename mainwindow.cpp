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
#include <QSettings>
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
    scene->installEventFilter(this);
    connect(ui->action_Save,SIGNAL(triggered()),this,SLOT(on_action_Save_clicked()));
    ui->graphicsView->installEventFilter(this);
    set_current_label(0);
    mod=NONE;

}
void MainWindow::updateState()
{

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
    loadImageData(current_img);
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
            //Instead of directly changing the state use Add_Object button to toggle its state to.
            ui->action_Add_Object->trigger();
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
        updateLabel(name);
    }

}
void MainWindow::updateLabel(const QString& name)
{
    mod=NONE;
    if(get_current_label()!=0)
        get_current_label()->endEdit();
    else
        return;
    this->get_current_label()->setName(name);
    updateStatus(name+tr(" Added to list of labels"));
}

void MainWindow::drawVertex(QPoint point,bool mapped)
{
    img_label* lbl= get_current_label();
    if(!mapped)
        lbl->addVertex(ui->graphicsView->mapToScene(point));
    else
        lbl->addVertex(point);
    scene->update();

}

void MainWindow::on_action_Add_Object_clicked()
{
    //The button now toggles between two states.
    if(mod!=NONE)
        CompleteEdit();
    else
    {
        img_label* lbl=new img_label();
        this->set_current_label(lbl);
        scene->addItem(lbl);
        this->labels.push_back(lbl);
        mod= ADD_LABEL;
        lbl->startEdit();
        updateStatus(tr("Click on screen to add vertex for a new object, Right Click to finish"));
    }
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

void MainWindow::loadImageData(QString file)
{
    qDebug()<<"Image name:"<<current_img;
    QString name=current_img;
    name+=".lbls.xml";
    readXmlFile(name);
}
void MainWindow::on_action_Save_clicked()
{

    qDebug()<<"Image name:"<<current_img;
    QString name=current_img;
    name+=".lbls.xml";
    writeXmlFile(name);



}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}

bool MainWindow::readXmlFile(QString& file)
{
    int lbl_count=0;
    int x_pos=0;
    int y_pos=0;
    int pnt =0;
    QString name;
    QFile file_handle( file );
    if(!file_handle.open( QIODevice::ReadOnly ))
    {
        qDebug()<<"File Not Found!";
        return false;
    }
    QXmlStreamReader xmlReader( &file_handle );

    while( !xmlReader.atEnd() )
    {

        xmlReader.readNext();

        while( xmlReader.isStartElement() )
        {

            if( xmlReader.name() == "labels" )
            {

                xmlReader.readNext();

                continue;
            }
            if( xmlReader.name() == "lbl"+QString::number(lbl_count) )
            {
                lbl_count++;
                xmlReader.readNext();
                xmlReader.readNext();

                x_pos=xmlReader.readElementText().toInt();

                xmlReader.readNext();

                xmlReader.readNext();

                y_pos=xmlReader.readElementText().toInt();

                xmlReader.readNext();
                xmlReader.readNext();

                name=xmlReader.readElementText();
                xmlReader.readNext();
                ui->action_Add_Object->trigger();
                pnt=0;
                continue;
            }
            if( xmlReader.name() == "pnt"+QString::number(pnt))
            {
                int x=0;
                int y=0;
                xmlReader.readNext();
                xmlReader.readNext();
                x=xmlReader.readElementText().toInt();
                xmlReader.readNext();
                xmlReader.readNext();
                y=xmlReader.readElementText().toInt();
                xmlReader.readNext();
                xmlReader.readNext();

                xmlReader.readNext();

                drawVertex(QPoint(x,y),true);
                pnt++;
                continue;
            }
            xmlReader.readNext();

        }
        if( xmlReader.isEndElement() )
        {
            if(mod==ADD_LABEL)
            {
            updateLabel(name);
            current_lbl->setPos(x_pos,y_pos);
            current_lbl->setName(name);
            }
            continue;
        }
    }

    if( xmlReader.hasError() )
    {
        return false;
    }

    return true;
}

bool MainWindow::writeXmlFile( QString& file )
{
    /*! \todo {This way of saving settings does not group labels and their information*/
    qDebug()<<file;
    QFile file_handle( file );
    file_handle.open( QIODevice::WriteOnly );
    QXmlStreamWriter xmlWriter(&file_handle );
    xmlWriter.setAutoFormatting( true );

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement( "labels" );


    QVector<img_label*>::const_iterator mi=labels.begin();
    int i=0;
    for( mi; mi != labels.end(); ++mi )
    {
        img_label* lbl=*mi;
        xmlWriter.writeStartElement( "lbl"+QString::number(i) );
        xmlWriter.writeStartElement("x");
        xmlWriter.writeCharacters(QString::number(lbl->pos().x()));
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("y");
        xmlWriter.writeCharacters(QString::number(lbl->pos().y()));
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("name");
        xmlWriter.writeCharacters(lbl->getName());
        xmlWriter.writeEndElement();
        int j=0;
        foreach(QPointF ver, lbl->getVertecies())
        {
            xmlWriter.writeStartElement( "pnt"+QString::number(j) );
            xmlWriter.writeStartElement("x");



            xmlWriter.writeCharacters(QString::number(ver.x()));
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("y");
            xmlWriter.writeCharacters(QString::number(ver.y()));
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
            j++;
        }
        xmlWriter.writeEndElement();
        i++;
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file_handle.close();
    return true;
}

