#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ProcessJson->setEnabled(false);

    inigetForecastURL("https://api.weather.gov/points/39.7456,-97.0892");
    connect(ui->ProcessJson, SIGNAL(clicked()), this, SLOT(show_json()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onlistchanged(QListWidgetItem*)));

    // Set the URL for the initial download
    //downloader->setUrl("https://api.weather.gov/gridpoints/TOP/32,81/forecast/hourly");
    //downloader->execute();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enable_button()
{
    ui->ProcessJson->setEnabled(true);
}

void MainWindow::show_json()
{
    QJsonObject jsonobject = downloader->loadedJson.object();
    qDebug() << jsonobject;

    // Extracting the forecast URL
    qDebug() << "Level 1: " << jsonobject.value("properties");
    qDebug() << "Level 2: " << jsonobject.value("properties").toObject().value("periods");

    forecastArray = jsonobject.value("properties").toObject().value("periods").toArray();

    for (int i=0; i<forecastArray.size(); i++)
    {
        qDebug()<< "Forecast Array (" << i << "):  " <<forecastArray[i];
    }

    QString forecastUrl = jsonobject.value("properties").toObject().value("forecastHourly").toString();
    qDebug() << "Forecast URL: " << forecastUrl;

    showDatainList();
    //connect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(process_forecast_data()));
    /* If forecast URL is available, proceed to download and process the forecast data
    if (!forecastUrl.isEmpty()) {
        // Disconnect the previous signal-slot connection and connect to process forecast data
        disconnect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(enable_button()));


        downloader->setUrl(forecastUrl);
        downloader->execute();
    }*/
}

void MainWindow::showDatainList()
{
    for (int i=0; i<forecastArray.size(); i++)
    {
        ui->listWidget_contents->addItem(forecastArray[i].toObject().value("startTime").toString());
    }
}

void MainWindow::process_forecast_data()
{
    QJsonObject forecastJson = downloader->loadedJson.object();
    qDebug() << forecastJson;

    // Assuming forecastJson contains the hourly forecast data
    QJsonValue forecastHourly = forecastJson.value("properties").toObject().value("period");

    if (forecastHourly.isArray()) {
        QJsonArray hourlyForecastArray = forecastHourly.toArray();

        ui->listWidget->clear();
        int j = 0;
        foreach (const QJsonValue &value, hourlyForecastArray) {
            QJsonObject forecastObj = value.toObject();
            QString startTime = forecastObj.value("startTime").toString();
            ui->listWidget->insertItem(j, startTime);
            jsonValues[startTime] = value;
            j++;
        }
    } else if (forecastHourly.isObject()) {
        QJsonObject hourlyForecastObj = forecastHourly.toObject();

        ui->listWidget->clear();
        int j = 0;
        foreach (const QString &key, hourlyForecastObj.keys()) {
            QJsonValue value = hourlyForecastObj.value(key);
            ui->listWidget->insertItem(j, key);
            jsonValues[key] = value;
            j++;
        }
    } else {
        qDebug() << "Unexpected data format for forecast hourly";
    }
}

void MainWindow::onlistchanged(QListWidgetItem *item)
{
    ui->listWidget_contents->clear();
    QJsonValue selectedValue = jsonValues[item->text()];

    if (selectedValue.isString()) {
        ui->lineEdit->setText(selectedValue.toString());
    } else if (selectedValue.isObject()) {
        QJsonObject jsonObject = selectedValue.toObject();
        foreach (const QString &key, jsonObject.keys()) {
            QString valueStr = jsonObject.value(key).toString();
            ui->listWidget_contents->addItem(key + ": " + valueStr);
        }
    } else if (selectedValue.isArray()) {
        QJsonArray jsonArray = selectedValue.toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            QJsonObject jsonObject = jsonArray[i].toObject();
            foreach (const QString &key, jsonObject.keys()) {
                QString valueStr = jsonObject.value(key).toString();
                ui->listWidget_contents->addItem(key + ": " + valueStr);
            }
        }
    } else {
        ui->lineEdit->setText("Unsupported JSON value type");
    }
}

void MainWindow::inigetForecastURL(const QString &url)
{
    downloader = new Downloader(url);
    connect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(getForecastURL()));
}

void MainWindow::getForecastURL()
{
    forecastURL =  downloader->loadedJson.object().value("properties").toObject().value("forecastHourly").toString();
    delete downloader;
    downloader = new Downloader(forecastURL, this);
    connect(downloader, SIGNAL(download_finished_sgnl()), this, SLOT(getWeatherPrediction()));
}

//void MainWindow::getWeatherPrediction()
//{

   // if (!downloader || downloader->loadedJson.isNull()) {
   //     qDebug() << "No valid JSON data loaded.";
    //    return;
   // }
    //QJsonObject jsonObject = downloader->loadedJson.object();

    //qDebug()<<jsonObject;

    //QJsonObject propObj = jsonObject.value("properties").toObject();

    //qDebug() << propObj;

    //QJsonValue periodsObj = propObj.value("periods");

    //qDebug() << periodsObj;

   // QJsonArray periodsArray = periodsObj.toArray();

    //qDebug() << periodsArray;

    //ui->listWidget_contents->clear();

   // for (int i=0; i<periodsArray.count(); i++)
   // {
    //    qDebug()<<periodsArray[i];
    //}

    //for (const QJsonValue &value : periodsArray) {
        //QJsonObject forecastObj = value.toObject();

        // Create a WeatherDataPoint and populate it with data
        //WeatherDataPoint dataPoint;
        //dataPoint.startTime = forecastObj.value("startTime").toString();
        //dataPoint.temperature = forecastObj.value("temperature").toString();
        //dataPoint.weather = forecastObj.value("weather").toString();
        //dataPoint.probabilityOfPrecipitation = forecastObj.value("probabilityOfPrecipitation").toDouble();
        //dataPoint.relativeHumidity = forecastObj.value("relativeHumidity").toString();

        // Append the populated dataPoint to the QVector
       // weatherData.append(dataPoint);

        // Add the extracted data to the list widget for display
        //QString itemText = QString("Start Time: %1, Temperature: %2, Weather: %3, Probability of Precipitation: %4, Relative Humidity: %5")
        //                       .arg(dataPoint.startTime)
        //                       .arg(dataPoint.temperature)
        //                       .arg(dataPoint.weather)
        //                       .arg(dataPoint.probabilityOfPrecipitation)
       //                        .arg(dataPoint.relativeHumidity);
       // ui->listWidget_contents->addItem(itemText);
   // }
   // }

void MainWindow::getWeatherPrediction()
{
    // Check if downloader is valid and has loaded JSON data
    if (!downloader || downloader->loadedJson.isNull()) {
        qDebug() << "No valid JSON data loaded.";
        return;
    }




    // Clear the previous data from the list widget

    QJsonObject jsonObject = downloader->loadedJson.object();

    qDebug()<<jsonObject;

    QJsonObject propObj = jsonObject.value("properties").toObject();

    qDebug() << propObj;

    QJsonValue periodsObj = propObj.value("periods");

    qDebug() << periodsObj;

     QJsonArray periodsArray = periodsObj.toArray();

    qDebug() << periodsArray;

    ui->listWidget_contents->clear();
    // QVector to store the weather predictions
    QVector<WeatherDataPoint> weatherData;

    // Process each forecast period
    for (const QJsonValue &value : periodsArray) {
        QJsonObject forecastObj = value.toObject();

        // Create a WeatherDataPoint and populate it with data
        WeatherDataPoint dataPoint;
        dataPoint.startTime = forecastObj.value("startTime").toString();
        dataPoint.endTime = forecastObj.value("endTime").toString();
        dataPoint.temperature = forecastObj.value("temperature").toDouble();
        dataPoint.weather = forecastObj.value("weather").toString();
        dataPoint.probabilityOfPrecipitation = forecastObj.value("probabilityOfPrecipitation").toDouble();
        dataPoint.relativeHumidity = forecastObj.value("relativeHumidity").toString();

        // Append the populated dataPoint to the QVector
        weatherData.append(dataPoint);

        // Add the extracted data to the list widget for display
        QString itemText = QString("Start Time: %1, End Time: %2, Temperature: %3, Weather: %4, Probability of Precipitation: %5, Relative Humidity: %6")
                               .arg(dataPoint.startTime)
                               .arg(dataPoint.endTime)
                               .arg(dataPoint.temperature)
                               .arg(dataPoint.weather)
                               .arg(dataPoint.probabilityOfPrecipitation)
                               .arg(dataPoint.relativeHumidity);
        ui->listWidget_contents->addItem(itemText);
    }

    // Optionally, you can store the weatherData QVector for later use
    //this->weatherData = weatherData;
}
