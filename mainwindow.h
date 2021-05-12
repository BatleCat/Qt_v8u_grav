//---------------------------------------------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//---------------------------------------------------------------------------
#include <QMainWindow>
#include <QString>
#include <QSettings>
//---------------------------------------------------------------------------
namespace Ui {
class MainWindow;
}
//---------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSettings* app_settings;
    QString filePath;
    void load_setings(void);
    void save_setings(void);

signals:
    void send_info(const QString &qs_info);

private slots:
    void fileOpen(void);

};
//---------------------------------------------------------------------------
#endif // MAINWINDOW_H
//---------------------------------------------------------------------------
