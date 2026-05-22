#include "Ploshchadka.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

// Снимает UTF-8 BOM (EF BB BF) с начала строки, если он есть. Excel при сохранении csv в utf-8 ставит BOM, и без этого первое поле первой строки получит "невидимый" префикс из трёх байт.
static void strip_bom(string& s) {
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF)
        s.erase(0, 3);
}

void Ploshchadka::zagruzit_tovary(const string& filename) {
    ifstream in(filename);
    if (!in.is_open())
        throw runtime_error("Не удалось открыть файл товаров: " + filename);

    string line;
    bool first_line = true;
    while (getline(in, line)) {
        if (first_line) { strip_bom(line); first_line = false; }
        if (line.empty()) continue;

        stringstream ss(line);
        string tip;
        getline(ss, tip, ';');

        try {
            if (tip == "O") {
                string artikul, naim, razmer, cvet, cena_s, kol_s;
                getline(ss, artikul, ';');
                getline(ss, naim, ';');
                getline(ss, razmer, ';');
                getline(ss, cvet, ';');
                getline(ss, cena_s, ';');
                getline(ss, kol_s);
                tovary[artikul] = new Odezhda(artikul, naim, razmer, cvet,
                                              stod(cena_s), stoi(kol_s));
            }
            else if (tip == "E") {
                string artikul, naim, model, proiz, cena_s, kol_s;
                getline(ss, artikul, ';');
                getline(ss, naim, ';');
                getline(ss, model, ';');
                getline(ss, proiz, ';');
                getline(ss, cena_s, ';');
                getline(ss, kol_s);
                tovary[artikul] = new Elektronika(artikul, naim, model, proiz,
                                                  stod(cena_s), stoi(kol_s));
            }
            else if (tip == "B") {
                string artikul, avtor, nazv, cena_s, izd, kol_s;
                getline(ss, artikul, ';');
                getline(ss, avtor, ';');
                getline(ss, nazv, ';');
                getline(ss, cena_s, ';');
                getline(ss, izd, ';');
                getline(ss, kol_s);
                tovary[artikul] = new Kniga(artikul, avtor, nazv,
                                            stod(cena_s), izd, stoi(kol_s));
            }
        }
        catch (const exception& e) {
            cerr << "Ошибка при разборе строки товара: " << e.what() << endl;
        }
    }
}

void Ploshchadka::zagruzit_pokupatelej(const string& filename) {
    ifstream in(filename);
    if (!in.is_open())
        throw runtime_error("Не удалось открыть файл покупателей: " + filename);

    string line;
    bool first_line = true;
    while (getline(in, line)) {
        if (first_line) { strip_bom(line); first_line = false; }
        if (line.empty()) continue;

        stringstream ss(line);
        string fio, karta, schet_s;
        getline(ss, fio, ';');
        getline(ss, karta, ';');
        getline(ss, schet_s);

        try {
            pokupateli[karta] = Pokupatel(fio, karta, stod(schet_s));
        }
        catch (const exception& e) {
            cerr << "Ошибка при разборе строки покупателя: " << e.what() << endl;
        }
    }
}

void Ploshchadka::zagruzit_pokupki(const string& filename) {
    ifstream in(filename);
    if (!in.is_open())
        throw runtime_error("Не удалось открыть файл покупок: " + filename);

    string line;
    bool first_line = true;
    while (getline(in, line)) {
        if (first_line) { strip_bom(line); first_line = false; }
        if (line.empty()) continue;

        stringstream ss(line);
        string nz_s, karta, artikul, kol_s;
        getline(ss, nz_s, ';');
        getline(ss, karta, ';');
        getline(ss, artikul, ';');
        getline(ss, kol_s);

        try {
            Pokupka p{ stoi(nz_s), karta, artikul, stoi(kol_s) };
            pokupki.push_back(p);
        }
        catch (const exception& e) {
            cerr << "Ошибка при разборе строки покупки: " << e.what() << endl;
        }
    }
}

double Ploshchadka::summa_korziny(const map<string, int>& korzina) const {
    double summa = 0;
    for (const auto& pair : korzina) {
        auto it = tovary.find(pair.first);
        if (it != tovary.end())
            summa += it->second->get_cena() * pair.second;
    }
    return summa;
}

double Ploshchadka::primenit_skidku(double summa) const {
    if (summa > S) return summa * (1.0 - p / 100.0);
    return summa;
}

void Ploshchadka::dobavit_soobshchenie(const string& karta, const string& msg) {
    soobshcheniya.push_back({ karta, msg });
    // в очередь покупателя добавляем только если его там ещё нет
    for (const string& k : ochered_pokupatelej)
        if (k == karta) return;
    ochered_pokupatelej.push_back(karta);
}

// Правило 3: жадная оптимизация корзины - удаляем товары с минимальной суммарной стоимостью (цена * количество) пока корзина не уместится в баланс.
map<string, int> Ploshchadka::optimizirovat_korzinu(map<string, int> korzina,
                                                    double balans) const {
    while (!korzina.empty()) {
        if (primenit_skidku(summa_korziny(korzina)) <= balans)
            return korzina;

        auto min_it = korzina.begin();
        double min_summa = tovary.at(min_it->first)->get_cena() * min_it->second;
        for (auto it = ++korzina.begin(); it != korzina.end(); ++it) {
            double s = tovary.at(it->first)->get_cena() * it->second;
            if (s < min_summa) {
                min_summa = s;
                min_it = it;
            }
        }
        korzina.erase(min_it);
    }
    return korzina;
}

void Ploshchadka::prodat(const string& karta, const map<string, int>& korzina) {
    double brutto = summa_korziny(korzina);
    double netto = primenit_skidku(brutto);
    // коэффициент скидки нужен чтобы разнести выручку по категориям корректно
    double koef = brutto > 0 ? netto / brutto : 1.0;

    for (const auto& pair : korzina) {
        Tovar* t = tovary[pair.first];
        vyruchka_po_kategorii[t->get_kategoriya()] += t->get_cena() * pair.second * koef;
        t->zarezervirovat(pair.second);
    }
    pokupateli[karta].spisat(netto);
}

void Ploshchadka::obrabotat_pokupki() {
    // группируем заказы по покупателю, сохраняем порядок первого появления (правило 2 - заказы обрабатываются последовательно)
    vector<string> poryadok;
    map<string, vector<Pokupka>> zakazy;

    for (const Pokupka& pk : pokupki) {
        if (zakazy.find(pk.nomer_karty) == zakazy.end())
            poryadok.push_back(pk.nomer_karty);
        zakazy[pk.nomer_karty].push_back(pk);
    }

    for (const string& karta : poryadok) {
        auto buyer_it = pokupateli.find(karta);
        if (buyer_it == pokupateli.end()) continue;
        Pokupatel& buyer = buyer_it->second;

        // собираем корзину: суммируем количества по одинаковым артикулам
        map<string, int> korzina;
        for (const Pokupka& z : zakazy[karta])
            korzina[z.artikul] += z.kolichestvo;

        // Правило 2: проверка наличия. Если артикула нет или не хватает - удаляем из заказа и отправляем сообщение покупателю.
        for (auto it = korzina.begin(); it != korzina.end(); ) {
            auto tov_it = tovary.find(it->first);
            if (tov_it == tovary.end()) {
                dobavit_soobshchenie(karta,
                    "Артикул " + it->first + " отсутствует в каталоге, удалён из заказа.");
                it = korzina.erase(it);
            }
            else if (!tov_it->second->est_v_nalichii(it->second)) {
                ostringstream msg;
                msg << "Артикул " << it->first << " (\""
                    << tov_it->second->get_naimenovanie()
                    << "\") в количестве " << it->second
                    << " отсутствует на складе, удалён из заказа.";
                dobavit_soobshchenie(karta, msg.str());
                it = korzina.erase(it);
            }
            else ++it;
        }

        if (korzina.empty()) continue;

        double itogo = primenit_skidku(summa_korziny(korzina));

        if (buyer.mozhet_oplatit(itogo)) {
            // средств хватает - продаём оставшиеся артикулы (правило 2)
            prodat(karta, korzina);
        }
        else {
            // Правило 3: оптимизируем корзину под имеющийся баланс
            map<string, int> opt = optimizirovat_korzinu(korzina, buyer.get_schet());

            if (opt.empty()) {
                dobavit_soobshchenie(karta,
                    "Недостаточно средств для покупки даже одного товара из заказа.");
            }
            else {
                // Правило 4: предложение оптимальной корзины + лист ожидания + резерв
                double summa = primenit_skidku(summa_korziny(opt));

                ostringstream msg;
                msg << "Предлагаемая оптимальная корзина: ";
                bool first = true;
                for (const auto& pair : opt) {
                    if (!first) msg << ", ";
                    msg << pair.first << " x" << pair.second;
                    first = false;
                }
                msg << ". Сумма к оплате: " << summa
                    << ". Вы переведены в лист ожидания, товары зарезервированы.";
                dobavit_soobshchenie(karta, msg.str());

                for (const auto& pair : opt) {
                    tovary[pair.first]->zarezervirovat(pair.second);
                    rezerv[pair.first] += pair.second;
                }
            }
        }
    }
}

void Ploshchadka::vyvesti_vyruchku() const {
    cout << "\n___ Объём выручки по категориям ___\n";
    if (vyruchka_po_kategorii.empty()) {
        cout << "Продаж не состоялось.\n";
        return;
    }

    double obshchaya = 0;
    string max_kategoriya;
    double max_vyruchka = -1;
    for (const auto& pair : vyruchka_po_kategorii) {
        cout << "  " << pair.first << ": " << pair.second << "\n";
        obshchaya += pair.second;
        if (pair.second > max_vyruchka) {
            max_vyruchka = pair.second;
            max_kategoriya = pair.first;
        }
    }
    cout << "Общая выручка: " << obshchaya << "\n";
    cout << "Наибольшую выручку дала категория: " << max_kategoriya
         << " (" << max_vyruchka << ")\n";
}

void Ploshchadka::vyvesti_ochered_pokupatelej() const {
    cout << "\n___ Очередь покупателей, получивших сообщения ___\n";
    if (ochered_pokupatelej.empty()) {
        cout << "  (пусто)\n";
        return;
    }
    int i = 1;
    for (const string& karta : ochered_pokupatelej) {
        auto it = pokupateli.find(karta);
        cout << "  " << i++ << ". ";
        if (it != pokupateli.end())
            cout << it->second.get_fio() << " (карта " << karta << ")\n";
        else
            cout << "(карта " << karta << ")\n";
    }
}

void Ploshchadka::vyvesti_rezerv() const {
    cout << "\n___ Товары в резерве ___\n";
    if (rezerv.empty()) {
        cout << "  (пусто)\n";
        return;
    }
    for (const auto& pair : rezerv) {
        auto it = tovary.find(pair.first);
        if (it != tovary.end())
            cout << "  " << it->second->get_info()
                 << "; зарезервировано: " << pair.second << "\n";
    }
}

void Ploshchadka::otpravit_povtornye_napominaniya() const {
    cout << "\n___ Повторные напоминания ___\n";
    if (soobshcheniya.empty()) {
        cout << "  (нет сообщений)\n";
        return;
    }
    for (const auto& pair : soobshcheniya) {
        auto it = pokupateli.find(pair.first);
        string fio = (it != pokupateli.end()) ? it->second.get_fio() : "";
        cout << "  [Повторное] " << fio << " (карта " << pair.first << "): "
             << pair.second << "\n";
    }
}
