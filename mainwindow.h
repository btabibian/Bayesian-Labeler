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
#include "img_label.h"
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
    void drawVertex(QPoint point);
    void CompleteEdit();
    void updateState();
    void updateStatus(QString msg);
    int numberOfZooms;
    int wheelDeltaInDegrees;
public slots:
    void on_action_TB_Open_clicked();
    void on_action_Add_Object_clicked();
    void on_action_Save_clicked();
    void on_action_Delete_clicked();
    void on_action_ZoomIn_clicked();
    void on_action_ZoomOut_clicked();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void wheelEvent(QWheelEvent *event);
private slots:
    void on_actionE_xit_triggered();
};

#endif // MAINWINDOW_H
