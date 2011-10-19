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
#include <math.h>
#include <QtAlgorithms>
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    current_img="";
    ui->setupUi(this);
    scene =new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    connect(ui->action_TB_Open,SIGNAL(triggered()),this,SLOT(on_action_TB_Open_clicked()));
    connect(ui->action_Save,SIGNAL(triggered()),this,SLOT(on_action_Save_clicked()));
    connect(ui->action_Add_Object,SIGNAL(triggered()),this,SLOT(on_action_Add_Object_clicked()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(on_action_Delete_clicked()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(on_action_ZoomIn_clicked()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(on_action_ZoomOut_clicked()));
    connect(ui->actionShow_Directories, SIGNAL(toggled(bool)), this, SLOT(showDirectoriesToggled(bool)));
    connect(ui->actionShow_Thumbnails, SIGNAL(toggled(bool)), this, SLOT(showThumbnailsToggled(bool)));
    connect(ui->actionZoomReset, SIGNAL(triggered()), this, SLOT(on_action_ZoomReset_Clicked()));
    scene->installEventFilter(this);
    ui->graphicsView->installEventFilter(this);
    set_current_label(0);
    mod=NONE;
    numberOfZooms=0;
    wheelDeltaInDegrees=0;

    // add the docks to the main window
    dirDock = new QDockWidget(tr("File Explorer"));
    dirDock->setObjectName("FExplore");
    dirDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connect(dirDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dirDockVisible(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, dirDock);
    thumbnailDock = new QDockWidget(tr("Thumbnail Viewer"));
    thumbnailDock->setObjectName("ThumbnailViewer");
    thumbnailDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connect(thumbnailDock, SIGNAL(visibilityChanged(bool)), this, SLOT(thumbnailDockVisible(bool)));
    splitDockWidget(dirDock, thumbnailDock, Qt::Vertical);

    // create the directory model and treeview
    dirModel = new QFileSystemModel();
    dirModel->setRootPath("/");
    treeView = new QTreeView();
    treeView->setModel(dirModel);
    treeView->setColumnHidden(1, true);
    treeView->setColumnHidden(2, true);
    treeView->setColumnHidden(3, true);
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(treeViewDoubleClicked(QModelIndex)));
    connect(treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(treeViewDirectoryCollapsed(QModelIndex)));
    connect(treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(treeViewDirectoryExpanded(QModelIndex)));

    // create the thumbnail list widget
    thumbnailList = new QListWidget();
    thumbnailList->setViewMode(QListView::IconMode);
    thumbnailList->setIconSize(QSize(150, 150));
    thumbnailList->setDragEnabled(false);
    connect(thumbnailList, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(thumbnailListItemDoubleClicked(QListWidgetItem *)));

    // set the sizes of the docks
    dirDock->setMinimumSize(175, 200);
    thumbnailDock->setMinimumSize(175, 200);

    // add the widgets to their docks
    dirDock->setWidget(treeView);
    thumbnailDock->setWidget(thumbnailList);
    // initializing inference system.
    img_add=QPixmap(":/add.png");
    img_edit=QPixmap(":/delete");
    img_open=QPixmap(":/open.png");
    img_save=QPixmap(":/save");
    current_action=START;
    nextOptDock = new QDockWidget(tr("Suggested Action"),this);
    nextOptDock->setObjectName("SuggestedAct");
    nextOptDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connect(nextOptDock, SIGNAL(visibilityChanged(bool)), this, SLOT(nextOptDockVisible(bool)));
    splitDockWidget(thumbnailDock, nextOptDock, Qt::Vertical);
    nextOptWidget=new QPushButton(this);
    nextOptWidget->setText("");
    nextOptDock->setWidget(nextOptWidget);
    nextOptDock->setMinimumSize(175, 70);
    nextOptDock->setMaximumSize(175, 90);
    nextOptWidget->setFlat(true);
    nextOptWidget->setIconSize(QSize(50,50));
    connect(nextOptWidget, SIGNAL(clicked()), this, SLOT(nextOptWidgetPushed()));


}
void MainWindow::storeWindowLayout()
{
    windowLayout=this->saveState();
}
void MainWindow::restoreWindowLayout()
{
    this->restoreState(windowLayout);
}

void MainWindow::hideEvent(QHideEvent *evt)
{
    if(evt->spontaneous())
    {
        storeWindowLayout();

    }
}
void MainWindow::showEvent(QShowEvent *evt)
{

    restoreWindowLayout();
}

void MainWindow::nextOptDockVisible(bool isVisible)
{
    if (isVisible) {
        ui->actionShow_Suggestions->setChecked(true);
    } else {
        ui->actionShow_Suggestions->setChecked(false);

    }

}

void MainWindow::dirDockVisible(bool isVisible)
{
    if (isVisible) {
        ui->actionShow_Directories->setChecked(true);
    } else {
        ui->actionShow_Directories->setChecked(false);
    }
}

void MainWindow::thumbnailDockVisible(bool isVisible)
{
    if (isVisible) {
        ui->actionShow_Thumbnails->setChecked(true);
    } else {
        ui->actionShow_Thumbnails->setChecked(false);
    }
}

void MainWindow::showDirectoriesToggled(bool checked)
{
    if (checked) {
        dirDock->setVisible(true);
    } else {
        dirDock->setVisible(false);
    }
}

void MainWindow::showThumbnailsToggled(bool checked)
{
    if (checked) {
        thumbnailDock->setVisible(true);
    } else {
        thumbnailDock->setVisible(false);
    }
}

void MainWindow::thumbnailListItemDoubleClicked(QListWidgetItem *item)
{
    QString fileName(item->text());
    QString fullPath(directoryPath);
    fullPath.append(fileName);
    displayImage(fullPath);
}

void MainWindow::treeViewDoubleClicked(QModelIndex index)
{
    if (!dirModel->isDir(index)) {
        displayImage(dirModel->filePath(index));
    }
}

void MainWindow::treeViewDirectoryCollapsed(QModelIndex index)
{
    treeViewDirectoryChanged(dirModel->parent(index));
}

void MainWindow::treeViewDirectoryExpanded(QModelIndex index)
{
    treeViewDirectoryChanged(index);
}

void MainWindow::treeViewDirectoryChanged(QModelIndex index)
{

    // clear all the old thumbnails from the previous directory
    for (int i=0; i<thumbnailList->count(); i++) {
        delete thumbnailList->item(i);
    }
    thumbnailList->clear();

    // get the new directory
    directoryPath = dirModel->filePath(index);
    QDir directory(directoryPath);

    // quick fix for formatting issues when in the root directory
    if (!directoryPath.endsWith("/")) {
        directoryPath.append("/");
    }

    // filter for image files only - according to QImageReader::supportedImageFormats
    QStringList filter;
    filter << "*.bmp" << "*.gif" << "*.jpg" << "*.jpeg" << "*.mng" << "*.png" << "*.pbm" << "*.pgm" << "*.ppm" << "*.tiff" << "*.tif" << "*.xbm" << "*.xpm" << "*.svg";
    QStringList files = directory.entryList(filter);

    // increment through the list of image files
    for (int i=0; i < files.size(); i++) {

        // create a thumbnail and add it to the list
        QIcon *thumbnail = new QIcon(directoryPath + files.at(i));
        QListWidgetItem *listItem = new QListWidgetItem(*thumbnail, files.at(i));
        thumbnailList->addItem(listItem);

    }

}

void MainWindow::updateState()
{

}

void MainWindow::on_action_ZoomIn_clicked()
{

    if (numberOfZooms >= MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomIn->setEnabled(false);
    } else {
        ui->graphicsView->scale(ZOOM_SCALE, ZOOM_SCALE);
        numberOfZooms++;
    }

    if (numberOfZooms > -MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomOut->setEnabled(true);
    }

    if (numberOfZooms == 0) {
        updateStatus(tr("Zoom Level: 100%"));
    } else if (numberOfZooms > 0) {
        updateStatus(tr("Zoom Level: ")+QString::number(pow(ZOOM_SCALE, (double)numberOfZooms)*100,'f',0)+"%");
    } else {
        updateStatus(tr("Zoom Level: ")+QString::number(100/pow(ZOOM_SCALE, (double)-numberOfZooms),'f',0)+"%");
    }

}

void MainWindow::on_action_ZoomReset_Clicked()
{
    ui->actionZoomIn->setEnabled(true);
    ui->actionZoomOut->setEnabled(true);
    ui->graphicsView->resetMatrix();
    updateStatus(tr("Zoom Level: 100%"));
    numberOfZooms = 0;
}

void MainWindow::on_action_ZoomOut_clicked()
{

    if (numberOfZooms <= -MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomOut->setEnabled(false);
    } else {
        ui->graphicsView->scale(1/ZOOM_SCALE, 1/ZOOM_SCALE);
        numberOfZooms--;
    }

    if (numberOfZooms < MAX_NUMBER_OF_ZOOMS) {
        ui->actionZoomIn->setEnabled(true);
    }

    if (numberOfZooms == 0) {
        updateStatus(tr("Zoom Level: 100%"));
    } else if (numberOfZooms > 0) {
        updateStatus(tr("Zoom Level: ")+QString::number(pow(ZOOM_SCALE, (double)numberOfZooms)*100,'f',0)+"%");
    } else {
        updateStatus(tr("Zoom Level: ")+QString::number(100/pow(ZOOM_SCALE, (double)-numberOfZooms),'f',0)+"%");
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
        changeState(EDIT);
    }

}
bool MainWindow::checkSave()
{
    QMessageBox msgBox;
    msgBox.setText("The image has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Save:
        ui->action_Save->trigger();
        return true;
        break;
    case QMessageBox::Discard:
        return true;
        break;
    case QMessageBox::Cancel:
        return false;
        break;
    default:
        // should never be reached
        break;
    }


}

void MainWindow::on_action_TB_Open_clicked()
{
    if(current_img==""||checkSave())
    {
        displayImage(QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  "",
                                                  tr("Files (*.*)")));
    }
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
    else
        ui->action_Add_Object->setChecked(true);

}
void MainWindow::updateLabel(const QString& name)
{
    mod=NONE;
    if(get_current_label()!=0)
        get_current_label()->endEdit();
    else
        return;
    this->get_current_label()->setName(name);
    ui->action_Add_Object->setChecked(false);
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
void MainWindow::changeState(ACTIONS new_state)
{

    predictor.updateVariable(current_action,new_state);
    current_action=new_state;
    ACTIONS next_prob=predictor.inferNextAction(current_action);

    //nextOptWidget->setText(predictor.actionName(next_prob));
    switch(next_prob)
    {
    case ADD:
        predicted_action=next_prob;
        nextOptWidget->setIcon(img_add);
        nextOptWidget->setText(tr("Add"));
        break;
    case EDIT:
        predicted_action=next_prob;
        nextOptWidget->setIcon(img_edit);
        nextOptWidget->setText(tr("Edit"));
        break;
    case OPEN:
        predicted_action=next_prob;
        nextOptWidget->setIcon(img_open);
        nextOptWidget->setText(tr("Open"));
        break;
    case SAVE:
        predicted_action=next_prob;
        nextOptWidget->setIcon(img_save);
        nextOptWidget->setText(tr("Save"));
        break;
    default:
        return;

    }
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
        changeState(ADD);
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
    if(file_name=="")
        return;
    current_img=file_name;
    scene->clear();
    qDeleteAll(scene->items());
    labels.clear();
    on_action_ZoomReset_Clicked();
    pixmap.fill();
    pixmap.load(file_name);
    scene->addPixmap(pixmap);
    pixmap.scaled(QSize(100,200));
    ui->graphicsView->show();
    loadImageData(current_img);
    changeState(OPEN);
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
    changeState(SAVE);


}
MainWindow::~MainWindow()
{
    delete ui;
    delete treeView;
    delete dirModel;
    delete dirDock;
    delete thumbnailList;
    delete thumbnailDock;
}

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    predictor.disable();
    if(current_img!=""&&!checkSave())
        event->ignore();
    predictor.enable();
}

bool MainWindow::readXmlFile(QString& file)
{
    predictor.disable();
    int lbl_count=0;
    int x_pos=0;
    int y_pos=0;
    int pnt =0;
    QString name;
    QFile file_handle( file );
    ACTIONS pre=current_action;
    if(!file_handle.open( QIODevice::ReadOnly ))
    {
        qDebug()<<"File Not Found!";
        changeState(pre);
        predictor.enable();
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
    updateStatus(QString::number(lbl_count)+" labels loaded!");
    changeState(pre);
    predictor.enable();

    return true;
}

bool MainWindow::writeXmlFile( QString& file )
{

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


void MainWindow::on_actionShow_Suggestions_toggled(bool checked )
{
    if (checked) {
        nextOptDock->setVisible(true);
    } else {
        nextOptDock->setVisible(false);
    }
}
void MainWindow::nextOptWidgetPushed()
{
    selectAction(predicted_action);
}

void MainWindow::selectAction(ACTIONS action)
{

    switch(action)
    {
    case ADD:
        ui->action_Add_Object->trigger();
        break;
    case EDIT:
        ui->actionDelete->trigger();
        break;
    case OPEN:
        ui->action_TB_Open->trigger();
        break;
    case SAVE:
        ui->action_Save->trigger();
        break;
    default:
        return;

    }
}
