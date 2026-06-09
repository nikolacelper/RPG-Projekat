

# Timski projekat - Razvoj jednostavne RPG igrice

## Ideja

Cilj projekta je razvoj konzolne RPG igre (engl. Role-Playing Game) u programskom jeziku C, u timu od 5 studenata. Igra se odvija u lavirintu kroz koji igrač prolazi, bori se sa neprijateljima, skuplja predmete i pokušava da pronađe izlaz.

Projekat je osmišljen tako da možete početi da radite odmah, čak i pre nego što na predmetu obradimo sve potrebne koncepte. Svaka faza donosi novi sloj funkcionalnosti, a vi sami odlučujete ko šta radi i to dokumentujete.

## Opis igre

Igrač se kreće kroz lavirint prikazan u konzoli. Lavirint se sastoji od više povezanih soba/nivoa. U sobama mogu biti neprijatelji, predmeti i izlazi. Cilj igre je pronaći izlaz iz lavirinta.

Pri pokretanju igre, treba ispisati pravila igre, uputstvo za igru, i pitati korisnika da li želi da započne igru, na šta korisnik treba da odgovori sa [d/n]. Dodatno, moguće je ponuditi i odabir nivoa koji korisnik želi da igra.

### Prikaz mape

Mapa se prikazuje kao 2D mreža u konzoli, pri čemu je svako polje predstavljeno jednim karakterom.

Primer izgleda jedne sobe:

```text
###########
#@..#...\$>#
#...#.Ε...#
#....
#
###########

```

| Simbol | Značenje |
| --- | --- |
| # | Zid |
| . | Pod (slobodno polje) |
| @ | Igrač |
| E | Neprijatelj |
| $ | Predmet |
| < | Izlaz (prelaz na sledeći nivo) |

Po izboru članova tima, mogu se koristiti i biblioteke koje omogućavaju kompleksniji grafički prikaz, čime bi sam grafički interfejs bio zanimljiviji. Mape treba da budu unapred osmišljene i sačuvane u tekstualnim fajlovima iz kojih se učitavaju (npr. nivo1.txt, nivo2.txt, itd).

### Kretanje

Igrač unosi komandu za kretanje:

* W - gore
* S - dole
* a - levo
* d - desno
* i - otvori inventar
* q - izađi iz igre (obavezna potvrda sa [d/n])

Kretanje na polje zida nije dozvoljeno. Kretanje na polje neprijatelja pokreće borbu. Kretanje na polje predmeta automatski ga otvara i pokreće proceduru za dodavanje u inventar. Kretanje na izlaz završava nivo i igrač prelazi na sledeći nivo.

Bonus (dodatni zadatak): Posle svakog poteza igrača, i neprijatelji prisutni u sobi mogu nasumično da se kreću za po jedno polje. Oni takođe ne mogu da se kreću na polje zida, niti na polje gde je drugi neprijatelj. Mogu napasti igrača ukoliko dođu na njegovo polje.

### Borba

Kada igrač stane na polje neprijatelja (ili obrnuto), pokreće se borba. Borba se odvija po rundama:

1. Igrač bira akciju: napad, odbrana ili bekstvo;
2. Neprijatelj nasumično bira akciju;
3. Napad oduzima protivniku onoliko HP kolika je jačina napada napadača;
4. Ukoliko je izabrana akcija odbrana, šteta se umanjuje za jačinu odbrane igrača;
5. Bekstvo ima 50% šanse za uspeh;
6. Borba se završava kada jedan od učesnika dođe na 0 HP ili pobegne na susedno polje.

Igrač na početku ima 100 HP, i jačinu napada i odbrane 10 HP. Neprijatelji imaju različite vrednosti HP i jačine napada, odnosno odbrane. Pri početku borbe potrebno je ispisati ove parametre, kao i posle svake akcije.

### Inventar

Igrač može nositi do 7 predmeta, koji mogu biti:

* Lek - obnavlja HP (narodni lek +10 HP po potezu, vilinski lek +20 HP);
* Mač - povećava napad trajno spram bonusa mača (običan mač +5, vitezov mač +15, vatreni mač +25), ali otežava kretanje, pa tako pri nošenju boljih mačeva igrač gubi HP pri svakom potezu (sa običnim mačem ne gubi uopšte, sa vitezovim mačem gubi 5 HP, a sa vatrenim mačem gubi 10 HP po potezu);
* Štit - trajno povećava odbranu (drveni štit +5, gvozdeni štit +10, zmajski štit +15);
* Ključ - potreban za određene izlaze;
* Eliksir života - može da se iskoristi ukoliko igrač pogine u borbi.

Svaki put kada igrač pokupi novi predmet, treba proveriti da li ima mesta za njega u inventaru. Ako nema, treba pitati igrača šta želi da odbaci. Ako ima više vrsta mačeva ili štitova, treba ga pitati koji želi da koristi.

Na mestu predmeta mogu se nalaziti i misteriozni napici, koji menjaju jačine napada ili odbrane. Igrač u tom slučaju ne zna šta tačno dobija ili gubi, ali mora da odluči da li želi da popije napitak. Ako je u pitanju čarobni napitak, napad ili odbrana mu se pojačavaju za pronađeni bonus, u suprotnom, ako je naišao na otrov, smanjuju se.

### Statistike igrača (primer)

Nakon svakog poteza, treba ponovo iscrtati mapu sa novim stanjem, i ispisati statistike igrača, npr:
HP: [########..] 80/100
Napad: 15 Odbrana: 8
Inventar: [Lek] [Vitezov Mač] [Zlatni Ključ] [] [][][]

---

## Faze razvoja

### Faza 1 - Osnova

Šta treba uraditi:

* Otvoriti repozitorijum na GitHubu i pozvati sve kolege iz tima i profesoricu (ivanasch) kao saradnike.
* Postaviti opis zadatka u fajl ZADATAK.md.
* Nacrtati skicu lavirinta na papiru i dogovoriti se oko simbola i koordinatnog sistema. Ovo dokumentovati u README fajlu na GitHubu.
* Implementirati prikaz jedne hardkodovane sobe u konzoli koristeći printf, testirati preglednost odabranih simbola.
* Implementirati glavni meni igre pomoću switch:
* 1 - Nova igra
* 2 - Uputstvo
* 0 - Izlaz
* Opcije za sada samo ispisuju poruku tipa "Nova igra u razvoju" (tzv. stub).


* Implementirati logiku jednog poteza borbe (bez ponavljanja): igrač bira akciju, uzima se nasumična akcija protivnika, program ispisuje rezultat. Koristiti if/else ili switch.
* Nacrtati na papiru i dokumentovati dizajn inventara: kako će predmeti biti zapisani, kako se prikazuje korisniku.
* Implementirati logika za situaciju kada igrač stane na polje sa misterioznim napitkom, ispisati poruku "Pronašao/la si misteriozni napitak! Da li ćeš ga popiti? [d/n]". Koristiti if/else za obradu odgovora: ako prihvati, ispisati nasumični efekat (npr. "Napad +5"), ako odbije, ispisati "Prosuo si napitak. Njegovo dejstvo je bilo: Napad +5".

Napomena: U ovoj fazi igra još ne "radi" kao celina, to je normalno. Cilj je da postavite temelje, upoznate se sa GitHubom i naučite kako da organizujete kod u timu.

### Faza 2 - Kretanje i petlje

Šta treba uraditi:

* Implementirati crtanje mape dvostrukom for petljom kroz 2D matricu karaktera (char mapa [VISINA] [SIRINA]).
* Implementirati unos komande od igrača i pomeranje simbola @ po mapi (uz proveru da li je polje slobodno).
* Implementirati borbu u više rundi: petlja koja se ponavlja dok jedan od učesnika ne dođe na 0 HP ili ne uspe da pobegne.
* Organizovati inventar: char inventar [7] [20] ili niz celih brojeva koji predstavljaju tipove predmeta. Implementirati dodavanje i uklanjanje predmeta.
* Implementirati ispis inventara.
* Implementirati punu logiku uzimanja predmeta kada igrač stane na polje $:
* Proveriti da li je polje predmet ili napitak (if/else)
* Ako je predmet: proveriti da li ima mesta u inventaru (brojač < 7)
* Ako ima mesta: proveriti da li igrač već ima mač ili štit istog tipa ako ima, pitati "Već imaš mač. Želiš li da ga zameniš? [d/n]" Ako nema konflikta ili igrač potvrdi zamenu: dodati predmet u inventar.
* Ako nema mesta: ispisati "Inventar je pun! Šta želiš da odbaciš?", prikazati listu inventara, učitati izbor igrača, ukloniti odabrani predmet i dodati novi.


* Ako je napitak: ispisati poruku i pitati igrača da li želi da ga popije (logika iz Faze 1).


* Implementirati više soba kao niz mapa ili kao mapu koja prikazuje trenutnu sobu.
* Implementirati statistike igrača i njihovo ažuriranje tokom igre.
* Implementirati petlju koja se vrti dok igrač ne pobedi, ne izgubi ili ne izađe.

### Faza 3 - Funkcije

Šta treba uraditi:

* Podeliti kod u logičke funkcije. Svaka funkcija radi jednu stvar. Primeri: isrtaj_mapu, borba, prikazi_inventar, pomeri_igraca, itd.
* Ovo je faza u kojoj se sav dosadašnji kod "čisti" i reorganizuje. Koristiti je kao priliku da se ukloni duplirani kod.

### Faza 4 - Strukturni tipovi podataka i finalizacija

Šta treba uraditi:

* Uvesti struct za glavne entitete:

```c
struct Igrac {
    int x, y;
    int hp;
    int napad;
    int odbrana;
    char inventar [7] [20];
    int br_predmeta;
};

struct Neprijatelj {
    int x, y;
    int hp;
    int napad;
    int odbrana;
    char naziv [20];
};

```

* Prebaciti sve funkcije da koriste struct umesto zasebnih promenljivih.
* Implementirati čuvanje i učitavanje igre u tekstualni fajl (fprintf, fscanf).
* Dodati naslovni ekran, poruku pobede i poruku poraza.
* Testirati celu igru i ukloniti bugove.

---

## GitHub pravila

### Struktura repozitorijuma

```text
rpg-projekat/
├── README.md          <-- opis projekta, uputstvo za pokretanje
├── ZADATAK.md         <-- opis zadatka
├── CONTRIBUTORS.md    <-- ko je radio šta (ažurirati svake nedelje)
├── src/
│   └── igra.c
└── docs/
    └── skice/         <-- fotografije skica sa papira, dijagrami

```

### Grane (branches)

* main grana je uvek stabilna kod na njoj mora da se kompajlira bez grešaka.
* Svaki član tima radi na svojoj grani. Naziv grane: ime/kratki-opis, npr. marko/prikaz-mape ili ana/sistem-borbe.
* Zabranjeno je direktno pisati (push) na main granu.

### Pull requestovi (PR)

* Svaka promena koja ide na main mora proći kroz pull request.
* Pull request mora pregledati i odobriti najmanje 2 člana tima pre merge-a.
* Reviewer ne sme biti osoba koja je pisala kod.
* U opisu PR-a obavezno napisati: šta je dodato, šta je promenjeno, kako se testiralo.
* Ne merge-ovati sopstveni pull request.

### Commit poruke

Svaka commit poruka mora biti smislena i opisivati šta je urađeno. Koristiti srpski ili engleski dosledno kroz projekat.

* Dobro:
* "Dodat prikaz mape dvostrukom petljom"
* "Ispravljena greška pri kretanju na zid"
* "Implementirana borba kroz više rundi"


* Loše: "update", "fix", "aaa", "izmene"

### CONTRIBUTORS.md

Svake nedelje svaki član tima dodaje u ovaj fajl šta je radio u toj nedelji. Format:

```markdown
## Nedelja 2 (datum)
Marko: implementirao prikaz mape petljom, testirao kretanje
Ana: napisala logiku borbe, nacrtala dijagram stanja
Nikola: pregledao PR od Marka, ispravio bug u meniju

```

### Zabrane

* Ne brisati tuđi kod bez dogovora, uvek koristiti komentar ili otvoriti issue.

---

## Ocenjivanje

| Kriterijum | Opis | Bodovi |
| --- | --- | --- |
| Funkcionalnost | Igra se pokreće i može se igrati do kraja | 5 |
| Podela posla | CONTRIBUTORS.md jasno pokazuje doprinos svakog člana | 3 |
| Git pravila | Smislene commit poruke, PR-ovi, code review | 3 |
| Kvalitet koda | Čitljivost, komentari, bez duplikacija | 2 |
| Napredak po fazama | Vidljiva istorija razvoja kroz commit istoriju | 2 |
| Bonus | Bonusi iz teksta, više nivoa, pamćenje imena igrača i nivoa do kog je stigao u fajlu, ili nešto dodatno što sami smislite | 5 |

Projekat treba da bude gotov do 10.6. kada će biti predstavljen kolegama tokom predavanja.
