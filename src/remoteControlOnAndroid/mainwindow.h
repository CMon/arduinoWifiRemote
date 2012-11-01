#pragma once

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    void showExpanded();

private slots:
    void enableGui();
    void on_pbSetSelection_clicked();
    void onColorChange();

    void on_pbRed_clicked();
    void on_pbGreen_clicked();
    void on_pbBlue_clicked();
    void on_pbViolet_clicked();
    void on_pbYellow_clicked();
    void on_pbOff_clicked();

private:
    void setColor(const QColor & color);
    void disableGui(bool disable);

private:
    QNetworkAccessManager accessManager_;
    QColor color_;
    QString baseUrl_;

private:
    Ui::MainWindow *ui;
};
