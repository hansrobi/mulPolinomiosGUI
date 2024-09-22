#include "mulPolinomiosGUI.h"
#include <vector>
#include <iostream>
#include <complex>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <QMessageBox>
#include <QLabel>
#include <QTimer>
#include <QElapsedTimer>
#include <QMovie>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QVBoxLayout>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>

const long double PI = 3.14159265358979323846L;

mulPolinomiosGUI::mulPolinomiosGUI(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Establecer un tamaño fijo para la ventana
    setFixedSize(731, 470);

    // Conectar los botones a las acciones
    connect(ui.btnAgregarPolinomio, &QPushButton::clicked, this, &mulPolinomiosGUI::agregarPolinomio);
    connect(ui.btnEliminarPolinomio, &QPushButton::clicked, this, &mulPolinomiosGUI::eliminarPolinomio);
    connect(ui.btnAgregarCoeficiente, &QPushButton::clicked, this, &mulPolinomiosGUI::agregarCoeficiente);
    connect(ui.btnGenerarPolinomios, &QPushButton::clicked, this, &mulPolinomiosGUI::generarPolinomiosAleatorios);
    connect(ui.btnMultiplicar, &QPushButton::clicked, this, &mulPolinomiosGUI::multiplicarPolinomios);
    connect(ui.btnReset, &QPushButton::clicked, this, &mulPolinomiosGUI::reset);
    connect(ui.btnGraficarTiempo, &QPushButton::clicked, this, &mulPolinomiosGUI::graficarTiempo);
}

mulPolinomiosGUI::~mulPolinomiosGUI()
{}

void mulPolinomiosGUI::agregarPolinomio() {
    // Añadir una nueva fila (polinomio) a la tabla
    int contFilasActual = ui.tablaPolinomios->rowCount();
    ui.tablaPolinomios->setRowCount(contFilasActual + 1);

    // Actualizar el encabezado vertical con "Polinomio X"
    for (int i = 0; i < ui.tablaPolinomios->rowCount(); ++i) {
        QString etiquetaEncabezado = "Polinomio " + QString::number(i + 1);
        QTableWidgetItem* itemEncabezado = new QTableWidgetItem(etiquetaEncabezado);
        itemEncabezado->setTextAlignment(Qt::AlignCenter);
        ui.tablaPolinomios->setVerticalHeaderItem(i, itemEncabezado);
    }

    // Alinear el contenido de las celdas en la nueva fila al centro
    for (int j = 0; j < ui.tablaPolinomios->columnCount(); ++j) {
        QTableWidgetItem* nuevoItem = new QTableWidgetItem("");  // Puedes inicializar con algún valor si es necesario
        nuevoItem->setTextAlignment(Qt::AlignCenter);  // Alinear el texto al centro
        ui.tablaPolinomios->setItem(contFilasActual, j, nuevoItem);
    }

    ajustarTabla();
}

void mulPolinomiosGUI::eliminarPolinomio() {
    // Obtener la fila seleccionada
    QList<QTableWidgetSelectionRange> rangoSeleccionado = ui.tablaPolinomios->selectedRanges();
    if (rangoSeleccionado.isEmpty()) {
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
    int filaSeleccionada = rangoSeleccionado.first().topRow();

    // Eliminar la fila seleccionada
    ui.tablaPolinomios->removeRow(filaSeleccionada);

    // Actualizar los encabezados verticales para reflejar los polinomios restantes
    for (int i = 0; i < ui.tablaPolinomios->rowCount(); ++i) {
        QString etiquetaEncabezado = "Polinomio " + QString::number(i + 1);
        QTableWidgetItem* itemEncabezado = new QTableWidgetItem(etiquetaEncabezado);
        itemEncabezado->setTextAlignment(Qt::AlignCenter);
        ui.tablaPolinomios->setVerticalHeaderItem(i, itemEncabezado);
    }

    ajustarTabla();
}

void mulPolinomiosGUI::agregarCoeficiente() {
    // Añadir una nueva columna (coeficiente) a la tabla
    int contColumnasActual = ui.tablaPolinomios->columnCount();
    ui.tablaPolinomios->setColumnCount(contColumnasActual + 1);

    // Crear el encabezado con x y su potencia como superíndice
    QString etiquetaEncabezado;
    if (contColumnasActual == 0) {
        etiquetaEncabezado = "x^0";  // El primer encabezado será "x^0"
    }
    else if (contColumnasActual == 1) {
        etiquetaEncabezado = "x";    // El segundo encabezado será "x"
    }
    else {
        etiquetaEncabezado = "x^" + QString::number(contColumnasActual);  // Los siguientes encabezados serán x^n
    }

    QTableWidgetItem* itemEncabezado = new QTableWidgetItem(etiquetaEncabezado);
    itemEncabezado->setTextAlignment(Qt::AlignCenter);
    ui.tablaPolinomios->setHorizontalHeaderItem(contColumnasActual, itemEncabezado);

    // Alinear el contenido de las celdas en la nueva columna al centro
    for (int i = 0; i < ui.tablaPolinomios->rowCount(); ++i) {
        QTableWidgetItem* item = ui.tablaPolinomios->item(i, contColumnasActual);
        if (!item) {
            item = new QTableWidgetItem("");  // Inicializar si la celda está vacía
            ui.tablaPolinomios->setItem(i, contColumnasActual, item);
        }
        item->setTextAlignment(Qt::AlignCenter);  // Alinear el texto al centro
    }

    ajustarTabla();
}

void mulPolinomiosGUI::generarPolinomiosAleatorios() {
    // Inicializar el generador de números aleatorios
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int numPolinomios = std::rand() % 8 + 3; // Generar un número aleatorio de polinomios entre 3 y 8
    int numTerminos = std::rand() % 20 + 10; // Generar un número aleatorio de términos entre 10 y 20

    // Limpiar la tabla antes de generar los nuevos polinomios aleatorios
    reset();

    // Añadir columnas (términos) al TableWidget si es necesario
    for (int col = 0; col < numTerminos; ++col) {
        agregarCoeficiente();
    }

    // Añadir filas (polinomios) y rellenar con coeficientes aleatorios
    for (int fila = 0; fila < numPolinomios; ++fila) {
        agregarPolinomio(); // Agregar una fila

        for (int col = 0; col < numTerminos; ++col) {
            int coeficienteAleatorio = std::rand() % 30 + 5; // Generar un coeficiente aleatorio entre 5 y 30

            // Crear un QTableWidgetItem con el valor del coeficiente aleatorio
            QTableWidgetItem* nuevoItem = new QTableWidgetItem(QString::number(coeficienteAleatorio));
            nuevoItem->setTextAlignment(Qt::AlignCenter);

            // Insertar el coeficiente en la tabla
            ui.tablaPolinomios->setItem(fila, col, nuevoItem);
        }
    }

    ajustarTabla();
}

void mulPolinomiosGUI::ajustarTabla() {
    ui.tablaPolinomios->resizeRowsToContents();
    ui.tablaPolinomios->resizeColumnsToContents();
}

// Funcion para sumar polinomios
std::vector<long double> sumaPolinomios(const std::vector<long double>& pol1, const std::vector<long double>& pol2) {
    std::vector<long double> resultado(std::max(pol1.size(), pol2.size()), 0);
    for (size_t i = 0; i < pol1.size(); ++i) {
        resultado[i] += pol1[i];
    }
    for (size_t i = 0; i < pol2.size(); ++i) {
        resultado[i] += pol2[i];
    }
    return resultado;
}

// Función auxiliar para la multiplicación de polinomios
std::vector<long double> multiplicacionPolinomios(const std::vector<long double>& pol1, const std::vector<long double>& pol2) {
    std::vector<long double> resultado(pol1.size() + pol2.size() - 1, 0);
    for (size_t i = 0; i < pol1.size(); ++i) {
        for (size_t j = 0; j < pol2.size(); ++j) {
            resultado[i + j] += pol1[i] * pol2[j];
        }
    }
    return resultado;
}

// Interpolación de Lagrange
std::vector<long double> lagrange(const std::vector<long double>& coeficientesA, const std::vector<long double>& coeficientesB) {
    size_t n = coeficientesA.size() + coeficientesB.size() - 1;
    std::vector<long double> tanteo(n, 0);
    std::vector<long double> basePolinomial(n, 0);

    auto tanteoPolinomial = [](const std::vector<long double>& coeficientes, int i) {
        long double resultado = 1;
        for (long double coef : coeficientes) {
            resultado *= coef * i;
        }
        return resultado;
        };

    for (size_t i = 0; i < n; ++i) {
        tanteo[i] = tanteoPolinomial(coeficientesA, i + 1) * tanteoPolinomial(coeficientesB, i + 1);
    }

    for (size_t k = 1; k <= n; ++k) {
        std::vector<long double> baseMult = { 1 };
        long double baseNum = tanteo[k - 1];
        for (size_t j = 1; j <= n; ++j) {
            if (k != j) {
                std::vector<long double> basic = { 1, -static_cast<long double>(j) };
                baseMult = multiplicacionPolinomios(baseMult, basic);
                baseNum /= (k - j);
            }
        }
        for (long double& coef : baseMult) {
            coef *= baseNum;
        }
        basePolinomial = sumaPolinomios(baseMult, basePolinomial);
    }

    return basePolinomial;
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

// FFT con Matriz de Vandermonde en Reales
std::vector<long double> vandermondeReales(const std::vector<long double>& coeficientesA, const std::vector<long double>& coeficientesB) {
    int n = 1;
    while (n < coeficientesA.size() + coeficientesB.size() - 1) {
        n <<= 1;
    }

    std::vector<std::complex<long double>> A(n), B(n);
    for (size_t i = 0; i < coeficientesA.size(); ++i) A[i] = std::complex<long double>(coeficientesA[i], 0.0);
    for (size_t i = 0; i < coeficientesB.size(); ++i) B[i] = std::complex<long double>(coeficientesB[i], 0.0);

    fft(A);
    fft(B);

    for (int i = 0; i < n; ++i) {
        A[i] *= B[i];
    }

    fft(A, true);

    std::vector<long double> resultado(n);
    for (int i = 0; i < n; ++i) {
        resultado[i] = std::round(A[i].real());
    }

    while (!resultado.empty() && std::abs(resultado.back()) < 1e-10) {
        resultado.pop_back();
    }

    return resultado;
}

// FFT con Matriz de Vandermonde en Imaginarios
std::vector<long double> vandermondeImaginarios(const std::vector<long double>& coeficientesA, const std::vector<long double>& coeficientesB) {
    int n = 1;
    while (n < coeficientesA.size() + coeficientesB.size() - 1) {
        n <<= 1;
    }

    std::vector<std::complex<long double>> A(n), B(n);
    for (size_t i = 0; i < coeficientesA.size(); ++i) A[i] = std::complex<long double>(coeficientesA[i], 0);
    for (size_t i = 0; i < coeficientesB.size(); ++i) B[i] = std::complex<long double>(coeficientesB[i], 0);

    fft(A);
    fft(B);

    for (int i = 0; i < n; ++i) {
        A[i] *= B[i];
    }

    fft(A, true);

    std::vector<long double> resultado(n);
    for (int i = 0; i < n; ++i) {
        resultado[i] = std::round(A[i].real());
    }

    while (!resultado.empty() && std::abs(resultado.back()) < 1e-10) {
        resultado.pop_back();
    }

    return resultado;
}

// FFT usando Bit Reverso
std::vector<long double> BitReverso(const std::vector<long double>& coeficientesA, const std::vector<long double>& coeficientesB) {
    int n = 1;
    while (n < coeficientesA.size() + coeficientesB.size() - 1) {
        n <<= 1;
    }

    std::vector<std::complex<long double>> A(n), B(n);
    for (size_t i = 0; i < coeficientesA.size(); ++i) A[i] = std::complex<long double>(coeficientesA[i], 0);
    for (size_t i = 0; i < coeficientesB.size(); ++i) B[i] = std::complex<long double>(coeficientesB[i], 0);

    fft(A);
    fft(B);

    for (int i = 0; i < n; ++i) {
        A[i] *= B[i];
    }

    fft(A, true);

    std::vector<long double> resultado(n);
    for (int i = 0; i < n; ++i) {
        resultado[i] = std::round(A[i].real());
    }

    while (!resultado.empty() && std::abs(resultado.back()) < 1e-10) {
        resultado.pop_back();
    }

    return resultado;
}

void mulPolinomiosGUI::rellenarCeldasVaciasConCeros() {
    int contFilas = ui.tablaPolinomios->rowCount();
    int contColumnas = ui.tablaPolinomios->columnCount();

    // Recorrer todas las celdas de la tabla
    for (int i = 0; i < contFilas; ++i) {
        for (int j = 0; j < contColumnas; ++j) {
            QTableWidgetItem* item = ui.tablaPolinomios->item(i, j);
            if (!item || item->text().isEmpty()) {
                // Si la celda está vacía, insertar "0"
                if (!item) {
                    item = new QTableWidgetItem("0");
                    ui.tablaPolinomios->setItem(i, j, item);
                }
                else {
                    item->setText("0");
                }
                item->setTextAlignment(Qt::AlignCenter);  // Alinear el texto al centro
            }
        }
    }

    ui.tablaPolinomios->resizeRowsToContents();
    ui.tablaPolinomios->resizeColumnsToContents();
}

void mulPolinomiosGUI::reset() {
    // Limpiar el contenido de la tabla de coeficientes y resultados
    limpiarTabla(ui.tablaPolinomios);
    limpiarTabla(ui.tablaResultados);

    // Reiniciar el estado de las etiquetas
    ui.lblResultado->setText("Sin resultados");
    ui.lblEstado->clear();

    // Detener cualquier animación de carga
    if (ui.lblEstado->movie()) {
        ui.lblEstado->movie()->stop();
    }

    // Reiniciar el comboBox de métodos de multiplicación a su valor por defecto
    ui.comboBox->setCurrentIndex(0);
}

bool mulPolinomiosGUI::validarDatosEntrada() {
    int contPolinomios = ui.tablaPolinomios->rowCount();
    int contTerminos = ui.tablaPolinomios->columnCount();

    // Validar que haya polinomios
    if (contPolinomios == 0 || contTerminos == 0) {
        QMessageBox::warning(this, "Error", "No hay polinomios o terminos disponibles para multiplicar.");
        return false;
    }

    // Recorrer todas las celdas de la tabla
    for (int i = 0; i < contPolinomios; ++i) {
        for (int j = 0; j < contTerminos; ++j) {
            QTableWidgetItem* item = ui.tablaPolinomios->item(i, j);

            // Intentar convertir el texto de la celda a un número
            bool conversionExitosa;
            item->text().toDouble(&conversionExitosa);
            if (!conversionExitosa) {
                QMessageBox::warning(this, "Entrada invalida", "Se han detectado valores no numericos. Por favor, ingresa solo números.");
                return false;
            }
        }
    }

    // Si todas las validaciones pasan
    return true;
}

// Función que se ejecuta al presionar el botón de multiplicar
void mulPolinomiosGUI::multiplicarPolinomios()
{
    rellenarCeldasVaciasConCeros();

    // Validar los datos antes de proceder
    if (!validarDatosEntrada()) {
        return;
    }

    limpiarTabla(ui.tablaResultados);

    // Mostrar el ícono de carga
    ui.lblResultado->setText("");
    QMovie* gifCarga = new QMovie("./images/carga.gif");
    ui.lblEstado->setScaledContents(true);
    ui.lblEstado->setMovie(gifCarga);
    ui.lblEstado->show();
    gifCarga->start();  // Iniciar la animación de carga

    // Leer los coeficientes actuales desde el QTableWidget
    int numPolinomios = ui.tablaPolinomios->rowCount();
    int numTerminos = ui.tablaPolinomios->columnCount();

    std::vector<std::vector<long double>> polinomiosActuales(numPolinomios, std::vector<long double>(numTerminos));

    for (int i = 0; i < numPolinomios; ++i) {
        for (int j = 0; j < numTerminos; ++j) {
            QTableWidgetItem* item = ui.tablaPolinomios->item(i, j);
            polinomiosActuales[i][j] = item && !item->text().isEmpty() ? item->text().toLongLong() : 0;
        }
    }

    // Comparar los polinomios actuales con los anteriores
    if (polinomiosActuales != polinomiosAnteriores) {
        // Si los polinomios son diferentes, reiniciar los tiempos
        tiempos.clear();
        QMessageBox::information(this, "Reinicio de tiempos", "Se detecto un cambio en los polinomios. Los tiempos de ejecucion han sido reiniciados.");
    }

    polinomiosAnteriores = polinomiosActuales;

    // Retrasar la ejecución de la multiplicación con un QTimer
    QTimer::singleShot(500, this, [this, gifCarga, polinomiosActuales]() {
        int numPolinomios = polinomiosActuales.size();

        // Determinar el método de multiplicación seleccionado
        QString metodoElegido = ui.comboBox->currentText();
        std::vector<long double> resultado = polinomiosActuales[0];
        QElapsedTimer temporizador;

        // Seleccionar el método de multiplicación
        temporizador.start();
        for (int i = 1; i < numPolinomios; ++i) {
            if (metodoElegido == "Vandermonde Reales") {
                resultado = vandermondeReales(resultado, polinomiosActuales[i]);
            }
            else if (metodoElegido == "Vandermonde Imaginarios") {
                resultado = vandermondeImaginarios(resultado, polinomiosActuales[i]);
            }
            else if (metodoElegido == "Bit Reverso") {
                resultado = BitReverso(resultado, polinomiosActuales[i]);
            }
        }

        qint64 tiempoTranscurrido = temporizador.elapsed();
        // Almacenar el tiempo en el map
        tiempos[metodoElegido] = tiempoTranscurrido;

        // Mostrar el resultado en el QTextEdit
        mostrarResultado(resultado);

        // Detener el icono de carga y mostrar el mensaje de éxito
        gifCarga->stop();
        ui.lblEstado->hide();
        ui.lblResultado->setText("Multiplicacion con " + metodoElegido + " exitosa");
        });
}

void mulPolinomiosGUI::limpiarTabla(QTableWidget* tabla) {
    tabla->clearContents();
    tabla->setRowCount(0);
    tabla->setColumnCount(0);
}

void mulPolinomiosGUI::mostrarResultado(const std::vector<long double>& resultado) {
    // Configurar el QTableWidget para mostrar el polinomio resultante
    int tamResultado = resultado.size();
    ui.tablaResultados->setRowCount(1);  // Solo una fila para el polinomio resultante
    ui.tablaResultados->setColumnCount(tamResultado);  // Cada término es una columna

    // Colocar cada coeficiente con su respectiva variable x y el exponente como superíndice
    for (int i = 0; i < tamResultado; ++i) {
        QString termino;

        // Mostrar el coeficiente si es diferente de cero
        if (std::abs(resultado[i]) > 1e-10) {
            termino += QString::number(static_cast<double>(std::abs(resultado[i])));  // Coeficiente

            if (i > 0) {
                termino += "x";  // Variable x
                if (i > 1) {
                    // Usar <sup> para el exponente
                    termino += "<sup>" + QString::number(i) + "</sup>";
                }
            }

            // Crear un QLabel para mostrar el término con HTML
            QLabel* label = new QLabel(termino);
            label->setAlignment(Qt::AlignCenter);
            label->setTextFormat(Qt::RichText);
            ui.tablaResultados->setCellWidget(0, tamResultado - i - 1, label);  // Añadir el QLabel como widget de celda
        }
        else {
            // Insertar una celda vacía si el coeficiente es cero
            QLabel* label = new QLabel("");
            label->setAlignment(Qt::AlignCenter);
            ui.tablaResultados->setCellWidget(0, tamResultado - i - 1, label);
        }
    }

    // Ajustar el tamaño de las celdas para mejor visualización
    ui.tablaResultados->resizeColumnsToContents();
}

void mulPolinomiosGUI::graficarTiempo() {
    // Asegúrate de tener los tiempos listos
    if (tiempos.empty()) {
        QMessageBox::warning(this, "Error", "No hay datos de tiempo para graficar.");
        return;
    }

    // Crear un nuevo gráfico
    QChart* chart = new QChart();
    chart->setTitle("Tiempo de Ejecucion de Metodos");

    // Crear un conjunto de barras
    QBarSet* set = new QBarSet("Tiempo (ms)");

    // Crear una serie de barras y añadir el conjunto
    QBarSeries* series = new QBarSeries();

    // Crear el eje X categorizado con nombres de métodos
    QStringList categorias;

    // Añadir los tiempos al conjunto de barras y las categorías al eje X
    for (const auto& pair : tiempos) {        
        *set << pair.second;
        categorias << pair.first;
    }

    series->append(set);

    // Añadir la serie al gráfico                                                                                                                                                                                                                                                   
    chart->addSeries(series);

    QBarCategoryAxis* ejeX = new QBarCategoryAxis();
    ejeX->append(categorias);
    chart->addAxis(ejeX, Qt::AlignBottom);
    series->attachAxis(ejeX);

    // Crear el eje Y para los tiempos
    QValueAxis* ejeY = new QValueAxis();
    ejeY->setTitleText("Tiempo (ms)");
    chart->addAxis(ejeY, Qt::AlignLeft);
    series->attachAxis(ejeY);

    // Ajustar el rango del eje Y
    qreal maxTiempo = 0;
    for (const auto& pair : tiempos) {
        if (pair.second > maxTiempo) {
            maxTiempo = pair.second;
        }
    }
    ejeY->setRange(0, maxTiempo * 1.1); // Añadir un 10% extra para mejor visualización

    // Crear un gráfico de vista
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Mostrar el gráfico en una ventana nueva
    QWidget* window = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(window);
    layout->addWidget(chartView);
    window->resize(800, 600);
    window->show();
}