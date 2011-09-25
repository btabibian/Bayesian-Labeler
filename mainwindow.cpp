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
bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);
bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);
void MainWindow::on_action_Save_clicked()
{

    const QSettings::Format XmlFormat =
            QSettings::registerFormat("xml",readXmlFile, writeXmlFile);
     QSettings::setPath(XmlFormat, QSettings::UserScope, "labels");
      QSettings::setDefaultFormat(XmlFormat);
      qDebug()<<"Image name:"<<current_img;
      QString name=current_img;
      name.replace("/","_");
      name.replace(":","");
     QSettings settings(XmlFormat,QSettings::UserScope,"scope", name+".lbls");
     int lbl_count=1;

     for(QVector<img_label*>::iterator i=labels.begin();i<labels.end();i++)
     {
         img_label* lbl=*i;
         settings.setValue("labels/lbl"+QString::number(lbl_count),lbl->getName());
         QVector<QPointF> vertecies= lbl->getVertecies();
         int ver_count=1;
         for(QVector<QPointF>::iterator j=vertecies.begin();j<vertecies.end();j++)
         {
             settings.setValue("labels/lbl"+QString::number(lbl_count)+"/ver"+QString::number(ver_count)+"/x",j->x());
             settings.setValue("labels/lbl"+QString::number(lbl_count)+"/ver"+QString::number(ver_count)+"/y",j->y());
             ver_count++;
         }
         lbl_count++;
     }



}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}

bool readXmlFile( QIODevice& device, QSettings::SettingsMap& map )
{
        QXmlStreamReader xmlReader( &device );

        QString currentElementName;
        while( !xmlReader.atEnd() )
        {
xmlReader.readNext();
                while( xmlReader.isStartElement() )
                {
                        if( xmlReader.name() == "SettingsMap" )
                        {
                                xmlReader.readNext();
                                continue;
                        }

                        if( !currentElementName.isEmpty() )
                        {
                                currentElementName += "/";
                        }
                        currentElementName += xmlReader.name().toString();
                        xmlReader.readNext();
                }

                if( xmlReader.isEndElement() )
                {
                        continue;
                }

                if( xmlReader.isCharacters() && !xmlReader.isWhitespace() )
                {
                        QString key = currentElementName;
                        QString value = xmlReader.text().toString();

                        map[ key ] = value;

                        currentElementName.clear();
                }
        }

         if( xmlReader.hasError() )
         {
                return false;
         }

        return true;
}

bool writeXmlFile( QIODevice& device, const QSettings::SettingsMap& map )
{
    /*! \todo {This way of saving settings does not group labels and their information*/
        QXmlStreamWriter xmlWriter( &device );
        xmlWriter.setAutoFormatting( true );

        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement( "SettingsMap" );

        QSettings::SettingsMap::const_iterator mi = map.begin();
        for( mi; mi != map.end(); ++mi )
        {

                QStringList groups= mi.key().split("/");
                foreach(QString groupName, groups )
                {
                        xmlWriter.writeStartElement( groupName);
                }

                xmlWriter.writeCharacters( mi.value().toString() );

                foreach( QString groupName, groups )
                {
                        xmlWriter.writeEndElement();
                }
        }

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();

        return true;
}

