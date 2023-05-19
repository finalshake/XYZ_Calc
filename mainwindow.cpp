#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "locpoint.h"
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

LocPoint point = LocPoint("tmp", 0, 0);
LocPoint p1("p1", 0 ,0), p2("p2", 0, 0);
//int point_line = 0;

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::find_point_name(QString name)
{
    int point_line = -1;
    QFile file("point.csv");
    QString line;
    if(!file.open(QIODevice::ReadOnly))
    {
        //file.close();
        return -1;
    }
    QTextStream stream_text(&file);
    while(!stream_text.atEnd())
    {
        point_line++;
        line = stream_text.readLine();
        if(line.split(",").at(0) == name)
        {
            file.close();
            return point_line;
        }
    }
    file.close();
    return -1;
}

void MainWindow::save_to_csv()
{
    QFile file("point.csv");
    QStringList line;
    line << point.name << "," << ui->num_text->toPlainText() << "," << ui->x_text->toPlainText()
         << "," << ui->y_text->toPlainText() << "," << ui->height_text->toPlainText()
         << "," << QString("%1").arg(point.longtitude, 0, 'f', 6) << "," << QString("%1").arg(point.latitude, 0, 'f', 6)
         << "\n";
    if(!file.open(QIODevice::Append))
    {
        QMessageBox::warning(this, "Error", "打开csv文件失败");
        return;
    }
    else
    {
        qDebug() << line;
        for(int i = 0; i < line.size(); i++)
        {
            file.write(line[i].toStdString().c_str());
        }
        file.close();
    }
}

void MainWindow::change_csv(int target_line)
{
    QFile file("point.csv");
    QStringList all_lines;
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "读取时打开csv文件失败");
        return;
    }
    QTextStream stream_text(&file);
    int cur_line = 0;
    while(!stream_text.atEnd())
    {
        if(cur_line == target_line)
        {
            all_lines <<  point.name << "," << ui->num_text->toPlainText() << "," << ui->x_text->toPlainText()
                << "," << ui->y_text->toPlainText() << "," << ui->height_text->toPlainText()
                << "," << QString("%1").arg(point.longtitude, 0, 'f', 6) << "," << QString("%1").arg(point.latitude, 0, 'f', 6)
                << "\n" ;
            stream_text.readLine();
        }
        else
        {
            all_lines << stream_text.readLine() << "\n";
        }
        cur_line++;
    }
    file.close();
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Error", "写入时打开csv文件失败");
        return;
    }
    for(int i = 0; i < all_lines.size(); i++)
    {
        file.write(all_lines[i].toStdString().c_str());
    }
    file.close();
}

void MainWindow::on_convert_clicked()
{
    if(ui->long_text->document()->isEmpty() || ui->lat_text->document()->isEmpty())
    {
        QMessageBox::information(this, "Error", "经度和纬度不能为空");
        return;
    }
    if(!ui->is_dms->isChecked())
    {
        point.longtitude =  ui->long_text->toPlainText().toDouble();
        point.latitude = ui->lat_text->toPlainText().toDouble();
    }
    else
    {
        QStringList long_info = ui->long_text->toPlainText().split(",");
        QStringList lat_info = ui->lat_text->toPlainText().split(",");
        point.longtitude = long_info.at(0).toDouble() + long_info.at(1).toDouble() / 60 + long_info.at(2).toDouble() / 3600;
        point.latitude = lat_info.at(0).toDouble() + lat_info.at(1).toDouble() / 60 + lat_info.at(2).toDouble() / 3600;
        //qDebug() << point.longtitude;
    }
    point.height = ui->height_text->toPlainText().toDouble();
    point.convert();
    ui->num_text->setText(QString::number(point.num));
    ui->x_text->setText(QString("%1").arg(point.x_val, 0, 'f', 3));
    ui->y_text->setText(QString("%1").arg(point.y_val, 0, 'f', 3));
}


void MainWindow::on_save_point_clicked()
{
    if(!ui->long_text->document()->isEmpty() && !ui->lat_text->document()->isEmpty())
        MainWindow::on_convert_clicked();

    if(ui->height_text->document()->isEmpty() || ui->x_text->document()->isEmpty() || ui->y_text->document()->isEmpty() || ui->num_text->document()->isEmpty())
    {
        QMessageBox::information(this, "Error", "点的X,Y和高程信息不全，请补全");
        return;
    }
    if(ui->point_name->text().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "请给这个点一个名称,否则就自动以tmp命名");
        ui->point_name->setText("tmp");
    }
    QString point_name = ui->point_name->text();
    int found_line = MainWindow::find_point_name(point_name);
    qDebug() << "point_name:" << point_name << "  " << found_line;
    if(found_line != -1)                       //same point name exists, overwrite or not?
    {
        int ans = QMessageBox::warning(this, "Warning", "同名的点已经存在，是否要覆盖掉之前的点？", QMessageBox::Yes, QMessageBox::No);
        if(ans == QMessageBox::Yes)
        {
            point.name = point_name;
            MainWindow::change_csv(found_line);
        }
        else if(ans == QMessageBox::No)
        {
            return;
        }
    }
    else                                       //new point, just save it
    {
        point.name = point_name;
        MainWindow::save_to_csv();
    }
}

void MainWindow::recv_data(QString data)
{
    //qDebug() << data;
    point.name = data;
}
void MainWindow::get_point()
{
    dialog = new Dialog(this);
    connect(dialog, SIGNAL(sendData(QString)), this, SLOT(recv_data(QString)));
    dialog->exec();
    QFile file("point.csv");
    QString line;
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "打开文件失败");
        return;
    }
    QTextStream stream_text(&file);
    while(!stream_text.atEnd())
    {
        line = stream_text.readLine();
        if(line.split(",").at(0) != point.name)
            continue;
        else
        {
            QStringList res = line.split(",");
            ui->point_name->setText(res.at(0));
            ui->num_text->setText(res.at(1));
            ui->x_text->setText(res.at(2));
            ui->y_text->setText(res.at(3));
            ui->height_text->setText(res.at(4));
            ui->long_text->setText(res.at(5));
            ui->lat_text->setText(res.at(6));
            break;
        }
    }
    file.close();
}

void MainWindow::on_pushButton_clicked()
{
    get_point();
}



void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if(index == 0)
        return;
    QFile file("point.csv");
    QString line;
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "文件打开失败，文件可能不存在");
        return;
    }
    QTextStream st(&file);
    QStringList res;
    while(!st.atEnd())
    {
        line = st.readLine();
        res.append(line.split(",").at(0));
    }
    ui->p1_name->clear();
    ui->p1_name->addItems(res);
    ui->p2_name->clear();
    ui->p2_name->addItems(res);
    file.close();

}


void MainWindow::on_p1_name_activated(const QString &arg1)
{
    QFile file("point.csv");
    QString line;
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "打开文件失败");
        return;
    }
    QTextStream stream_text(&file);
    while(!stream_text.atEnd())
    {
        line = stream_text.readLine();
        if(line.split(",").at(0) != arg1)
            continue;
        else
        {
            p1.name = arg1;
            QStringList res = line.split(",");
            QString y = res.at(1) + res.at(3);
            ui->p1_x->setText(res.at(2));
            ui->p1_y->setText(y);
            ui->p1_h->setText(res.at(4));
            p1.x_val = res.at(2).toDouble();
            p1.y_val = y.toDouble();
            p1.height = res.at(4).toDouble();
            p1.longtitude = res.at(5).toDouble();
            p1.latitude = res.at(6).toDouble();
            break;
        }
    }
    file.close();
}


void MainWindow::on_p2_name_activated(const QString &arg1)
{
    QFile file("point.csv");
    QString line;
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Error", "打开文件失败");
        return;
    }
    QTextStream stream_text(&file);
    while(!stream_text.atEnd())
    {
        line = stream_text.readLine();
        if(line.split(",").at(0) != arg1)
            continue;
        else
        {
            p2.name = arg1;
            QStringList res = line.split(",");
            QString y = res.at(1) + res.at(3);
            ui->p2_x->setText(res.at(2));
            ui->p2_y->setText(y);
            ui->p2_h->setText(res.at(4));
            p2.x_val = res.at(2).toDouble();
            p2.y_val = y.toDouble();
            p2.height = res.at(4).toDouble();
            p2.longtitude = res.at(5).toDouble();
            p2.latitude = res.at(6).toDouble();
            break;
        }
    }
    file.close();
}


void MainWindow::on_calc_distance_clicked()
{
    if(ui->p1_h->text().isEmpty() || ui->p1_x->text().isEmpty() || ui->p1_y->text().isEmpty() ||
            ui->p2_h->text().isEmpty() || ui->p2_x->text().isEmpty() || ui->p2_y->text().isEmpty())
    {
        QMessageBox::information(this, "Warning", "请先选择要计算的点，或者手动输入点的直角座标");
        return;
    }
    double p1_x = ui->p1_x->text().toDouble();
    double p1_y = ui->p1_y->text().toDouble();
    double p1_h = ui->p1_h->text().toDouble();
    double p2_x = ui->p2_x->text().toDouble();
    double p2_y = ui->p2_y->text().toDouble();
    double p2_h = ui->p2_h->text().toDouble();
    double distance = sqrt(pow(p1_x-p2_x, 2) + pow(p1_y-p2_y, 2) + pow(p1_h-p2_h, 2));
    ui->distance->setText(QString("%1").arg(distance, 0, 'f', 2));

    //LocPoint p1(ui->p1_name->currentText(), p1_x, p1_y, p1_h), p2(ui->p2_name->currentText(), p2_x, p2_y, p2_h); // problem
    double az, el, azn, dis;
    double compass_dire = LocPoint::direction_calc(p1.latitude, p1.longtitude, p2.latitude, p2.longtitude);
    double north_dire = LocPoint::LBH_to_polar(p1, p2, az, el, azn, dis);

    qDebug() << "distance from xyz = " << distance << "  from polar = " << dis
             << "\n" << "compass direction = " << compass_dire << "\n"
             << "north direction az = " << az << " azn = " << azn << "\n"
             << "LBH_to_polar func return :" << north_dire << "\n"
             << "ele = " << el;
    ui->angle->setText(QString::number(az, 'f', 3));
}


void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox::information(this, "XYZ v1.1.0", "XYZ v1.1.0\nCopyright © 2021-2022 Shake (Weng Kaiqiang).\n All Rights Reserved.");
}

