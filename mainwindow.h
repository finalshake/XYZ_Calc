#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int find_point_name(QString);
    void save_to_csv(void);
    void change_csv(int);
    void get_point();

public:
    Dialog *dialog;

private slots:
    void on_convert_clicked();

    void on_save_point_clicked();

    void on_pushButton_clicked();

    void recv_data(QString);

    void on_tabWidget_tabBarClicked(int index);

    void on_p1_name_activated(const QString &arg1);

    void on_p2_name_activated(const QString &arg1);

    void on_calc_distance_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
