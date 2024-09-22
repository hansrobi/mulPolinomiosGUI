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

    // Declaración de funciones
    void agregarPolinomio();
    void eliminarPolinomio();
    void agregarCoeficiente();
    void ajustarTabla();
    void generarPolinomiosAleatorios();
    void rellenarCeldasVaciasConCeros();
    bool validarDatosEntrada();
    void multiplicarPolinomios();
    void limpiarTabla(QTableWidget* tabla);
    void mostrarResultado(const std::vector<long double>& result);
    void reset();
    void graficarTiempo();
    std::map<QString, qint64> tiempos;
    std::vector<std::vector<long double>> polinomiosAnteriores;
};
