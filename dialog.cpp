#include "dialog.h"
#include "ui_dialog.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
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
    ui->comboBox->addItems(res);
    file.close();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{
    emit sendData(ui->comboBox->currentText());
}

