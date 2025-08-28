#include <QApplication>
#include "loginwindow.h"
#include "mainwindow.h"
#include <memory>

#include <QMetaType>

int main(int argc, char *argv[])
{
    // QApplication manages GUI application-wide resources
    QApplication app(argc, argv);

    // Set a flag to ensure the app doesn't quit when the login window closes
    app.setQuitOnLastWindowClosed(false);

    // Create the login window
    LoginWindow loginWindow;

    // Create a pointer for the main window, which will be created upon successful login
    std::unique_ptr<MainWindow> mainWindow;

    // Connect the login window's success signal to a lambda function
    QObject::connect(&loginWindow, &LoginWindow::loginSuccess, [&](User* user) {
        // When login is successful, create and show the main window
        mainWindow = std::make_unique<MainWindow>(user);
        mainWindow->show();
    });

    loginWindow.show();

    return app.exec();
}