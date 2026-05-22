#ifndef PLOSHCHADKA_H
#define PLOSHCHADKA_H

#include "Tovar.h"
#include "Odezhda.h"
#include "Elektronika.h"
#include "Kniga.h"
#include "Pokupatel.h"
#include "Pokupka.h"

#include <map>
#include <vector>
#include <string>
#include <utility>

// Торговая площадка - управляет товарами, покупателями и заказами. Реализует правила обработки заказов из задания
class Ploshchadka {
    std::map<std::string, Tovar*> tovary;        // ключ - артикул товара
    std::map<std::string, Pokupatel> pokupateli; // ключ - номер карты
    std::vector<Pokupka> pokupki;

    double S;  // порог суммы заказа для предоставления скидки
    double p;  // размер скидки в процентах

    std::map<std::string, double> vyruchka_po_kategorii;
    std::map<std::string, int> rezerv;
    std::vector<std::string> ochered_pokupatelej;
    std::vector<std::pair<std::string, std::string>> soobshcheniya;

public:
    Ploshchadka(double S, double p) : S(S), p(p) {}

    ~Ploshchadka() {
        for (auto& pair : tovary) delete pair.second;
    }

    void zagruzit_tovary(const std::string& filename);
    void zagruzit_pokupatelej(const std::string& filename);
    void zagruzit_pokupki(const std::string& filename);

    void obrabotat_pokupki();

    void vyvesti_vyruchku() const;
    void vyvesti_ochered_pokupatelej() const;
    void vyvesti_rezerv() const;
    void otpravit_povtornye_napominaniya() const;

private:
    void dobavit_soobshchenie(const std::string& karta, const std::string& msg);
    double summa_korziny(const std::map<std::string, int>& korzina) const;
    double primenit_skidku(double summa) const;
    std::map<std::string, int> optimizirovat_korzinu(
        std::map<std::string, int> korzina, double balans) const;
    void prodat(const std::string& karta, const std::map<std::string, int>& korzina);
};

#endif
