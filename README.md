<Copyright Alexandru OLteanu, grupa 322CA, alexandruolteanu2001@gmail.com>

    Tema mi s-a parut foarte interesanta, ideea de simulare a unei asa zise 
platforme folosind request-uri HTTP de tip GET, POST si DELETE a fost destul 
de usor de implementat dar pot spune ca ne-a familiarizat destul de bine 
cu conceptul. Am ales sa rezolv cerintele in C++, alegere facuta din motivul
lucrului mai usor cu sirurile de caractere comparativ cu C. Am folosit
biblioteca json recomandata in documentatie, a fost destul de usor de inteles
cum se foloseste si overall a fost foarte ok.
    Arhiva cuprinde fisierele helpers.cpp, helpers.h, buffer.cpp si buffer.h 
preluate din laborator ce realizeaza trimiterea unui request odata construit catre 
server si primirea rezultatului din partea acestuia. Aceste fisiere nu au fost 
modificate de catre mine. Urmatoarele fisiere sunt cele de request care erau putin 
mai complicate si incomplete, astfel am modificat functiile folosind string-uri, codul
fiind acum mult mai lizibil si usor de inteles. Am adaugat de asemenea partea de 
Autorizare pentru token-ul Jwt si am creat functia pentru DELETE. O ultima modificare
adusa este eliminarea pointer-ului dublu pentru cookie si token, deoarece in solutia
mea am trimis direct mesajul construit sub forma unui obiect formatat json.
    In cadrul clientului am implementat functiile necesare pentru exit, register, login,
enter_library, get_books, get_book, add_book, delete_book si logout. Detalierea pentru
aceste functii se gaseste in cod, insa in mare parte acestea au fost asemanatoare. In
prima faza trebuia deschisa conexiunea catre server, apoi verificate ca si comenzi valide
fiecare dintre ele. Fiecare functie a urmat ideea de construire a mesajului trimis sub forma 
unui string, unele ofereau promt pentru interactiunea cu userul. In final, dupa trimiterea
request-ului catre server, raspunsul era primit, acesta fiind tratat in functie de erori si
de succes.
    Solutia a fost conceputa sa accepte usor modificari viitoare, astfel am construit structuri
pentru adresa url a componentelor serverului, functii de adaugare de comenzi, etc. De asemenea, 
pentru testarea temei consider important si verificarea raspunsului de la server, astfel inainte
de int main() {} am adaugat o variabila care pusa pe true ofera afisarea mesajului trimis de
server in urma fiecarei comenzi. Pe langa raspunsul serverului am tratat si eu diverse erori
precum incercarea de logare consecutiva fara un logout intre cele doua comenzi, etc.
    In final, o tema faina si nu foarte dificil de rezolvat!