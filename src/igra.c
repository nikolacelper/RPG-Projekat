#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define MAX_ROWS 10
#define MAX_COLS 100
#define MAX_INVENTAR 7
#define UKUPNO_NIVOA 5
#define BORBA_POBJEDA 1
#define BORBA_BEKSTVO_IGRACA 2
#define BORBA_BEKSTVO_NEPRIJATELJA 3

void sacuvajIgru();
void pomjeriNeprijatelje();
int ucitajSacuvanuIgru();
void novaIgra(int ucitaj_save);

typedef enum {
    LEK_NARODNI,
    LEK_VILINSKI,
    MAC_OBICAN,
    MAC_VITEZOV,
    MAC_VATRENI,
    STIT_DRVENI,
    STIT_GVOZDENI,
    STIT_ZMAJSKI,
    KLJUC,
    ELIKSIR_ZIVOTA
} TipPredmeta;

typedef struct {
    char naziv[40];
    TipPredmeta tip;
    int bonus;
} Predmet;

typedef struct {
    int hp;
    int max_hp;

    int osnovni_napad;
    int osnovna_odbrana;

    int napad;
    int odbrana;

    int bonus_maca;
    int kazna_maca_po_potezu;
    int bonus_stita;

    Predmet inventar[MAX_INVENTAR];
    int broj_predmeta;
} Igrac;

typedef struct {
    char naziv[40];
    int hp;
    int napad;
    int odbrana;
} Neprijatelj;

int pokreniBorbu(Neprijatelj *vanjski);

char map[MAX_ROWS][MAX_COLS];

int rows = 0;
int px = 0;
int py = 0;
int trenutni_nivo = 1;

/* NOVO - linija 70: brojac ubijenih neprijatelja tokom cijele partije */
int ukupno_ubijenih = 0;

/* Tabela koja pamti cijelog neprijatelja na mapi po poziciji [y][x].
   aktivan[y][x] == 0 znaci da na toj celiji nema neprijatelja. */
Neprijatelj neprijatelj_tabela[MAX_ROWS][MAX_COLS];
int neprijatelj_aktivan[MAX_ROWS][MAX_COLS];

/* Pozicija garantovanog kljuca na zadnjem nivou.
   -1 znaci da nema garantovanog kljuca na ovom nivou. */
int garantovani_kljuc_y = -1;
int garantovani_kljuc_x = -1;

Igrac heroj = {
 100, 100,
    10, 10,
    10, 10,
    0, 0, 0,
    {{0}}, 0
};

void pauza() {
    printf("\nPritisni bilo koji taster...");
    _getch();

}

void izracunajStatistiku() {
    heroj.napad = heroj.osnovni_napad + heroj.bonus_maca;
    heroj.odbrana = heroj.osnovna_odbrana + heroj.bonus_stita;

    if (heroj.napad < 1) {
        heroj.napad = 1;
    }

    if (heroj.odbrana < 0) {
        heroj.odbrana = 0;
    }

}

void iscrtajHPBar(int hp, int max_hp) {
    int popunjeno = (hp * 10) / max_hp;

    if (popunjeno < 0) {
        popunjeno = 0;
    }

    if (popunjeno > 10) {
        popunjeno = 10;
    }

    printf("HP: [");

    for (int i = 0; i < 10; i++) {
        if (i < popunjeno) {
            printf("#");
        }
        else {
            printf(".");
        }
    }

    printf("] %d/%d\n", hp, max_hp);

}

void prikaziKratakInventar() {

}

void ukloniPredmet(int indeks) {

}

int pronadjiEliksir() {

}

int imaKljuc() {

}

int upotrebiEliksirAkoPostoji() {

}

void napraviPredmet(Predmet *p, TipPredmeta tip) {
  p->tip = tip;

    if (tip == LEK_NARODNI) {
        strcpy(p->naziv, "Narodni lek");
        p->bonus = 10;
    }
    else if (tip == LEK_VILINSKI) {
        strcpy(p->naziv, "Vilinski lek");
        p->bonus = 20;
    }
    else if (tip == MAC_OBICAN) {
        strcpy(p->naziv, "Obican mac");
        p->bonus = 5;
    }
    else if (tip == MAC_VITEZOV) {
        strcpy(p->naziv, "Vitezov mac");
        p->bonus = 15;
    }
    else if (tip == MAC_VATRENI) {
        strcpy(p->naziv, "Vatreni mac");
        p->bonus = 25;
    }
    else if (tip == STIT_DRVENI) {
        strcpy(p->naziv, "Drveni stit");
        p->bonus = 5;
    }
    else if (tip == STIT_GVOZDENI) {
        strcpy(p->naziv, "Gvozdeni stit");
        p->bonus = 10;
    }
    else if (tip == STIT_ZMAJSKI) {
        strcpy(p->naziv, "Zmajski stit");
        p->bonus = 15;
    }
    else if (tip == KLJUC) {
        strcpy(p->naziv, "Kljuc");
        p->bonus = 0;
    }
    else if (tip == ELIKSIR_ZIVOTA) {
        strcpy(p->naziv, "Eliksir zivota");
        p->bonus = 0;
    }
}

Predmet generisiNasumicanPredmet() {

}

void opremiMac(Predmet p) {

}

void opremiStit(Predmet p) {

}

void pitajZaOpremanje(Predmet p) {

}

void dodajPredmet(Predmet p) {

}

void prikaziInventar() {

}

void misteriozniNapitak() {

}

Neprijatelj generisiNeprijatelja() {

}

/* NOVO - linija 546: pomocna funkcija za ispis zavrsnog ekrana */
void ispisiZavrsniEkran(int pobjeda) {

}

int pokreniBorbu(Neprijatelj *vanjski) {

}

int ucitajNivo(int broj_nivoa) {
 char ime_fajla[30];

    sprintf(ime_fajla, "nivo%d.txt", broj_nivoa);

    FILE *f = fopen(ime_fajla, "r");

    if (f == NULL) {
        printf("Greska pri otvaranju fajla: %s\n", ime_fajla);
        printf("Fajl mora biti u istom folderu gdje se pokrece .exe.\n");
        return 0;
    }

    rows = 0;

    while (rows < MAX_ROWS && fgets(map[rows], MAX_COLS, f)) {
        for (int i = 0; i < MAX_COLS; i++) {
            if (map[rows][i] == '\n' || map[rows][i] == '\r') {
                map[rows][i] = '\0';
            }
        }

        rows++;
    }

    fclose(f);

    int pronadjen_igrac = 0;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; map[y][x] != '\0'; x++) {
            if (map[y][x] == '@') {
                px = x;
                py = y;
                pronadjen_igrac = 1;
            }
        }
    }

    if (!pronadjen_igrac) {
        printf("Greska: u fajlu %s ne postoji znak @ za igraca.\n", ime_fajla);
        return 0;
    }

    for (int y = 0; y < MAX_ROWS; y++)
        for (int x = 0; x < MAX_COLS; x++)
            neprijatelj_aktivan[y][x] = 0;

    garantovani_kljuc_y = -1;
    garantovani_kljuc_x = -1;

    int ima_zakljucana_vrata = 0;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; map[y][x] != '\0'; x++) {
            if (map[y][x] == 'L') {
                ima_zakljucana_vrata = 1;
            }
        }
    }

    if (ima_zakljucana_vrata) {
        int kutija_y[MAX_ROWS * MAX_COLS];
        int kutija_x[MAX_ROWS * MAX_COLS];
        int broj_kutija = 0;

        for (int y = 0; y < rows; y++) {
            for (int x = 0; map[y][x] != '\0'; x++) {
                if (map[y][x] == '$') {
                    kutija_y[broj_kutija] = y;
                    kutija_x[broj_kutija] = x;
                    broj_kutija++;
                }
            }
        }

        if (broj_kutija > 0) {
            int izbor = rand() % broj_kutija;
        
            garantovani_kljuc_y = kutija_y[izbor];
            garantovani_kljuc_x = kutija_x[izbor];
        }
    }

    trenutni_nivo = broj_nivoa;

    return 1;

    
}

void postaviBoju(int boja) {

     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), boja);

}

void crtajPolje(char polje) {
    if (polje == '#') {
        postaviBoju(8);
        printf("%c", 219);
    }
    else if (polje == '@') {
        postaviBoju(10);
        printf("@");
    }
    else if (polje == 'E') {
        postaviBoju(12);
        printf("E");
    }
    else if (polje == '$') {
        postaviBoju(14);
        printf("$");
    }
    else if (polje == '?') {
        postaviBoju(13);
        printf("?");
    }
    else if (polje == '>') {
        postaviBoju(11);
        printf(">");
    }
    else if (polje == 'L') {
        postaviBoju(9);
        printf("L");
    }
    else {
        postaviBoju(7);
        printf(" ");
    }

    postaviBoju(7);

}

void crtaj() {
system("cls");

    printf("NIVO %d/%d\n", trenutni_nivo, UKUPNO_NIVOA);

    iscrtajHPBar(heroj.hp, heroj.max_hp);

    printf("Napad: %d  Odbrana: %d\n", heroj.napad, heroj.odbrana);
    printf("Kazna maca po potezu: %d HP\n", heroj.kazna_maca_po_potezu);

    prikaziKratakInventar();

    printf("----------------------------------\n");

    for (int i = 0; i < rows; i++) {
    for (int j = 0; map[i][j] != '\0'; j++) {
        crtajPolje(map[i][j]);
    }
    printf("\n");
}

    printf("----------------------------------\n");
    printf("WASD - kretanje | I - inventar | Q - izlaz\n");
    printf("# zid | E neprijatelj | $ predmet | ? napitak | > izlaz | L zakljucan izlaz\n");
}

void provjeriKaznuMaca() {

}

void predjiNaSljedeciNivo() {

}

void pomjeriNeprijatelje() {

}

void novaIgra(int ucitaj_save) {
/* Ako je izabran nastavak sacuvane igre, pokusavamo ucitati save.dat */
if (ucitaj_save) {
    if (!ucitajSacuvanuIgru()) {
        printf("\nNije moguce ucitati sacuvanu igru.");
        pauza();
        return;
    }
}
else {
    /* Ako je nova igra, resetujemo osnovne parametre igraca */
    heroj.hp = 100;
    heroj.max_hp = 100;

    heroj.osnovni_napad = 10;
    heroj.osnovna_odbrana = 10;

    heroj.napad = 10;
    heroj.odbrana = 10;

    /* Reset opreme: nema maca, nema kazne, nema stita */
    heroj.bonus_maca = 0;
    heroj.kazna_maca_po_potezu = 0;
    heroj.bonus_stita = 0;

    /* Prazan inventar na pocetku nove igre */
    heroj.broj_predmeta = 0;

    /* Reset broja ubijenih neprijatelja */
    ukupno_ubijenih = 0;

    /* Racuna konacni napad i odbranu na osnovu osnovnih vrijednosti i opreme */
    izracunajStatistiku();

    /* Ucitava prvi nivo iz fajla nivo1.txt */
    if (!ucitajNivo(1)) {
        pauza();
        return;
    }
}

/* Glavna petlja igre - vrti se dok igrac ne izadje ili ne zavrsi igru */
while (1) {
    /* Iscrtavanje mape, HP-a, inventara i ostalih podataka */
    crtaj();

    /* Citanje tastera bez potrebe za Enter */
    char c = _getch();

    /* Obrada izlaska iz igre */
    if (c == 'q' || c == 'Q') {
        printf("\nStvarno zelis da izadjes iz igre? (d/n): ");

        char provera = _getch();

        /* Ako igrac potvrdi izlaz, igra se cuva i izlazi se iz petlje */
        if (provera == 'd' || provera == 'D') {
            sacuvajIgru();
            pauza();
            break;
        }
        else {
            continue;
        }
    }

    /* Otvaranje inventara */
    if (c == 'i' || c == 'I') {
        prikaziInventar();
        continue;
    }

    /* Pocetna nova pozicija je trenutna pozicija igraca */
    int nx = px;
    int ny = py;

    /* Odredjivanje nove pozicije na osnovu WASD tastera */
    if (c == 'w' || c == 'W') {
        ny--;
    }
    else if (c == 's' || c == 'S') {
        ny++;
    }
    else if (c == 'a' || c == 'A') {
        nx--;
    }
    else if (c == 'd' || c == 'D') {
        nx++;
    }
    else {
        /* Ako nije pritisnut validan taster za kretanje, ignorise se */
        continue;
    }

    /* Provjera da li nova pozicija izlazi van granica mape */
    if (ny < 0 || ny >= rows || nx < 0 || nx >= strlen(map[ny])) {
        continue;
    }

    /* Uzimamo znak sa polja na koje igrac pokusava da stane */
    char polje = map[ny][nx];

    /* Zid - igrac ne moze proci */
    if (polje == '#') {
        continue;
    }

    /* Ako je na polju neprijatelj, pokrece se borba */
    if (polje == 'E') {
        /* Ako neprijatelj jos nema svoje sacuvane podatke, generise se */
        if (!neprijatelj_aktivan[ny][nx]) {
            neprijatelj_tabela[ny][nx] = generisiNeprijatelja();
            neprijatelj_aktivan[ny][nx] = 1;
        }

        /* Pokretanje borbe sa neprijateljem na toj poziciji */
        int rezultat = pokreniBorbu(&neprijatelj_tabela[ny][nx]);

        /* Ako je igrac pobijedio, neprijatelj se uklanja sa mape */
        if (rezultat == BORBA_POBJEDA) {
            neprijatelj_aktivan[ny][nx] = 0;
            map[ny][nx] = '.';

            /* Igrac prelazi na polje gdje je bio neprijatelj */
            map[py][px] = '.';
            px = nx;
            py = ny;
            map[py][px] = '@';

            /* Poslije poteza se provjerava kazna maca i pomjeraju neprijatelji */
            provjeriKaznuMaca();
            pomjeriNeprijatelje();
        }
        else if (rezultat == BORBA_BEKSTVO_NEPRIJATELJA) {
            /* Ako neprijatelj pobjegne, trazimo slobodno susjedno polje */
            int dx[] = {0, 0, 1, -1};
            int dy[] = {-1, 1, 0, 0};

            /* Mijesanje smjerova da bjekstvo bude nasumicno */
            for (int k = 3; k > 0; k--) {
                int j = rand() % (k + 1);
                int tmp;

                tmp = dx[k];
                dx[k] = dx[j];
                dx[j] = tmp;

                tmp = dy[k];
                dy[k] = dy[j];
                dy[j] = tmp;
            }

            int premjesten = 0;

            /* Pokusaj premjestanja neprijatelja na jedno od 4 susjedna polja */
            for (int k = 0; k < 4; k++) {
                int enx = nx + dx[k];
                int eny = ny + dy[k];

                /* Provjera granica mape */
                if (eny < 0 || eny >= rows) continue;
                if (enx < 0 || enx >= (int)strlen(map[eny])) continue;

                /* Neprijatelj moze pobjeci samo na prazno polje */
                if (map[eny][enx] != '.') continue;

                /* Prenosimo podatke neprijatelja na novu poziciju */
                neprijatelj_tabela[eny][enx] = neprijatelj_tabela[ny][nx];
                neprijatelj_aktivan[eny][enx] = 1;
                neprijatelj_aktivan[ny][nx] = 0;

                /* Azuriranje mape */
                map[ny][nx] = '.';
                map[eny][enx] = 'E';

                premjesten = 1;
                break;
            }

            /* Ako nema slobodnog polja, neprijatelj ostaje gdje jeste */
            if (!premjesten) { }

            /* Igrac ulazi na polje gdje je neprijatelj bio */
            map[py][px] = '.';
            px = nx;
            py = ny;
            map[py][px] = '@';

            provjeriKaznuMaca();
            pomjeriNeprijatelje();
        }
        else if (rezultat == BORBA_BEKSTVO_IGRACA) {
            /* Ako igrac pobjegne, ne mijenja se pozicija */
            continue;
        }
    }
    else if (polje == '$') {
        /* Polje sa predmetom */
        Predmet novi_predmet;

        /* Ako je ovo garantovana kutija sa kljucem, pravi se kljuc */
        if (ny == garantovani_kljuc_y && nx == garantovani_kljuc_x) {
            napraviPredmet(&novi_predmet, KLJUC);
            garantovani_kljuc_y = -1;
            garantovani_kljuc_x = -1;
        }
        else {
            /* U suprotnom se generise nasumican predmet */
            novi_predmet = generisiNasumicanPredmet();
        }

        /* Dodavanje predmeta u inventar */
        dodajPredmet(novi_predmet);

        /* Kutija nestaje sa mape */
        map[ny][nx] = '.';

        /* Igrac prelazi na polje predmeta */
        map[py][px] = '.';
        px = nx;
        py = ny;
        map[py][px] = '@';

        provjeriKaznuMaca();
        pomjeriNeprijatelje();
    }
    else if (polje == '?') {
        /* Misteriozni napitak - moze povecati ili smanjiti napad/odbranu */
        misteriozniNapitak();

        /* Napitak se uklanja sa mape */
        map[ny][nx] = '.';

        /* Igrac prelazi na to polje */
        map[py][px] = '.';
        px = nx;
        py = ny;
        map[py][px] = '@';

        provjeriKaznuMaca();
        pomjeriNeprijatelje();
    }
    else if (polje == 'L') {
        /* Zakljucan izlaz - potreban je kljuc */
        int indeks_kljuca = imaKljuc();

        if (indeks_kljuca == -1) {
            printf("\nOvaj izlaz je zakljucan. Potreban ti je kljuc.");
            pauza();
            continue;
        }

        /* Ako igrac ima kljuc, uklanja se iz inventara i prelazi se nivo */
        printf("\nOtkljucao si izlaz pomocu kljuca!");
        ukloniPredmet(indeks_kljuca);
        pauza();

        predjiNaSljedeciNivo();
        continue;
    }
    else if (polje == '>') {
        /*
           Obican izlaz.
           Na zadnjem nivou dodatno trazimo kljuc da bi igrac zavrsio igru.
        */
        if (trenutni_nivo == UKUPNO_NIVOA && imaKljuc() == -1) {
            printf("\nOva vrata su zakljucana. Potreban ti je kljuc da izadjes!");
            pauza();
            continue;
        }

        /* Ako je zadnji nivo i igrac ima kljuc, kljuc se trosi */
        if (trenutni_nivo == UKUPNO_NIVOA) {
            ukloniPredmet(imaKljuc());
        }

        predjiNaSljedeciNivo();
        continue;
    }
    else {
        /* Obicno prazno polje - igrac se samo pomjera */
        map[py][px] = '.';
        px = nx;
        py = ny;
        map[py][px] = '@';

        provjeriKaznuMaca();
        pomjeriNeprijatelje();
    }
}
}

void uputstvo() {

    system("cls");

    printf("=== UPUTSTVO ===\n\n");

    printf("Cilj igre je da prodjes kroz 5 nivoa lavirinta.\n\n");

    printf("Kontrole:\n");
    printf("W - gore\n");
    printf("A - lijevo\n");
    printf("S - dole\n");
    printf("D - desno\n");
    printf("I - inventar\n");
    printf("Q - izlaz iz igre\n\n");

    printf("Simboli:\n");
    printf("@ - igrac\n");
    printf("# - zid\n");
    printf(". - prazan prostor\n");
    printf("E - neprijatelj\n");
    printf("$ - predmet\n");
    printf("? - misteriozni napitak\n");
    printf("> - izlaz\n");
    printf("L - zakljucan izlaz, potreban je kljuc\n\n");

    printf("Borba:\n");
    printf("N - napad\n");
    printf("O - odbrana\n");
    printf("B - bekstvo, ima 50%% sanse za uspeh\n\n");

    pauza();
}


void sacuvajIgru() {

}

int ucitajSacuvanuIgru() {

}

void meni() {
    while (1) {
        system("cls");

        printf("=== RPG LAVIRINT ===\n\n");

        // Provera da li postoji save fajl i pravilno zatvaranje fajla
        FILE *f = fopen("save.dat", "r");
        int ima_save = (f != NULL);
        if (ima_save) {
            fclose(f); 
        }

        // Prikaz menija
        if (ima_save) {
            printf("1 - Nova igra\n");
            printf("2 - Nastavi sacuvanu igru\n");
            printf("3 - Uputstvo\n");
            printf("0 - Izlaz\n\n");
        } else {
            printf("1 - Nova igra\n");
            printf("2 - Uputstvo\n");
            printf("0 - Izlaz\n\n");
        }

        printf("Izbor: ");
        char izbor = _getch();

        // Glavna logika menija preko switch-a
        switch (izbor) {
            case '1':
                if (ima_save) {
                    remove("save.dat");
                }
                novaIgra(0);
                break;

            case '2':
                if (ima_save) {
                    novaIgra(1); // Ako ima save, 2 je "Nastavi"
                } else {
                    uputstvo();  // Ako nema save, 2 je "Uputstvo"
                }
                break;

            case '3':
                if (ima_save) {
                    uputstvo(); // Opcija 3 postoji samo ako ima save fajla
                } else {
                    printf("\nPogresan izbor.");
                    pauza();
                }
                break;

            case '0':
                return; // Izlaz iz funkcije i prekid while petlje

            default:
                printf("\nPogresan izbor.");
                pauza();
                break;
        }
    }
}

int main() {

}
