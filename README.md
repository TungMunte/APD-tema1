\*/
Funcția getNthRoot aplică căutarea binară pentru a găsi a n_ea putere a datelor de intrare. Valoarea high este aleasă în funcție de a n-a putere dorită.

\*/ thread-ul Mapper
Fiecare thread-ul Mapper va citi un număr de fișiere în funcție de următoarele reguli :
1/ id-ul thread-ului este primul număr de fișier pe care îl va citi
2/ numărul fișierului următor = numărul fișierului curent plus id-ul acestuia
3/ numărul fișierului este mai mic decât numărul total de fișiere.

Pentru fiecare linie după prima linie, se utilizează funcția getNthRoot pentru a verifica numărul perfect al putere și se introduce numărul corect în data->array și se actualizează numărul de elemente din acel array.

După terminarea citirii tuturor fișierelor, thread Mapper va atribui data->complete la 1. Pentru a comunica thread Reducer că thread-ul Mapper este complet.

\*/ thread-ul Reducer
Pentru fiecare thread-ul Reducer, va utiliza funcția check_complete pentru a detecta dacă toate firele au terminat de citit fișierele sau toate thread-uri Mapper a atribuit data->complete la 1. Thread-ul Reducer accesează fiecare mapator pentru a calcula numărul total de elemente pentru reductorul curent și le adaugă într-o singură matrice. Apoi, sortează acest tablou și numără numărul de elemente diferite din acel tablou și scrie rezultatul în fișier.
