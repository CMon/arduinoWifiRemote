#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QtNetwork>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      baseUrl_("http://192.168.43.177/"),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&accessManager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(enableGui()));

    connect(ui->hsRed,   SIGNAL(valueChanged(int)), this, SLOT(onColorChange()));
    connect(ui->hsGreen, SIGNAL(valueChanged(int)), this, SLOT(onColorChange()));
    connect(ui->hsBlue,  SIGNAL(valueChanged(int)), this, SLOT(onColorChange()));
    onColorChange();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showExpanded()
{
#if defined(Q_WS_SIMULATOR)
    showFullScreen();
#else
    show();
#endif
}

void MainWindow::enableGui()
{
    disableGui(false);
}

void MainWindow::on_pbSetSelection_clicked()
{
    const int red   = ui->hsRed->value();
    const int green = ui->hsGreen->value();
    const int blue  = ui->hsBlue->value();

    setColor(QColor(red, green, blue));
}

void MainWindow::onColorChange()
{
    const int red   = ui->hsRed->value();
    const int green = ui->hsGreen->value();
    const int blue  = ui->hsBlue->value();

    ui->lblRedValue->setText(QString::number(red));
    ui->lblGreenValue->setText(QString::number(green));
    ui->lblBlueValue->setText(QString::number(blue));

    color_ = QColor(red, green, blue);

    QPalette palette = ui->lblPreview->palette();
    palette.setColor(ui->lblPreview->backgroundRole(), color_);

    ui->lblPreview->setPalette(palette);
    ui->lblPreview->repaint();
}

void MainWindow::disableGui(bool disable)
{
    ui->hsRed->setDisabled(disable);
    ui->hsGreen->setDisabled(disable);
    ui->hsBlue->setDisabled(disable);

    ui->pbRed->setDisabled(disable);
    ui->pbGreen->setDisabled(disable);
    ui->pbBlue->setDisabled(disable);
    ui->pbViolet->setDisabled(disable);
    ui->pbYellow->setDisabled(disable);
    ui->pbOff->setDisabled(disable);

    ui->pbSetSelection->setDisabled(disable);
}

void MainWindow::on_pbOff_clicked()
{
    setColor(QColor(0, 0, 0));
}

void MainWindow::setColor(const QColor &color)
{
    QString request;
    request.sprintf("%sa%03d%03d%03d", baseUrl_.toLatin1().data(), color.red(), color.green(), color.blue());
    accessManager_.get(QNetworkRequest(QUrl(request)));

    ui->hsRed->setValue(color.red());
    ui->hsGreen->setValue(color.green());
    ui->hsBlue->setValue(color.blue());

    disableGui(true);
}

void MainWindow::on_pbYellow_clicked()
{
    setColor(QColor(170, 100, 0));
}

void MainWindow::on_pbViolet_clicked()
{
    setColor(QColor(100, 0, 170));
}

void MainWindow::on_pbBlue_clicked()
{
    setColor(QColor(0, 0, 170));
}

void MainWindow::on_pbGreen_clicked()
{
    setColor(QColor(0, 170, 0));
}

void MainWindow::on_pbRed_clicked()
{
    setColor(QColor(170, 0, 0));
}
