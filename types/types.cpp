// types.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include <string>

struct s {
    int brackPos[4];
    std::string coreType;
    std::string core;
    bool fun;
};

void findCoreType(struct s& str);

void findCoreType(struct s& str)
{
    int bracket = 0;
    int brackPosCnt = 0;
    for (auto c = str.coreType.begin() + str.brackPos[0]; c < str.coreType.begin() + str.brackPos[1]; c++) {
        if (*c == '(') {
            bracket++;
            if (bracket == 1) str.brackPos[brackPosCnt++] = c - str.coreType.begin();
        }
        else if (*c == ')') {
            bracket--;
            if (bracket == 0) str.brackPos[brackPosCnt++] = c - str.coreType.begin();
        }
        if (brackPosCnt == 5) {
            break;
        }
    }
    str.core = str.coreType.substr(str.brackPos[0], str.brackPos[1] - str.brackPos[0] + 1);
    if (brackPosCnt == 2) {
        str.brackPos[0]++;
        str.brackPos[2] = 0;
        str.brackPos[3] = 0;
        findCoreType(str);
    }
    else if (brackPosCnt == 4) {
        str.fun = true;
    }
    return;
}

int main() {
    std::string str = R"###(int (*(*(*((*foo)(int, char (*)[2])))[4])[4])[4])###";
    struct s in;
    in.brackPos[0] = 0;
    in.brackPos[1] = str.length();
    in.brackPos[2] = -1;
    in.brackPos[3] = -1;
    in.coreType = str;
    in.fun = false;

    findCoreType(in);

}

// Per eseguire il programma: CTRL+F5 oppure Debug > Avvia senza eseguire debug
// Per eseguire il debug del programma: F5 oppure Debug > Avvia debug

// Suggerimenti per iniziare: 
//   1. Usare la finestra Esplora soluzioni per aggiungere/gestire i file
//   2. Usare la finestra Team Explorer per connettersi al controllo del codice sorgente
//   3. Usare la finestra di output per visualizzare l'output di compilazione e altri messaggi
//   4. Usare la finestra Elenco errori per visualizzare gli errori
//   5. Passare a Progetto > Aggiungi nuovo elemento per creare nuovi file di codice oppure a Progetto > Aggiungi elemento esistente per aggiungere file di codice esistenti al progetto
//   6. Per aprire di nuovo questo progetto in futuro, passare a File > Apri > Progetto e selezionare il file con estensione sln
