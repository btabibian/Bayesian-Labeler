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

    // add the docks to the main window
    dirDock = new QDockWidget(tr("Directories"));
    dirDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, dirDock);
    thumbnailDock = new QDockWidget(tr("Thumbnail Viewer"));
    thumbnailDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
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
    connect(thumbnailList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(thumbnailListItemDoubleClicked(QListWidgetItem *)));

    // set the sizes of the docks
    dirDock->setMinimumSize(175, 200);
    thumbnailDock->setMinimumSize(175, 200);

    // add the widgets to their docks
    dirDock->setWidget(treeView);
    thumbnailDock->setWidget(thumbnailList);

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
    scene->clear();
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
