/*!
 *  \brief     implementing main class of the application.
 *  \details   This class is used to show and edit labels of ann image.
 *  \author    Behzad Tabibian
 *  \version   0.1
 *  \date      Sep. 24, 2011
 *  \copyright GNU Public License.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAX_NUMBER_OF_ZOOMS 3
#define MOUSE_WHEEL_SENSITIVITY 1
#define ZOOM_SCALE 1.3

#include <QMainWindow>
#include <QGraphicsScene>
#include <QModelIndex>
#include <QListWidget>
#include <QFileSystemModel>
#include <QDockWidget>
#include <QTreeView>
#include <QPushButton>
#include "img_label.h"
#include <QSettings>
#include <act_predict.h>
namespace Ui {

    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum WindowMode { ADD_LABEL, EDIT_LABEL, NONE};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    WindowMode mod;
    Ui::MainWindow *ui;
    QString current_img;
    QGraphicsScene* scene;
    QPixmap pixmap;
    img_label* current_lbl;
    QVector<img_label*> labels;
    void displayImage(QString file);
    void set_current_label(img_label* lbl);
    img_label* get_current_label();
    void drawVertex(QPoint point,bool mapped=false);
    void CompleteEdit();
    void updateState();
    void removeImage();
    void updateStatus(QString msg);
    void updateLabel(const QString& name);
    void loadImageData(QString file);
    bool readXmlFile(QString& file);
    bool writeXmlFile(QString& file);
    int numberOfZooms;
    int wheelDeltaInDegrees;
    QDockWidget *dirDock;
    QDockWidget *thumbnailDock;
    QFileSystemModel *dirModel;
    QTreeView *treeView;
    QListWidget *thumbnailList;
    QString directoryPath;
    bool changed;
    void closeEvent(QCloseEvent *event);
    bool checkSave();
    void storeWindowLayout();
    void restoreWindowLayout();
    QByteArray windowLayout;
    bool init;
    //inference engine
    QIcon img_add;
    QIcon img_edit;
    QIcon img_save;
    QIcon img_open;
    act_predict predictor;
    ACTIONS current_action;
    ACTIONS predicted_action;
    QDockWidget *nextOptDock;
    QPushButton *nextOptWidget;
    void changeState(ACTIONS new_state);
    void selectAction(ACTIONS action);
public slots:
    void on_action_TB_Open_clicked();
    void on_action_Add_Object_clicked();
    void on_action_Save_clicked();
    void on_action_Delete_clicked();
    void on_action_ZoomIn_clicked();
    void on_action_ZoomOut_clicked();
    void on_action_ZoomReset_Clicked();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void wheelEvent(QWheelEvent *event);
    void hideEvent(QHideEvent *);
    void showEvent(QShowEvent *);
private slots:
    void on_actionShow_Suggestions_toggled(bool );
    void dirDockVisible(bool);
    void showDirectoriesToggled(bool);
    void thumbnailDockVisible(bool);
    void showThumbnailsToggled(bool);
    void thumbnailListItemDoubleClicked(QListWidgetItem *);
    void treeViewDirectoryCollapsed(QModelIndex index);
    void treeViewDirectoryExpanded(QModelIndex index);
    void treeViewDirectoryChanged(QModelIndex index);
    void treeViewDoubleClicked(QModelIndex index);
    void on_actionE_xit_triggered();
    //inference engine slots
    void nextOptDockVisible(bool);
    void nextOptWidgetPushed();
};

#endif // MAINWINDOW_H
