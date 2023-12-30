#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextStream>
#include <QFileDialog>
#include <QCoreApplication>
#include <QDebug>
#include <QSpinBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QtSql>
#include <QSqlError>

bool deleteTask(const QString& name)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString pathToDatabase = "d:/labs/kursvizprog/build-kursovaya-desktop_qt_5_10_0_mingw_32bit-debug/task_scheduler.db";
    db.setDatabaseName(pathToDatabase);

    if (db.open())
    {
        qDebug() << "database open";
    } else
    {
        qDebug() << "error" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM tasks WHERE name = :name");
    query.bindValue(":name", name);

    if (query.exec())
    {
        qDebug() << "delete success";
    }
    else
    {
        qDebug() << "failed to delete task";
        qDebug() << query.lastError().text();
    }

    db.close();
    return true;
}

bool addTask(const QString& name, const QString& description, int priority, const QDate& dueDate)
{
// проверка на ввод некорректных значений
    if (name.isEmpty() || description.isEmpty() || priority < 0)
    {
        qDebug() << "invalid input values";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString pathToDatabase = "d:/labs/kursvizprog/build-kursovaya-desktop_qt_5_10_0_mingw_32bit-debug/task_scheduler.db";
    db.setDatabaseName(pathToDatabase);

    if (db.open())
    {
    qDebug() << "database open";
    }
    else
    {
        qDebug() << "error" << db.lastError().text();
        return false;
    }

// создание таблицы для хранения задач
    QSqlQuery query;
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS tasks"
    "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT NOT NULL,"
    "description TEXT,"
    "priority INTEGER,"
    "due_date DATE)";
    if (!query.exec(createTableQuery))
    {
        qDebug() << "failed to create tasks table";
        qDebug() << query.lastError().text();
        return false;
    }

    query.prepare("INSERT INTO tasks (name, description, priority, due_date) VALUES (:name, :description, :priority, :dueDate)");
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":priority", priority);
    query.bindValue(":dueDate", dueDate);

    if (!query.exec())
    {
        qDebug() << "failed to add task";
        qDebug() << query.lastError().text();
    return false;
    }

    db.close();
    return true;
}

// Функция для загрузки задач из базы данных
QList<QMap<QString, QVariant>> loadTasks()
{
    QList<QMap<QString, QVariant>> tasks;

// Подключение к базе данных
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString pathToDatabase = "d:/labs/kursvizprog/build-kursovaya-desktop_qt_5_10_0_mingw_32bit-debug/task_scheduler.db";
    db.setDatabaseName(pathToDatabase);

    if (!db.open())
    {
        qDebug() << "Error" << db.lastError().text();
        return tasks;
    }

// Загрузка данных из таблицы tasks
    QSqlQuery query("SELECT * FROM tasks");
    while (query.next())
    {
        QMap<QString, QVariant> task;
        task["id"] = query.value("id");
        task["name"] = query.value("name");
        task["description"] = query.value("description");
        task["priority"] = query.value("priority");
        task["due_date"] = query.value("due_date");
        tasks.append(task);
    }

    db.close();
    return tasks;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

// создание главного окна
    QMainWindow mainWindow;
    QMainWindow secondWindow;

// создание виджетов для ввода данных
    QLineEdit* nameLineEdit = new QLineEdit;
    QLineEdit* deleteLineEdit = new QLineEdit;
    QTextEdit* descriptionTextEdit = new QTextEdit;
    QSpinBox* prioritySpinBox = new QSpinBox;
    QDateEdit* dueDateEdit = new QDateEdit;

// создание кнопок для добавления и удаления задачи
    QPushButton* addButton = new QPushButton("Add Task");
    QPushButton* deleteButton = new QPushButton("Delete Task");
    QPushButton* loadButton = new QPushButton("Load Task");

// создание таблицы для отображения задач
    QTableWidget* taskTableWidget = new QTableWidget;
    taskTableWidget->setColumnCount(5);
    taskTableWidget->setHorizontalHeaderLabels(QStringList() << "ID" << "Name" << "Description" << "Priority" << "Due Date");

    QPushButton* loadImageButton = new QPushButton("Load Image");

    QWidget* centralWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->addWidget(new QLabel("Name:"));
    layout->addWidget(nameLineEdit);
    layout->addWidget(new QLabel("Description:"));
    layout->addWidget(descriptionTextEdit);
    layout->addWidget(new QLabel("Priority:"));
    layout->addWidget(prioritySpinBox);
    layout->addWidget(new QLabel("Due Date:"));
    layout->addWidget(dueDateEdit);
    layout->addWidget(addButton);
    layout->addWidget(loadButton);
    layout->addWidget(deleteButton);
    layout->addWidget(new QLabel("Task Name to Delete:"));
    layout->addWidget(deleteLineEdit);
    layout->addWidget(loadImageButton);
    layout->addWidget(new QLabel("Tasks:"));
    layout->addWidget(taskTableWidget);
    layout->addStretch();

    QLabel* label = new QLabel(&secondWindow);
    label->setText("Your task has added!!!");

    QString filePath;

QObject::connect(loadImageButton, &QPushButton::clicked, [&]()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec())
    {
        QStringList fileNames = dialog.selectedFiles();
        if (!fileNames.isEmpty())
        {
            filePath = fileNames.first();
            QPixmap pixmap(filePath);
            QLabel* imageLabel = new QLabel;
            imageLabel->setPixmap(pixmap);
            layout->addWidget(imageLabel);
        }
    }
});

QObject::connect(loadButton, &QPushButton::clicked, [&]()
{
    QList<QMap<QString, QVariant>> tasks = loadTasks();
    for (const auto& task : tasks)
    {
        int row = taskTableWidget->rowCount();
        taskTableWidget->insertRow(row);
        taskTableWidget->setItem(row, 0, new QTableWidgetItem(task["id"].toString()));
        taskTableWidget->setItem(row, 1, new QTableWidgetItem(task["name"].toString()));
        taskTableWidget->setItem(row, 2, new QTableWidgetItem(task["description"].toString()));
        taskTableWidget->setItem(row, 3, new QTableWidgetItem(task["priority"].toString()));
        taskTableWidget->setItem(row, 4, new QTableWidgetItem(task["due_date"].toString()));
    }
});

QObject::connect(addButton, &QPushButton::clicked, [&]()
{
    QString name = nameLineEdit->text();
    QString description = descriptionTextEdit->toPlainText();
    int priority = prioritySpinBox->value();
    QDate dueDate = dueDateEdit->date();

    if (addTask(name, description, priority, dueDate))
    {
    int row = taskTableWidget->rowCount();
    taskTableWidget->insertRow(row);
    taskTableWidget->setItem(row, 0, new QTableWidgetItem());
    taskTableWidget->setItem(row, 1, new QTableWidgetItem(name));
    taskTableWidget->setItem(row, 2, new QTableWidgetItem(description));
    taskTableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(priority)));
    taskTableWidget->setItem(row, 4, new QTableWidgetItem(dueDate.toString(Qt::ISODate)));

    nameLineEdit->clear();
    descriptionTextEdit->clear();
    prioritySpinBox->setValue(0);
    dueDateEdit->setDate(QDate::currentDate());

    secondWindow.show();
    }
});

QObject::connect(deleteButton, &QPushButton::clicked, [&]()
{
    QString taskId = deleteLineEdit->text();
    if (!taskId.isEmpty())
    {
        bool success = deleteTask(taskId);
        if (success)
        {
        }
    }
});

mainWindow.setCentralWidget(centralWidget);
mainWindow.show();

return app.exec();
}
