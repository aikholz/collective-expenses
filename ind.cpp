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

struct Tranzakcia {
    string otKogo;
    double summa;
    string komu;
};

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

string obrezat(const string& stroka) {
    string resultat = stroka;
    resultat.erase(0, resultat.find_first_not_of(" \t\n\r"));
    resultat.erase(resultat.find_last_not_of(" \t\n\r") + 1);
    return resultat;
}

double okruglit(double chislo) {
    return round(chislo * 10.0) / 10.0;
}

bool sravnitPoSumme(const pair<string, double>& a, const pair<string, double>& b) {
    return a.second > b.second;
}

int main() {
   
    ifstream vhodnoiFail("input.txt");
    
   
    if (!vhodnoiFail.is_open()) {
        cout << "eror" << endl;
        return 1;
    }
    
   
    string pervayaStroka;
    getline(vhodnoiFail, pervayaStroka);
    
   
    vector<string> slova = podelit(pervayaStroka, ' ');
    
  
    int kolvoUchastnikov = stoi(slova[0]);
    
   
    vector<string> imena;
    for (int i = 1; i <= kolvoUchastnikov && i < slova.size(); i++) {
        imena.push_back(slova[i]);
    }
    

    map<string, double> vseRashody;  
    map<string, double> dolg;         
    

    for (int i = 0; i < imena.size(); i++) {
        vseRashody[imena[i]] = 0.0;
        dolg[imena[i]] = 0.0;
    }
    
  
    string stroka;
    while (getline(vhodnoiFail, stroka)) {
        stroka = obrezat(stroka);
  
        if (stroka.empty()) {
            continue;
        }
        
        size_t poziciaDvoetochia = stroka.find(':');
        if (poziciaDvoetochia == string::npos) {
            continue;
        }
        
        string imyaPlatelshika = obrezat(stroka.substr(0, poziciaDvoetochia));
        
        string ostatok = obrezat(stroka.substr(poziciaDvoetochia + 1));
        
   
        vector<string> iskluchennie;
        double summaRashoda = 0.0;
        
        size_t poziciaSlesha = ostatok.find('/');
        
        if (poziciaSlesha != string::npos) {
            
            string strokaSummi = obrezat(ostatok.substr(0, poziciaSlesha));
            summaRashoda = stod(strokaSummi);
            
            string iskluchennieStroka = obrezat(ostatok.substr(poziciaSlesha + 1));
            iskluchennie = podelit(iskluchennieStroka, ',');
        } else {
           
            summaRashoda = stod(ostatok);
        }
        
        
        set<string> iskluchennieMnozhestvo;
        for (int i = 0; i < iskluchennie.size(); i++) {
            iskluchennieMnozhestvo.insert(iskluchennie[i]);
        }
        
        
        vector<string> uchastnikiTrati;
        for (int i = 0; i < imena.size(); i++) {
            string tekusheeImya = imena[i];
            
            if (iskluchennieMnozhestvo.find(tekusheeImya) == iskluchennieMnozhestvo.end()) {
                uchastnikiTrati.push_back(tekusheeImya);
            }
        }
      
        double dolyaNaOdnogo = summaRashoda / uchastnikiTrati.size();
    
        vseRashody[imyaPlatelshika] = vseRashody[imyaPlatelshika] + summaRashoda;
  
        for (int i = 0; i < uchastnikiTrati.size(); i++) {
            string imyaUchastnika = uchastnikiTrati[i];
            dolg[imyaUchastnika] = dolg[imyaUchastnika] + dolyaNaOdnogo;
        }
    }

    vhodnoiFail.close();
    
    ofstream vihodnoiFail("output.txt");

    if (!vihodnoiFail.is_open()) {
        cout << "error" << endl;
        return 1;
    }
    
    vihodnoiFail << fixed << setprecision(1);

    vihodnoiFail << "Rashody" << endl;
    for (int i = 0; i < imena.size(); i++) {
        string imya = imena[i];
        double rashod = okruglit(vseRashody[imya]);
        double norma = okruglit(dolg[imya]);
        vihodnoiFail << imya << " " << rashod << " " << norma << endl;
    }
    
    map<string, double> balans;
    for (int i = 0; i < imena.size(); i++) {
        string imya = imena[i];
        balans[imya] = vseRashody[imya] - dolg[imya];
    }
    
    vector<Tranzakcia> vseTranzakcii;
    
    // делим на должников и кредиторов
    vector<pair<string, double>> dolzhniki;    
    vector<pair<string, double>> kreditori;  
    
    for (int i = 0; i < imena.size(); i++) {
        string imya = imena[i];
        if (balans[imya] < -0.01) {
            dolzhniki.push_back(make_pair(imya, -balans[imya]));
        } else if (balans[imya] > 0.01) {
            kreditori.push_back(make_pair(imya, balans[imya]));
        }
    }
    
    sort(dolzhniki.begin(), dolzhniki.end(), sravnitPoSumme);
    sort(kreditori.begin(), kreditori.end(), sravnitPoSumme);
    
    int indexDolzhnika = 0;
    int indexKreditora = 0;
    
    while (indexDolzhnika < dolzhniki.size() && indexKreditora < kreditori.size()) {
        double summaPerevoda = min(dolzhniki[indexDolzhnika].second, 
                                   kreditori[indexKreditora].second);
        
        if (summaPerevoda > 0.01) {
        
            Tranzakcia novaya;
            novaya.otKogo = dolzhniki[indexDolzhnika].first;
            novaya.summa = okruglit(summaPerevoda);
            novaya.komu = kreditori[indexKreditora].first;
            vseTranzakcii.push_back(novaya);
        }
        
        dolzhniki[indexDolzhnika].second = dolzhniki[indexDolzhnika].second - summaPerevoda;
        kreditori[indexKreditora].second = kreditori[indexKreditora].second - summaPerevoda;
    
        if (dolzhniki[indexDolzhnika].second < 0.01) {
            indexDolzhnika = indexDolzhnika + 1;
        }
        if (kreditori[indexKreditora].second < 0.01) {
            indexKreditora = indexKreditora + 1;
        }
    }
    
    vihodnoiFail << endl;
    vihodnoiFail << "Tranzakcii" << endl;
    for (int i = 0; i < vseTranzakcii.size(); i++) {
        vihodnoiFail << vseTranzakcii[i].otKogo << " " 
                     << vseTranzakcii[i].summa << " " 
                     << vseTranzakcii[i].komu << endl;
    }
    
    vihodnoiFail.close();
    cout << "Gotovo! t" << endl;
    
    return 0;
}