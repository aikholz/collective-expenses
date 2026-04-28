#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

// структура для хранения одной транзакции
struct Tranzakcia {
    string otKogo;
    double summa;
    string komu;
};

// функция делит строку на части по разделителю
vector<string> podelit(const string& stroka, char razdelitel) {
    vector<string> chasti;
    stringstream potok(stroka);
    string chast;
    
    while (getline(potok, chast, razdelitel)) {
        // убираем пробелы в начале
        chast.erase(0, chast.find_first_not_of(" \t"));
        // убираем пробелы в конце
        chast.erase(chast.find_last_not_of(" \t") + 1);
        if (!chast.empty()) {
            chasti.push_back(chast);
        }
    }
    
    return chasti;
}

// функция убирает пробелы в начале и конце строки
string obrezat(const string& stroka) {
    string resultat = stroka;
    resultat.erase(0, resultat.find_first_not_of(" \t\n\r"));
    resultat.erase(resultat.find_last_not_of(" \t\n\r") + 1);
    return resultat;
}

// функция для округления до одного знака после запятой
double okruglit(double chislo) {
    return round(chislo * 10.0) / 10.0;
}

// функция для сравнения двух пар по сумме (для сортировки)
bool sravnitPoSumme(const pair<string, double>& a, const pair<string, double>& b) {
    return a.second > b.second;
}

int main() {
    // открываем входной файл
    ifstream vhodnoiFail("input.txt");
    
    // проверяем открылся ли файл
    if (!vhodnoiFail.is_open()) {
        cout << "eror" << endl;
        return 1;
    }
    
    // читаем первую строку с количеством и именами
    string pervayaStroka;
    getline(vhodnoiFail, pervayaStroka);
    
    // разбиваем первую строку на слова
    vector<string> slova = podelit(pervayaStroka, ' ');
    
    // первое слово это количество участников
    int kolvoUchastnikov = stoi(slova[0]);
    
    // остальные слова это имена участников
    vector<string> imena;
    for (int i = 1; i <= kolvoUchastnikov && i < slova.size(); i++) {
        imena.push_back(slova[i]);
    }
    
    // словари для хранения расходов
    map<string, double> vseRashody;    // сколько каждый потратил на самом деле
    map<string, double> dolg;          // сколько каждый должен (норма)
    
    // изначально у всех нули
    for (int i = 0; i < imena.size(); i++) {
        vseRashody[imena[i]] = 0.0;
        dolg[imena[i]] = 0.0;
    }
    
    // читаем строки с расходами
    string stroka;
    while (getline(vhodnoiFail, stroka)) {
        // убираем пробелы по краям
        stroka = obrezat(stroka);
        
        // если строка пустая - пропускаем
        if (stroka.empty()) {
            continue;
        }
        
        // ищем двоеточие
        size_t poziciaDvoetochia = stroka.find(':');
        if (poziciaDvoetochia == string::npos) {
            continue;
        }
        
        // имя того кто платил (всё что до двоеточия)
        string imyaPlatelshika = obrezat(stroka.substr(0, poziciaDvoetochia));
        
        // всё что после двоеточия
        string ostatok = obrezat(stroka.substr(poziciaDvoetochia + 1));
        
        // список исключённых и сумма
        vector<string> iskluchennie;
        double summaRashoda = 0.0;
        
        // ищем слэш (если есть исключённые)
        size_t poziciaSlesha = ostatok.find('/');
        
        if (poziciaSlesha != string::npos) {
            // есть слэш значит есть исключённые
            string strokaSummi = obrezat(ostatok.substr(0, poziciaSlesha));
            summaRashoda = stod(strokaSummi);
            
            string iskluchennieStroka = obrezat(ostatok.substr(poziciaSlesha + 1));
            iskluchennie = podelit(iskluchennieStroka, ',');
        } else {
            // нет слэша значит исключённых нет
            summaRashoda = stod(ostatok);
        }
        
        // создаём множество исключённых для быстрого поиска
        set<string> iskluchennieMnozhestvo;
        for (int i = 0; i < iskluchennie.size(); i++) {
            iskluchennieMnozhestvo.insert(iskluchennie[i]);
        }
        
        // определяем кто участвует в трате
        vector<string> uchastnikiTrati;
        for (int i = 0; i < imena.size(); i++) {
            string tekusheeImya = imena[i];
            // если имени нет в множестве исключённых
            if (iskluchennieMnozhestvo.find(tekusheeImya) == iskluchennieMnozhestvo.end()) {
                uchastnikiTrati.push_back(tekusheeImya);
            }
        }
        
        // считаем долю на каждого участника
        double dolyaNaOdnogo = summaRashoda / uchastnikiTrati.size();
        
        // записываем что плательщик потратил всю сумму
        vseRashody[imyaPlatelshika] = vseRashody[imyaPlatelshika] + summaRashoda;
        
        // распределяем долги
        for (int i = 0; i < uchastnikiTrati.size(); i++) {
            string imyaUchastnika = uchastnikiTrati[i];
            dolg[imyaUchastnika] = dolg[imyaUchastnika] + dolyaNaOdnogo;
        }
    }
    
    // закрываем входной файл
    vhodnoiFail.close();
    
    // открываем выходной файл
    ofstream vihodnoiFail("output.txt");
    
    // проверяем открылся ли файл
    if (!vihodnoiFail.is_open()) {
        cout << "error" << endl;
        return 1;
    }
    
    // настраиваем вывод чисел
    vihodnoiFail << fixed << setprecision(1);
    
    // выводим расходы и нормы каждого участника
    vihodnoiFail << "Rashody" << endl;
    for (int i = 0; i < imena.size(); i++) {
        string imya = imena[i];
        double rashod = okruglit(vseRashody[imya]);
        double norma = okruglit(dolg[imya]);
        vihodnoiFail << imya << " " << rashod << " " << norma << endl;
    }
    
    // считаем баланс каждого участника
    map<string, double> balans;
    for (int i = 0; i < imena.size(); i++) {
        string imya = imena[i];
        balans[imya] = vseRashody[imya] - dolg[imya];
    }
    
    // список всех транзакций
    vector<Tranzakcia> vseTranzakcii;
    
    // делим на должников и кредиторов
    vector<pair<string, double>> dolzhniki;    // те кто должны
    vector<pair<string, double>> kreditori;     // те кому должны
    
    for (int i = 0; i < imena.size(); i++) {
        string imya = imena[i];
        if (balans[imya] < -0.01) {
            // отрицательный баланс значит должен
            dolzhniki.push_back(make_pair(imya, -balans[imya]));
        } else if (balans[imya] > 0.01) {
            // положительный баланс значит переплатил
            kreditori.push_back(make_pair(imya, balans[imya]));
        }
    }
    
    // сортируем по убыванию суммы
    sort(dolzhniki.begin(), dolzhniki.end(), sravnitPoSumme);
    sort(kreditori.begin(), kreditori.end(), sravnitPoSumme);
    
    // строим транзакции
    int indexDolzhnika = 0;
    int indexKreditora = 0;
    
    while (indexDolzhnika < dolzhniki.size() && indexKreditora < kreditori.size()) {
        double summaPerevoda = min(dolzhniki[indexDolzhnika].second, 
                                   kreditori[indexKreditora].second);
        
        if (summaPerevoda > 0.01) {
            // создаём новую транзакцию
            Tranzakcia novaya;
            novaya.otKogo = dolzhniki[indexDolzhnika].first;
            novaya.summa = okruglit(summaPerevoda);
            novaya.komu = kreditori[indexKreditora].first;
            vseTranzakcii.push_back(novaya);
        }
        
        // уменьшаем долги
        dolzhniki[indexDolzhnika].second = dolzhniki[indexDolzhnika].second - summaPerevoda;
        kreditori[indexKreditora].second = kreditori[indexKreditora].second - summaPerevoda;
        
        // если долг погашен переходим к следующему
        if (dolzhniki[indexDolzhnika].second < 0.01) {
            indexDolzhnika = indexDolzhnika + 1;
        }
        if (kreditori[indexKreditora].second < 0.01) {
            indexKreditora = indexKreditora + 1;
        }
    }
    
    // выводим транзакции
    vihodnoiFail << endl;
    vihodnoiFail << "Tranzakcii" << endl;
    for (int i = 0; i < vseTranzakcii.size(); i++) {
        vihodnoiFail << vseTranzakcii[i].otKogo << " " 
                     << vseTranzakcii[i].summa << " " 
                     << vseTranzakcii[i].komu << endl;
    }
    
    // закрываем выходной файл
    vihodnoiFail.close();
    
    cout << "Gotovo! t" << endl;
    
    return 0;
}