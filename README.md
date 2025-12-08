## Autobus (Projekat iz predmeta Računarska Grafika)
Napraviti 2D grafičku aplikaciju za praćenje autobuske rute.

- Na ekranu je prikazana putanja kojom se kreće autobus. Sačinjena je od krivih crvenih linija koje se nalaze između stanica i koje formiraju nepravilan oblik, koje su obeležene kao crveni krugovi sa belim brojem unutra. Stanica ima 10 i obeležene su od 0 do 9. Autobus je prikazan sličicom autobusa i kreće se po putanji stanicama redom od 0 do 9, a putanja je zatvorena, odnosno, osim svake dve susedne, stanica 0 je takođe povezana sa stanicom 9.

- Autobus se od stanice do stanice kreće konstantnom brzinom. Kada dođe do stanice, on se zaustavi. Potrebno je na proizvoljnom uglu ekrana prikazati ikonicu vrata koja su zatvorena dok se autobus vozi, a otvorena su 10 sekundi dok je on na stanici (za ovo vreme se ikonica autobusa takođe zadrži na stanici).

- U drugom proizvoljnom uglu ekrana pamti se broj putnika u autobusu, koji je na početku 0. Dok su vrata autobusa otvorena na stanici, levim klikom miša na ekran ulazi po jedan putnik, odnosno broj se povećava za 1; dok desnim klikom miša na ekran izlazi po jedan putnik, odnosno broj se smanji za 1. Opseg broja putnika se kreće od 0 do 50.

- Dok su vrata autobusa otvorena, pritiskom na taster K u autobus ulazi kontrola. Broj putnika se tada poveća za 1, i trećem proizvoljnom uglu ekrana se prikazuje oznaka kontrole (npr. slika saobraćajca u Monopolu) sve dok autobus ne dođe do sledeće stanice, kad kontrola izlazi i broj putnika se smanji za 1. Na stanici na kojoj kontrola izlazi se generiše random vrednost između 0 i broja putnika - 1 kojima je naplaćena kazna (- 1 jer je jedan od putnika kontrola) i sabira se sa ukupnim brojem naplaćenih kazni. Ovaj broj je inicijalno 0 i potrebno ga je uvek prikazivati pored broja trenutnih putnika. Moguće je da druga kontrola uđe na stanici na kojoj je prethodna kontrola izašla.

- Kursor treba da ima izgled žutog rombastog saobraćajnog znaka (kao BUS STOP znak)  tako da je sam znak u gornjem levom uglu, a metalna šipka dijagonala slike.

### Marina Ivanović SV6/2022
