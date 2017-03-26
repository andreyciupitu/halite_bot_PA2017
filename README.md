# halite_bot_PA2017
Halite Bot - PA 2017 Project

Echipa: 202
Membri: Stefan Adrian-Cosmin, 322CC
        State Nicolae, 323CC
        Cochilet Serban-Bogdan, 323CC
        Ciupitu Andrei-Valentin, 322CC - capitan

========== Mod de compilare ====================================================

Compilarea se face din linia de comanda, folosind Makefile-ul, sursele si
header-ele se afla in folderul src/, iar exacutabilul se afla in folderul curent.

========== Surse de inspiratie =================================================

Am pornit implementarea de la tutorialele de pe site-ul jocului si de forum:
https://halite.io/basics_improve_random.php
http://forums.halite.io/t/so-youve-improved-the-random-bot-now-what/482

========== Detalii de implementare =============================================

Implementarea efectiva a botului este facuta in clasa Player. Botul aplica
strategia urmatoare:
    - Casutele din interiorul terenului ocupat vor cauta cea mai apropiata
    granita, iar daca aceasta nu exista vor merge in mod implicit in directia
    SOUTH.
    - Casutele de la granita folosesc o abordare greedy pentru a ataca piesele
    neocupate. Astfel, o piesa va ataca de indata ce este posibil, iar daca
    exista mai multe casute ce pot fi atacate, atunci va fi aleasa piesa cu cel
    mai mare scor.
    - Daca o casuta de granita nu poate ataca, atunci ea va cauta o casuta de
    granita vecina cu care sa se combine, fara a face OVERFLOW. Pentru aceasta
    este tinuta o matrice cu puterile casutelor.

Pentru determinarea unei mutari sa va face in timp constant O(1) pentru casutele
de granita, iar pentru o casuta interioara se va face in O(height).

Complexitatea totala a algoritmului folosit va fi asadar O(width * (height ^ 2))

========== Credits =============================================================

App design(organizarea codului) - Andrei Ciupitu
Research & Strategie - Andrei Ciupitu & State Nicolae
Algoritmul utilizat - Toti
Implementare - Stefan Adrian & Serban Cochilet
