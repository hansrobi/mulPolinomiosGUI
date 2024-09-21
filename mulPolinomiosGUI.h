#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mulPolinomiosGUI.h"

class mulPolinomiosGUI : public QMainWindow
{
    Q_OBJECT

public:
    mulPolinomiosGUI(QWidget *parent = nullptr);
    ~mulPolinomiosGUI();

private:
    Ui::mulPolinomiosGUIClass ui;

    // Declaraci�n de funciones
    void agregarPolinomio();
    void eliminarPolinomio();
    void agregarCoeficiente();
    void rellenarCeldasVaciasConCeros();
    void multiplicarPolinomios();
    void limpiarTabla(QTableWidget* tabla);
    void displayResultInTable(const std::vector<long double>& result);
    void reset();
    //void graficarTiempo();
    QVector<qint64> tiempos;  // Almacena los tiempos de ejecuci�n
};
