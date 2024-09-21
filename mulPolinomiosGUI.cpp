#include "mulPolinomiosGUI.h"
#include <vector>
#include <iostream>
#include <complex>
#include <cmath>
#include <QMessageBox>
#include <QLabel>
#include <QTimer>
#include <QElapsedTimer>
#include <QMovie>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>

using namespace std;

const long double PI = 3.14159265358979323846L;

mulPolinomiosGUI::mulPolinomiosGUI(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //QLineSeries* series = new QLineSeries();
    //QChart* chart = new QChart();
    //QChartView* chartView = new QChartView(chart);

    // Conectar los botones a las acciones
    connect(ui.btnAgregarPolinomio, &QPushButton::clicked, this, &mulPolinomiosGUI::agregarPolinomio);
    connect(ui.btnEliminarPolinomio, &QPushButton::clicked, this, &mulPolinomiosGUI::eliminarPolinomio);
    connect(ui.btnAgregarCoeficiente, &QPushButton::clicked, this, &mulPolinomiosGUI::agregarCoeficiente);
    connect(ui.btnMultiplicar, &QPushButton::clicked, this, &mulPolinomiosGUI::multiplicarPolinomios);
    connect(ui.btnReset, &QPushButton::clicked, this, &mulPolinomiosGUI::reset);
    //connect(ui.btnGraficarTiempo, &QPushButton::clicked, this, &fft_dft_gui::graficarTiempo);
}

mulPolinomiosGUI::~mulPolinomiosGUI()
{}

void mulPolinomiosGUI::agregarPolinomio() {
    // Añadir una nueva fila (polinomio) a la tabla
    int currentRowCount = ui.tableWidget->rowCount();
    ui.tableWidget->setRowCount(currentRowCount + 1);

    // Actualizar el encabezado vertical con "Polinomio X"
    for (int i = 0; i < ui.tableWidget->rowCount(); ++i) {
        QString headerLabel = "Polinomio " + QString::number(i + 1);
        QTableWidgetItem* headerItem = new QTableWidgetItem(headerLabel);
        headerItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setVerticalHeaderItem(i, headerItem);
    }

    // Alinear el contenido de las celdas en la nueva fila al centro
    for (int j = 0; j < ui.tableWidget->columnCount(); ++j) {
        QTableWidgetItem* newItem = new QTableWidgetItem("");  // Puedes inicializar con algún valor si es necesario
        newItem->setTextAlignment(Qt::AlignCenter);  // Alinear el texto al centro
        ui.tableWidget->setItem(currentRowCount, j, newItem);
    }

    ui.tableWidget->resizeRowsToContents();
    ui.tableWidget->resizeColumnsToContents();
}

void mulPolinomiosGUI::eliminarPolinomio() {
    // Obtener la fila seleccionada
    QList<QTableWidgetSelectionRange> selectedRanges = ui.tableWidget->selectedRanges();
    if (selectedRanges.isEmpty()) {
        // Crear el mensaje de advertencia
        QMessageBox msgBox;
        msgBox.setWindowTitle("Eliminar Polinomio");
        msgBox.setText("Por favor, seleccione un polinomio para eliminar.");

        // Usar un ícono de advertencia predefinido
        msgBox.setIcon(QMessageBox::Warning);

        msgBox.exec();
        return;
    }

    // Suponiendo que solo se puede seleccionar una fila a la vez
    int selectedRow = selectedRanges.first().topRow();

    // Eliminar la fila seleccionada
    ui.tableWidget->removeRow(selectedRow);

    // Actualizar los encabezados verticales para reflejar los polinomios restantes
    for (int i = 0; i < ui.tableWidget->rowCount(); ++i) {
        QString headerLabel = "Polinomio " + QString::number(i + 1);
        QTableWidgetItem* headerItem = new QTableWidgetItem(headerLabel);
        headerItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setVerticalHeaderItem(i, headerItem);
    }

    ui.tableWidget->resizeRowsToContents();
}

void mulPolinomiosGUI::agregarCoeficiente() {
    // Añadir una nueva columna (coeficiente) a la tabla
    int currentColumnCount = ui.tableWidget->columnCount();
    ui.tableWidget->setColumnCount(currentColumnCount + 1);

    // Crear el encabezado con x y su potencia como superíndice
    QString headerLabel;
    if (currentColumnCount == 0) {
        headerLabel = "x^0";  // El primer encabezado será "ind"
    }
    else if (currentColumnCount == 1) {
        headerLabel = "x";    // El segundo encabezado será "x"
    }
    else {
        headerLabel = "x^" + QString::number(currentColumnCount);  // Los siguientes encabezados serán x^n
    }

    // Usar HTML para el encabezado
    QTableWidgetItem* headerItem = new QTableWidgetItem(headerLabel);
    headerItem->setTextAlignment(Qt::AlignCenter);
    ui.tableWidget->setHorizontalHeaderItem(currentColumnCount, headerItem);

    // Alinear el contenido de las celdas en la nueva columna al centro
    for (int i = 0; i < ui.tableWidget->rowCount(); ++i) {
        QTableWidgetItem* item = ui.tableWidget->item(i, currentColumnCount);
        if (!item) {
            item = new QTableWidgetItem("");  // Inicializar si la celda está vacía
            ui.tableWidget->setItem(i, currentColumnCount, item);
        }
        item->setTextAlignment(Qt::AlignCenter);  // Alinear el texto al centro
    }

    ui.tableWidget->resizeRowsToContents();
    ui.tableWidget->resizeColumnsToContents();
}

// Function to sum polynomials
std::vector<long double> sumPolynomials(const std::vector<long double>& poly1, const std::vector<long double>& poly2) {
    std::vector<long double> result(std::max(poly1.size(), poly2.size()), 0);
    for (size_t i = 0; i < poly1.size(); ++i) {
        result[i] += poly1[i];
    }
    for (size_t i = 0; i < poly2.size(); ++i) {
        result[i] += poly2[i];
    }
    return result;
}

// Función auxiliar para la multiplicación de polinomios
std::vector<long double> multiplyPolynomials(const std::vector<long double>& a, const std::vector<long double>& b) {
    std::vector<long double> result(a.size() + b.size() - 1, 0);
    for (size_t i = 0; i < a.size(); ++i) {
        for (size_t j = 0; j < b.size(); ++j) {
            result[i + j] += a[i] * b[j];
        }
    }
    return result;
}

// Lagrange interpolation
std::vector<long double> Lagrange(const std::vector<long double>& coefficientsA, const std::vector<long double>& coefficientsB) {
    size_t n = coefficientsA.size() + coefficientsB.size() - 1;
    std::vector<long double> Tanteo(n, 0);
    std::vector<long double> BasePolynomial(n, 0);

    auto tanteoPolynomial = [](const std::vector<long double>& coefficients, int i) {
        long double result = 1;
        for (long double coef : coefficients) {
            result *= coef * i;
        }
        return result;
        };

    for (size_t i = 0; i < n; ++i) {
        Tanteo[i] = tanteoPolynomial(coefficientsA, i + 1) * tanteoPolynomial(coefficientsB, i + 1);
    }

    for (size_t k = 1; k <= n; ++k) {
        std::vector<long double> BaseMult = { 1 };
        long double baseNum = Tanteo[k - 1];
        for (size_t j = 1; j <= n; ++j) {
            if (k != j) {
                std::vector<long double> basic = { 1, -static_cast<long double>(j) };
                BaseMult = multiplyPolynomials(BaseMult, basic);
                baseNum /= (k - j);
            }
        }
        for (long double& coef : BaseMult) {
            coef *= baseNum;
        }
        BasePolynomial = sumPolynomials(BaseMult, BasePolynomial);
    }

    return BasePolynomial;
}

// Fast Fourier Transform (FFT)
void fft(std::vector<std::complex<long double>>& X, bool inv = false) {
    size_t n = X.size();
    if (n <= 1) return;

    std::vector<std::complex<long double>> even(n / 2), odd(n / 2);
    for (size_t i = 0; i < n / 2; ++i) {
        even[i] = X[2 * i];
        odd[i] = X[2 * i + 1];
    }

    fft(even, inv);
    fft(odd, inv);

    for (size_t k = 0; k < n / 2; ++k) {
        std::complex<long double> t = std::polar(1.0L, 2 * PI * k / n * (inv ? 1 : -1)) * odd[k];
        X[k] = even[k] + t;
        X[k + n / 2] = even[k] - t;
        if (inv) {
            X[k] /= 2;
            X[k + n / 2] /= 2;
        }
    }
}

// Vandermonde method with real numbers using FFT
std::vector<long double> VandermondeReales(const std::vector<long double>& coefficientsA, const std::vector<long double>& coefficientsB) {
    int n = 1;
    while (n < coefficientsA.size() + coefficientsB.size() - 1) {
        n <<= 1;
    }

    std::vector<std::complex<long double>> A(n), B(n);
    for (size_t i = 0; i < coefficientsA.size(); ++i) A[i] = std::complex<long double>(coefficientsA[i], 0.0);
    for (size_t i = 0; i < coefficientsB.size(); ++i) B[i] = std::complex<long double>(coefficientsB[i], 0.0);

    fft(A);
    fft(B);

    for (int i = 0; i < n; ++i) {
        A[i] *= B[i];
    }

    fft(A, true);

    std::vector<long double> result(n);
    for (int i = 0; i < n; ++i) {
        result[i] = std::round(A[i].real());
    }

    while (!result.empty() && std::abs(result.back()) < 1e-10) {
        result.pop_back();
    }

    return result;
}

// Vandermonde method with imaginary numbers
std::vector<long double> VandermondeImaginarios(const std::vector<long double>& coefficientsA, const std::vector<long double>& coefficientsB) {
    int n = 1;
    while (n < coefficientsA.size() + coefficientsB.size() - 1) {
        n <<= 1;
    }

    std::vector<std::complex<long double>> A(n), B(n);
    for (size_t i = 0; i < coefficientsA.size(); ++i) A[i] = std::complex<long double>(coefficientsA[i], 0);
    for (size_t i = 0; i < coefficientsB.size(); ++i) B[i] = std::complex<long double>(coefficientsB[i], 0);

    fft(A);
    fft(B);

    for (int i = 0; i < n; ++i) {
        A[i] *= B[i];
    }

    fft(A, true);

    std::vector<long double> result(n);
    for (int i = 0; i < n; ++i) {
        result[i] = std::round(A[i].real());
    }

    while (!result.empty() && std::abs(result.back()) < 1e-10) {
        result.pop_back();
    }

    return result;
}

// Bit reversal method
std::vector<long double> BitReverso(const std::vector<long double>& coefficientsA, const std::vector<long double>& coefficientsB) {
    int n = 1;
    while (n < coefficientsA.size() + coefficientsB.size() - 1) {
        n <<= 1;
    }

    std::vector<std::complex<long double>> A(n), B(n);
    for (size_t i = 0; i < coefficientsA.size(); ++i) A[i] = std::complex<long double>(coefficientsA[i], 0);
    for (size_t i = 0; i < coefficientsB.size(); ++i) B[i] = std::complex<long double>(coefficientsB[i], 0);

    fft(A);
    fft(B);

    for (int i = 0; i < n; ++i) {
        A[i] *= B[i];
    }

    fft(A, true);

    std::vector<long double> result(n);
    for (int i = 0; i < n; ++i) {
        result[i] = std::round(A[i].real());
    }

    while (!result.empty() && std::abs(result.back()) < 1e-10) {
        result.pop_back();
    }

    return result;
}

void mulPolinomiosGUI::rellenarCeldasVaciasConCeros() {
    int rowCount = ui.tableWidget->rowCount();
    int columnCount = ui.tableWidget->columnCount();

    // Recorrer todas las celdas de la tabla
    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < columnCount; ++j) {
            QTableWidgetItem* item = ui.tableWidget->item(i, j);
            if (!item || item->text().isEmpty()) {
                // Si la celda está vacía, insertar "0"
                if (!item) {
                    item = new QTableWidgetItem("0");
                    ui.tableWidget->setItem(i, j, item);
                }
                else {
                    item->setText("0");
                }
                item->setTextAlignment(Qt::AlignCenter);  // Alinear el texto al centro
            }
        }
    }

    ui.tableWidget->resizeRowsToContents();
    ui.tableWidget->resizeColumnsToContents();
}

void mulPolinomiosGUI::reset() {
    // Limpiar el contenido de la tabla de coeficientes y resultados
    limpiarTabla(ui.tableWidget);
    limpiarTabla(ui.tableWidgetResult);

    // Reiniciar el estado de las etiquetas
    ui.lblResultado->setText("Sin resultados");
    ui.lblEstado->clear();

    // Detener cualquier animación de carga
    if (ui.lblEstado->movie()) {
        ui.lblEstado->movie()->stop();
    }

    // Reiniciar el comboBox de métodos de multiplicación a su valor por defecto
    ui.comboBox->setCurrentIndex(0);

    // Mostrar un mensaje de que la interfaz ha sido reseteada
    QMessageBox::information(this, "Reinicio", "La interfaz ha sido reiniciada.");
}

// Función que se ejecuta al presionar el botón de multiplicar
void mulPolinomiosGUI::multiplicarPolinomios()
{
    rellenarCeldasVaciasConCeros();
    limpiarTabla(ui.tableWidgetResult);

    // Mostrar el ícono de carga
    ui.lblResultado->setText("");
    QMovie* loadingMovie = new QMovie("./images/carga.gif");
    ui.lblEstado->setScaledContents(true);
    ui.lblEstado->setMovie(loadingMovie);
    ui.lblEstado->show();
    loadingMovie->start();  // Iniciar la animación de carga

    // Retrasar la ejecución de la multiplicación con un QTimer
    QTimer::singleShot(500, this, [this, loadingMovie]() {
        // Realizar la multiplicación
        int numRows = ui.tableWidget->rowCount();    // Número de polinomios
        int numCols = ui.tableWidget->columnCount(); // Número de términos en cada polinomio

        // Leer los coeficientes desde el QTableWidget
        std::vector<std::vector<long double>> polinomios(numRows, std::vector<long double>(numCols));

        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                QTableWidgetItem* item = ui.tableWidget->item(i, j);
                if (item && !item->text().isEmpty()) {
                    polinomios[i][j] = item->text().toLongLong();
                }
                else {
                    polinomios[i][j] = 0; // Si la celda está vacía, se toma como 0
                }
            }
        }

        // Determinar el método de multiplicación seleccionado
        QString selectedMethod = ui.comboBox->currentText();
        std::vector<long double> result;
        QElapsedTimer temporizador;

        // Multiplicar los polinomios de acuerdo al método seleccionado
        if (selectedMethod == "Vandermonde Reales") {
            temporizador.start();
            result = polinomios[0];
            for (int i = 1; i < numRows; ++i) {
                result = VandermondeReales(result, polinomios[i]);
            }
        }
        else if (selectedMethod == "Vandermonde Imaginarios") {
            temporizador.start();
            result = polinomios[0];
            for (int i = 1; i < numRows; ++i) {
                result = VandermondeImaginarios(result, polinomios[i]);
            }
        }
        else if (selectedMethod == "Bit Reverso") {
            temporizador.start();
            result = polinomios[0];
            for (int i = 1; i < numRows; ++i) {
                result = BitReverso(result, polinomios[i]);
            }
        }
        else {
            QMessageBox::warning(this, "Error", "Método de multiplicación no reconocido.");
            loadingMovie->stop();
            return;
        }

        qint64 tiempoTranscurrido = temporizador.elapsed();
        // Almacenar el tiempo en un vector
        tiempos.push_back(tiempoTranscurrido);

        // Mostrar el resultado en el QTextEdit
        displayResultInTable(result);

        // Detener el icono de carga y mostrar el mensaje de éxito
        loadingMovie->stop();
        ui.lblEstado->hide();
        ui.lblResultado->setText("Multiplicacion con " + selectedMethod + " exitosa");
        });
}

void mulPolinomiosGUI::limpiarTabla(QTableWidget* tabla) {
    tabla->clearContents();
    tabla->setRowCount(0);
    tabla->setColumnCount(0);
}

void mulPolinomiosGUI::displayResultInTable(const std::vector<long double>& result) {
    // Configurar el QTableWidget para mostrar el polinomio resultante
    int resultSize = result.size();
    ui.tableWidgetResult->setRowCount(1);  // Solo una fila para el polinomio resultante
    ui.tableWidgetResult->setColumnCount(resultSize);  // Cada término es una columna

    // Colocar cada coeficiente con su respectiva variable x y el exponente como superíndice
    for (int i = 0; i < resultSize; ++i) {
        QString term;

        // Mostrar el coeficiente si es diferente de cero
        if (std::abs(result[i]) > 1e-10) {
            term += QString::number(static_cast<double>(std::abs(result[i])));  // Coeficiente

            if (i > 0) {
                term += "x";  // Variable x
                if (i > 1) {
                    // Usar <sup> para el exponente
                    term += "<sup>" + QString::number(i) + "</sup>";
                }
            }

            // Crear un QLabel para mostrar el término con HTML
            QLabel* label = new QLabel(term);
            label->setAlignment(Qt::AlignCenter);  // Centrar el texto
            label->setTextFormat(Qt::RichText);    // Permitir que QLabel interprete HTML
            ui.tableWidgetResult->setCellWidget(0, resultSize - i - 1, label);  // Añadir el QLabel como widget de celda
        }
        else {
            // Insertar una celda vacía si el coeficiente es cero
            QLabel* label = new QLabel("");
            label->setAlignment(Qt::AlignCenter);
            ui.tableWidgetResult->setCellWidget(0, resultSize - i - 1, label);
        }
    }

    // Ajustar el tamaño de las celdas para mejor visualización
    ui.tableWidgetResult->resizeColumnsToContents();
}

//void fft_dft_gui::graficarTiempo() {
//    // Asegúrate de tener los tiempos listos
//    if (tiempos.empty()) {
//        QMessageBox::warning(this, "Error", "No hay datos de tiempo para graficar.");
//        return;
//    }
//
//    // Crear un nuevo gráfico
//    QChart* chart = new QChart();
//    chart->setTitle("Tiempo de Ejecución de Métodos");
//
//    // Crear la serie
//    QLineSeries* series = new QLineSeries();
//    for (int i = 0; i < tiempos.size(); ++i) {
//        series->append(i, tiempos[i]); // Usar índices como x y tiempos como y
//    }
//
//    // Añadir la serie al gráfico
//    chart->addSeries(series);
//    chart->createDefaultAxes();
//    chart->axes(Qt::Vertical).first()->setTitleText("Tiempo (ms)");
//    chart->axes(Qt::Horizontal).first()->setTitleText("Métodos");
//
//    // Crear un gráfico de vista
//    QChartView* chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);
//
//    // Mostrar el gráfico en una ventana nueva
//    chartView->resize(640, 480);
//    chartView->show();
//
//    //// Mostrar la ventana de gráfico
//    //QWidget* ventanaGrafica = new QWidget();
//    //QVBoxLayout* layout = new QVBoxLayout(ventanaGrafica);
//    //layout->addWidget(chartView);
//    //ventanaGrafica->setLayout(layout);
//    //ventanaGrafica->setWindowTitle("Gráfica de Tiempo de Ejecución");
//    //ventanaGrafica->resize(800, 600);
//    //ventanaGrafica->show();
//}