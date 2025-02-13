import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, HostListener, OnInit } from '@angular/core';
import { Router } from '@angular/router';

@Component({
  selector: 'app-terms-and-conditions',
  templateUrl: './terms-and-conditions.component.html',
  styleUrls: ['./terms-and-conditions.component.css']
})
export class TermsAndConditionsComponent implements OnInit {
  dropdownVisible = false;
  selectedSection: string = 'generalitati';
  apiURL: string = "{{API_URL}}";
  key: string = "{{POSTGRES_PASSWORD}}";
  
  termsSections = [
    {
      title: '1. Informații preliminare',
      content: [
        'Prezentul document intitulat „Termeni și Condiții” constituie acordul legal dintre dumneavoastră în calitate de Client, Utilizator și/sau Vizitator și ParkPass, vizând utilizarea Site-ului, comanda unei politici GDPR, accesarea și folosirea facilităților Site-ului etc. Pentru utilizarea conformă a Site-ului, vă recomandăm să citiți în prealabil acești Termeni și Condiții. Accesarea și vizitarea Site-ului, înregistrarea unui cont de utilizator/client, dar și plasarea unei Comenzi înseamnă acceptarea în totalitate și necondiționată a acestor Termeni și Condiții.',
        'Pentru a putea accesa în mod facil produsele pe care vi le punem la dispoziție, trebuie să acceptați în mod neechivoc și în totalitate Termenii și Condițiile de față. Dacă nu sunteți de acord să respectați acești Termeni și Condiții, fie chiar și parțial, vă rugăm să nu utilizați Site-ul. De asemenea, dacă, în orice moment, acești Termeni și Condiții nu mai sunt acceptați de către dumneavoastră, vă rugăm să sistați imediat accesarea și utilizarea Site-ului.',
        'Termenii și condițiile pot fi modificate oricând, fără a fi necesară o notificare prealabilă. Modificările și informațiile privind valabilitatea acestora vor fi postate pe acest site, în vederea informării celor interesați/vizați. Utilizatorii, Vizitatorii și Clienții trebuie să se raporteze la regulile, drepturile, obligațiile, procedurile etc. prevăzute de Termeni și Condiții în formatul valabil la data interacționării cu Site-ul, fără a exista o obligație a ParkPass de a-i informa individual despre actualizări și/sau modificări.'
      ]
    },
    {
      title: '2. Cine suntem?',
      content: [
        'www.parkpass.ro este un proiect al ParkPass. Ne propunem să venim în sprijinul dumneavoastră cu servicii și produse care să satisfacă nevoile apărute ca urmare a digitalizării economiei. De asemenea, dorim să conlucrăm cu toți clienții noștri pentru a ne îmbunătăți serviciile sau pentru a crea noi servicii ori produse care să răspundă nevoilor emergente ale acestora.',
        'Datele noastre de identificare sunt următoarele:',
        'Denumire: ParkPass',
        'Email: parking.info@yahoo.com – timpul mediu de răspuns 1 zile lucrătoare',
        'Telefon: +16313378380 - ne puteți contacta telefonic în intervalul'
      ]
    },
    {
      title: '3. Definirea unor termeni',
      content: [
        'Având în vedere faptul că în prezentul document anumite cuvinte ori fraze vor fi utilizate în mod recurent, am decis să facem o listă cu cele mai uzuale formulări pentru a facilita înțelegerea cât mai corectă a întregului document: o Comandă – reprezintă intenția Clientului, exprimată prin intermediul site-ului, prin care acesta își exprimă dorința și angajamentul de a achiziționa cel puțin unul dintre produsele comercializate de către noi.',
        'Cont – reprezintă o interfață unică a site-ului nostru, care este accesibilă doar clienților care își creează un cont. Crearea unui cont presupune personalizarea unei secțiuni a site-ului prin introducerea adresei de email și a unei parole, secțiune care conține diverse informații despre Client.',
        'Contract – reprezintă contractul la distanță încheiat între noi și dumneavoastră, clienții noștri. Acest contract se încheie fără prezența fizică simultană a Vânzătorului și a Cumpărătorului, contract ce se încheie prin acceptarea, după atentă citire, a prezentului Document de către Cumpărător.',
        'Cumpărător/Client – orice persoană fizică, în vârstă de cel puțin 18 ani, cu capacitate de exercițiu deplină sau orice persoană/entitate juridică, care își creează sau nu un Cont în Site și efectuează o Comandă online.',
        'Vânzător – reprezintă societatea ParkPass, astfel cum ne-am prezentat anterior.',
        'Document / Politică de Utilizare / Termeni și Condiții – prezenta Politică de Utilizare privind termenii și condițiile de utilizare a Site-ului, denumită și „Termenii și Condițiile”, „T&C” sau „Politică”, care va guverna raportul contractual dintre SRL și Clienți, Utilizatori și/sau Vizitatori și care va fi interpretată în conformitate cu legea română. Orice neconformitate sau nevalabilitate a unei părți sau clauze din Termeni și Condiții cu alte prevederi legale aplicabile nu are efecte asupra valabilității și legalității celorlalte prevederi din Politică.',
        'Produs – reprezintă oricare din produsele ale Politicii GDPR care sunt comercializate pe site-ul www.parkpass.ro.',
        'Utilizator – orice persoană care a creat un cont pe site.',
        'Vizitator – orice persoană care accesează site-ul.',
        'Site-ul – reprezintă site-ul nostru www.parkpass.ro, precum și orice secțiune sau subpagină a acestuia.'
      ]
    },
    {
      title: '4. Condiții generale de utilizare',
      content: [
        'Site-ul www.parkpass.ro este un site public ce poate fi utilizat de către orice persoană interesată în a achiziționa unul sau mai multe produse dintre cele comercializate de către noi.',
        'Ne rezervăm dreptul de a refuza ori restrânge accesul vizitatorilor sau clienților la anumite subsecțiuni sau funcții specifice ale site-ului dacă va exista suspiciunea rezonabilă că se încearcă fraudarea sau eludarea politicilor site-ului. De asemenea, accesul va fi refuzat ori restrâns ori de câte ori vom avea suspiciunea rezonabilă că, prin anumite acțiuni, se încearcă prejudicierea intereselor noastre.',
        'Interzicem în mod expres orice utilizare abuzivă a site-ului nostru, prin utilizare abuzivă înțelegându-se orice acțiune ori inacțiune prin care nu sunt respectate regulile statuate în prezenta Politică sau în orice altă politică existentă pe site, orice acțiune care contravine bunei-credințe în relațiile comerciale ori contractuale sau orice acțiune care încalcă legislația în vigoare sau bunele practici și/sau moravuri.'
      ]
    },
    {
      title: '5. Comenzile online',
      content: [
        'Prin plasarea unei comenzi online, vă exprimați dorința și acordul de a intra într-o relație contractuală bilaterală cu noi, echipa ParkPass. O comandă online poate conține unul sau mai multe dintre produsele pe care noi le comercializăm în magazinul nostru online.',
        'Poate plasa o comandă pe site orice persoană fizică (dacă a împlinit vârsta de minimum 18 ani) și orice persoană juridică. Comanda poate fi plasată din meniul aferent Contului de client. Contul personal va facilita interacțiunea dintre noi și dumneavoastră, vă va aduce beneficii suplimentare (cu titlu de exemplu: promoții exclusive, bonusuri sau reduceri) și, mai mult decât atât, veți putea avea o evidență concretă a tuturor comenzilor pe care le-ați avut.',
        'Simplă adăugare a unui produs în coșul de cumpărături nu va conduce automat la rezervarea respectivului element. De asemenea, adăugarea unui produs în coșul de client, fără definitivarea procedurii de achiziție, nu va conduce la încheierea unui contract valabil, corelativ, noi nu vom avea obligația să livrăm respectivul produs.',
        'O comandă va fi considerată finalizată doar după parcurgerea tuturor etapelor din meniul Finalizare Comandă/Date de facturare și doar după confirmarea plății integrale a prețului produselor ori serviciilor pentru care ați optat. Plata se va considera efectuată în momentul creditării contului nostru cu suma aferentă produselor ori serviciilor achiziționate de către dumneavoastră.',
        'Produsele comandate de dumneavoastră vor fi livrate prin curier la adresa indicată în formularul de comandă, în termen de cel mult 14 zile lucrătoare, calculat de la momentul confirmării comenzii și creditării contului nostru cu suma aferentă comenzii.'
      ]
    }
  ];

  privacyPolicySections = [
    {
      title: '1. Informații generale',
      content: ['Confidențialitatea datelor dumneavoastră cu caracter personal reprezintă una dintre preocupările principale ale ParkPass în calitate de operator de date. Acest document are rolul de a vă informa cu privire la prelucrarea datelor dumneavoastră cu caracter personal, în contextul utilizării paginii de internet www.parkpass.ro („Site-ul”).']
    },
    {
      title: '2. Categoriile de date cu caracter personal prelucrate',
      content: ['2.1. Dacă sunteți client al Site-ului: ',
        'ParkPass va prelucra datele dumneavoastră cu caracter personal, cum ar fi nume și prenume, telefon, adresa de e-mail, adresa de facturare, adresa de livrare, date referitoare la modul în care utilizați Site-ul, de exemplu comportamentul/preferințele/obiceiurile dumneavoastră în cadrul ParkPass, precum și orice alte categorii de date pe care le furnizați în mod direct în contextul creării contului de utilizator, în contextul plasării unei comenzi prin intermediul site-ului sau în orice alt mod care rezultă din utilizarea Site-ului. Dacă pentru a vă crea cont de utilizator pe Site utilizați contul dumneavoastră de Facebook sau Google, ParkPass va prelucra următoarele date publice de profil afișate de aplicațiile respective: nume utilizator, adresa de e-mail. În cazul în care alegeți să vă creați cont de utilizator doar înainte de a finaliza comanda unui produs disponibil pe Site, se va solicita adresa dumneavoastră de e-mail, pe baza căreia va fi creat automat un cont. În cazul în care nu finalizați comanda, adresa de e-mail și celelalte date furnizate nu vor fi stocate de ParkPass, iar contul creat va fi șters automat.',
        '2.2. Dacă sunteți vizitator al Site-ului:',
        'ParkPass va prelucra datele dumneavoastră cu caracter personal pe care le furnizați în mod direct în contextul utilizării Site-ului, cum ar fi datele pe care le furnizați în cadrul secțiunii de contact / întrebări / reclamații, în măsura în care ne contactați în acest fel.']
    },
    {
      title: '3. Scopurile și temeiurile prelucrării',
      content: ['3.1. Dacă sunteți client al Site-ului, ParkPass prelucrează datele dumneavoastră cu caracter personal astfel:',
        'Pentru desfășurarea relației contractuale dintre dumneavoastră și ParkPass, respectiv pentru preluarea, validarea, expedierea și facturarea comenzii plasate pe Site, informarea dumneavoastră asupra stării comenzii, organizarea returului de produse comandate etc.',
        'Temei: Prelucrarea datelor dumneavoastră în acest scop are la bază contractul încheiat între dumneavoastră și ParkPass, definit în cuprinsul Termenelor și Condițiilor. Furnizarea datelor dumneavoastră cu caracter personal este necesară pentru executarea acestui contract. Refuzul furnizării datelor poate avea drept consecință imposibilitatea derulării raporturilor contractuale dintre dumneavoastră și ParkPass.',
        'Pentru îndeplinirea obligațiilor legale care incumbă ParkPass în contextul serviciilor prestate prin intermediul Site-ului, inclusiv a obligațiilor în materie fiscală, precum și în materie de arhivare.',
        'Temei: Prelucrarea datelor dumneavoastră pentru acest scop este necesară în baza unor obligații legale. Furnizarea datelor dumneavoastră în acest scop este necesară. Refuzul furnizării datelor poate avea drept consecință imposibilitatea ParkPass de a respecta obligațiile legale care îi revin și, implicit, imposibilitatea de a vă oferi serviciile prin intermediul Site-ului.',
        'Pentru activități de marketing, respectiv pentru transmiterea, prin intermediul mijloacelor de comunicare la distanță (e-mail, sms), de comunicări comerciale privind produsele și serviciile oferite de ParkPass prin intermediul Site-ului.',
        'Temei: Prelucrarea datelor dumneavoastră pentru acest scop are la bază consimțământul dumneavoastră, dacă alegeți să-l furnizați. Vă puteți exprima consimțământul pentru prelucrarea datelor în acest scop prin bifarea căsuței corespunzătoare la momentul creării contului sau ulterior creării contului, în secțiunea Informațiile contului meu. Pentru dezabonarea de la primirea unor astfel de comunicări comerciale, puteți folosi opțiunea de la finalul fiecărui e-mail/sms conținând comunicări comerciale. În plus, vă puteți dezabona prin accesarea secțiunii Informațiile contului meu. Furnizarea datelor dumneavoastră în acest scop este voluntară. Refuzul furnizării consimțământului pentru prelucrarea datelor dumneavoastră în acest scop nu va avea urmări negative pentru dumneavoastră.',
        'În scopul efectuării diverselor analize, raportări privind modul de funcționare a Site-ului, realizarea de profiluri de preferințe de consum, în principal, în vederea îmbunătățirii experienței oferite pe Site.',
        'Temei: Prelucrarea datelor dumneavoastră pentru acest scop are la bază interesul legitim al ParkPass de a îmbunătăți permanent experiența clienților pe Site. Furnizarea datelor dumneavoastră în acest scop este voluntară. Refuzul furnizării datelor pentru acest scop nu va avea urmări negative pentru dumneavoastră.',
        '3.2. Dacă sunteți vizitator al Site-ului, ParkPass prelucrează datele dumneavoastră cu caracter personal astfel:',
        'Pentru activități de marketing, respectiv pentru transmiterea, prin intermediul mijloacelor de comunicare la distanță (e-mail, sms), de comunicări comerciale privind produsele și serviciile oferite de ParkPass prin intermediul Site-ului.',
        'Temei: Prelucrarea datelor dumneavoastră pentru acest scop are la bază consimțământul dumneavoastră, dacă alegeți să-l furnizați. Vă puteți exprima consimțământul pentru prelucrarea datelor în acest scop prin completarea și bifarea căsuței corespunzătoare din formularul pentru abonarea la newsletter disponibil pe Site. Pentru dezabonarea de la primirea unor astfel de comunicări comerciale, puteți folosi opțiunea de la finalul fiecărui e-mail/sms conținând comunicări comerciale. Furnizarea datelor dumneavoastră în acest scop este voluntară. Refuzul furnizării consimțământului pentru prelucrarea datelor dumneavoastră în acest scop nu va avea urmări negative pentru dumneavoastră.',
        'Pentru rezolvarea plângerilor, reclamațiilor și pentru a monitoriza traficul și a îmbunătăți experiența dumneavoastră oferită pe Site.',
        'Temei: Prelucrarea datelor dumneavoastră pentru acest scop are la bază interesul legitim al ParkPass de a asigura funcționarea corectă a Site-ului, precum și pentru a îmbunătăți permanent experiența vizitatorilor Site-ului, inclusiv prin soluționarea diferitelor comentarii, întrebări sau reclamații. Furnizarea datelor dumneavoastră în acest scop este voluntară. Refuzul furnizării datelor pentru acest scop nu va avea urmări negative pentru dumneavoastră.']
    },
    {
      title: '4. Durata pentru care vă prelucrăm datele',
      content: ['Ca principiu, ParkPass va prelucra datele dumneavoastră cu caracter personal atât cât este necesar pentru realizarea scopurilor de prelucrare menționate mai sus. În cazul în care sunteți client, vom prelucra datele dumneavoastră pe întreaga durată a raporturilor contractuale și ulterior, conform obligațiilor legale care revin în sarcina ParkPass (de ex., în cazul documentelor justificative financiar-contabile pentru care termenul de păstrare prevăzut de lege este de 10 ani de la data încheierii exercițiului financiar în cursul căruia au fost întocmite). În situația în care sunteți client și vă exercitați opțiunea de ștergere a contului de utilizator, prin apăsarea butonului Ștergere cont din secțiunea Informațiile contului meu, ParkPass va interpreta această acțiune ca opțiunea dumneavoastră de a vă dezabona de la primirea de comunicări comerciale prin care vă ținem la curent despre produsele și serviciile oferite prin intermediul site-ului. În acest sens, dacă alegeți să vă ștergeți contul de utilizator, nu vă vom mai trimite e-mailuri și/sau sms-uri de acest gen. Totuși, dorim să vă informăm că ștergerea contului nu va avea ca efect automat ștergerea datelor dumneavoastră cu caracter personal. În cazul în care doriți să nu vă mai fie prelucrate datele cu caracter personal sau dacă doriți ștergerea datelor, vă puteți exercita drepturile detaliate la punctul 7 de mai jos. Dacă solicitați ștergerea contului, dar pe acel cont există cel puțin o comandă activă, solicitarea de ștergere a contului va putea fi înregistrată numai după livrarea produselor și finalizarea ultimei comenzi active. Dacă vă retrageți consimțământul pentru prelucrarea datelor în scop de marketing, ParkPass va înceta prelucrarea datelor dumneavoastră cu caracter personal în acest scop, fără însă a afecta prelucrările desfășurate de ParkPass pe baza consimțământului exprimat de către dumneavoastră înainte de retragerea acestuia.']
    },
    {
      title: '5. Dezvăluirea datelor cu caracter personal',
      content: ['Pentru îndeplinirea scopurilor de prelucrare, ParkPass poate dezvălui datele dumneavoastră către parteneri, către terțe persoane sau entități care sprijină ParkPass în desfășurarea activității prin intermediul Site-ului (de exemplu, firme de curierat, furnizori de servicii IT), ori către autorități publice centrale/locale, în următoarele cazuri exemplificativ enumerate:',
        'pentru administrarea Site-ului;',
        'în situațiile în care această comunicare ar fi necesară pentru atribuirea de premii sau alte facilități clienților, obținute ca urmare a participării lor la diverse campanii promoționale organizate de către ParkPass prin intermediul Site-ului;',
        'pentru menținerea, personalizarea și îmbunătățirea Site-ului și a serviciilor derulate prin intermediul lui;',
        'pentru efectuarea analizei datelor, testarea și cercetarea, monitorizarea tendințelor de utilizare și activitate, dezvoltarea de caracteristici de siguranță și autentificare a utilizatorilor;',
        'pentru transmiterea de comunicări comerciale de marketing, în condițiile și limitele prevăzute de lege;',
        'atunci când dezvăluirea datelor cu caracter personal este prevăzută de lege etc.']
    },
    {
      title: '6. Transferul datelor cu caracter personal',
      content: ['Datele cu caracter personal furnizate către ParkPass pot fi transferate în afara României, dar doar către state din Uniunea Europeană.']
    },
    {
      title: '7. Drepturile de care beneficiați',
      content: ['În condițiile prevăzute de legislația în domeniul prelucrării datelor cu caracter personal, în calitate de persoane vizate, beneficiați de următoarele drepturi:',
        'dreptul la informare, respectiv dreptul de a primi detalii privind activitățile de prelucrare efectuate de către ParkPass, conform celor descrise în prezentul document;',
        'dreptul de acces la date, respectiv dreptul de a obține confirmarea din partea ParkPass cu privire la prelucrarea datelor cu caracter personal, precum și detalii privind activitățile de prelucrare, cum ar fi modalitatea în care sunt prelucrate datele, scopul în care se face prelucrarea, destinatarii sau categoriile de destinatari ai datelor etc.;',
        'dreptul la rectificare, respectiv dreptul de a obține corectarea, fără întârzieri justificate, de către ParkPass a datelor cu caracter personal inexacte/nejustificate, precum și completarea datelor incomplete; rectificarea/completarea va fi comunicată fiecărui destinatar la care au fost transmise datele, cu excepția cazului în care acest lucru se dovedește imposibil sau presupune eforturi disproporționate;',
        'dreptul la ștergerea datelor, fără întârzieri nejustificate, („dreptul de a fi uitat”), în cazul în care se aplică unul dintre următoarele motive:',
        'acestea nu mai sunt necesare pentru îndeplinirea scopurilor pentru care au fost colectate sau prelucrate;',
        'în cazul în care este retras consimțământul și nu există niciun alt temei juridic pentru prelucrare;',
        'în cazul în care persoana vizată se opune prelucrării și nu există motive legitime care să prevaleze;',
        'în cazul în care datele cu caracter personal au fost prelucrate ilegal;',
        'în cazul în care datele cu caracter personal trebuie șterse pentru respectarea unei obligații legale;',
        'datele cu caracter personal au fost colectate în legătură cu oferirea de servicii ale societății informaționale conform dreptului Uniunii sau al dreptului intern sub incidența căruia se află operatorul. Este posibil ca, în urma solicitării de ștergere a datelor, ParkPass să anonimizeze aceste date (lipsindu-le astfel de caracterul personal) și să continue în aceste condiții prelucrarea pentru scopuri statistice;',
        'dreptul la restricționarea prelucrării în măsura în care:',
        'persoana contestă exactitatea datelor, pe o perioadă care ne permite verificarea corectitudinii datelor;',
        'prelucrarea este ilegală, iar persoana vizată se opune ștergerii datelor cu caracter personal, solicitând în schimb restricționarea utilizării lor;',
        'operatorul nu mai are nevoie de datele cu caracter personal în scopul prelucrării, dar persoana vizată i le solicită pentru constatarea, exercitarea sau apărarea unui drept în instanță;',
        'persoana vizată s-a opus prelucrării (altele decât cele de marketing direct), pentru intervalul de timp în care se verifică dacă drepturile legitime ale operatorului prevalează asupra celor ale persoanei vizate;',
        'dreptul la portabilitatea datelor, respectiv (i) dreptul de a primi datele cu caracter personal într-o modalitate structurată, utilizată în mod curent și într-un format ușor de citit, precum și (ii) dreptul ca aceste date să fie transmise de către ParkPass către alt operator de date, în măsura în care sunt îndeplinite condițiile prevăzute de lege;',
        'dreptul la opoziție – în ceea ce privește activitățile de prelucrare se poate exercita prin transmiterea unei solicitări conform celor indicate mai jos:',
        'în orice moment, din motive legate de situația particulară în care se află persoana vizată, ca datele care o vizează să fie prelucrate în temeiul interesului legitim al ParkPass sau în temeiul interesului public, cu excepția cazurilor în care ParkPass poate demonstra că are motive legitime și imperioase care justifică prelucrarea și care prevalează asupra intereselor, drepturilor și libertăților persoanelor vizate sau că scopul este constatarea, exercitarea sau apărarea unui drept în instanță;',
        'în orice moment, în mod gratuit și fără nicio justificare, ca datele care o vizează să fie prelucrate în scop de marketing direct;',
        'dreptul de a nu fi supus unei decizii individuale automate, respectiv dreptul de a nu fi subiectul unei decizii luate numai pe baza unor activități de prelucrare automate, inclusiv crearea de profiluri, care produce efecte juridice care privesc persoana vizată sau o afectează în mod similar într-o măsură semnificativă;',
        'dreptul de a vă adresa Autorității Naționale de Supraveghere a Prelucrării Datelor cu Caracter Personal sau instanțelor competente, în măsura în care considerați necesar.',
        'Pentru orice întrebări suplimentare cu privire la modul în care datele cu caracter personal sunt prelucrate și pentru a vă exercita drepturile menționate mai sus, vă rugăm să vă adresați la adresa de email: parking.info@yahoo.com.']
    }
  ];

  cookiePolicySections = [
    {
      title: '1. Introducere',
      content: ['Prezenta Politică privind fișierele cookies se aplică tuturor utilizatorilor paginii de internet www.parkpass.ro. Informațiile prezentate în continuare au ca scop informarea utilizatorilor acestei pagini de internet cu privire la plasarea, utilizarea și administrarea cookie-urilor de către ParkPass în contextul navigării utilizatorilor pe această pagină de internet.']
    },
    {
      title: '2. Ce sunt cookie-urile?',
      content: ['Folosim termenul „cookie-uri” pentru a ne referi la modulele cookie și la tehnologiile similare prin intermediul cărora pot fi colectate informații în mod automat. Un „Internet Cookie” (termen cunoscut și sub denumirea de „browser cookie” sau „HTTP cookie” ori „cookie”) reprezintă un fișier de mici dimensiuni, format din litere și numere, care va fi stocat pe computerul, terminalul mobil sau pe alte echipamente ale unui utilizator prin intermediul cărora se accesează internetul.',
        'Cookie-urile sunt instalate prin solicitarea emisă de un web-server către un browser (de ex.: Internet Explorer, Firefox, Chrome). Odată instalate, cookie-urile au o durată de existență determinată, rămânând „pasive”, în sensul că nu conțin programe software, viruși sau spyware și nu vor accesa informațiile de pe hard drive-ul utilizatorului pe al cărui echipament au fost instalate. Un cookie este format din două părți: 1. numele cookie-ului; și 2. conținutul sau valoarea cookie-ului. Din punct de vedere tehnic, doar web-serverul care a trimis cookie-ul îl poate accesa din nou în momentul în care un utilizator se întoarce pe pagina de internet asociată web-serverului respectiv.']
    },
    {
      title: '3. Pentru ce scopuri sunt utilizate cookie-urile prin intermediul acestei pagini de internet?',
      content: ['Cookie-urile sunt utilizate pentru a furniza utilizatorilor acestei pagini de internet o experiență mai bună de navigare și servicii adaptate nevoilor și interesului fiecărui utilizator în parte, și anume pentru:',
        'îmbunătățirea utilizării acestei pagini de internet, inclusiv prin identificarea oricăror erori care apar în timpul vizitării/utilizării acesteia de către utilizatori;',
        'furnizarea de statistici anonime cu privire la modul în care este utilizată această pagină de internet către ParkPass, în calitate de deținător al acestei pagini de internet;',
        'anticiparea unor eventuale bunuri care vor fi în viitor puse la dispoziția utilizatorilor prin intermediul acestei pagini de internet, în funcție de serviciile/produsele accesate. Pe baza feedback-ului transmis prin cookie-uri în legătură cu modul în care se utilizează această pagină de internet, ParkPass poate adopta măsuri pentru ca această pagină de internet să fie mai eficientă și mai accesibilă pentru utilizatori. Astfel, utilizarea cookie-urilor permite memorarea anumitor setări/preferințe stabilite de către utilizatorii acestei pagini de internet, precum:',
        'limba în care este vizualizată o pagină de internet;',
        'moneda în care se exprimă anumite prețuri sau tarife;',
        'facilitarea accesului în contul utilizatorilor;',
        'postarea comentariilor pe site.']
    },
    {
      title: '4. Care este durata de viață a cookie-urilor?',
      content: ['Durata de viață a cookie-urilor poate varia semnificativ, depinzând de scopul pentru care sunt plasate. Există următoarele categorii de cookie-uri care determină și durata de viață a acestora:',
        'Cookie-uri de sesiune – Un „cookie de sesiune” este un cookie care este șters automat când utilizatorul își închide browserul.',
        'Cookie-uri persistente sau fixe – Un „cookie persistent” sau „fix” este un cookie care rămâne stocat în terminalul utilizatorului până când atinge o anumită dată de expirare (care poate fi în câteva minute, zile sau câțiva ani în viitor) sau până la ștergerea acestuia de către utilizator în orice moment prin intermediul setărilor browserului.']
    },
    {
      title: '5. Ce sunt cookie-urile plasate de terți?',
      content: ['Anumite secțiuni de conținut de pe pagina de internet pot fi furnizate prin intermediul unor terți, adică nu de către ParkPass, caz în care aceste cookie-uri sunt denumite cookie-uri plasate de terți („third party cookie-uri”). Terții furnizori ai cookie-urilor trebuie să respecte, de asemenea, regulile în materie de protecție a datelor și Politica de Confidențialitate disponibilă pe această pagină de internet. Aceste cookie-uri pot proveni de la următorii terți: Google Analytics, Facebook.']
    },
    {
      title: '6. Ce cookie-uri sunt folosite prin intermediul acestei pagini de internet?',
      content: ['Prin utilizarea/vizitarea paginii de internet pot fi plasate următoarele cookie-uri: a. Cookie-uri de performanță a paginii de internet; b. Cookie-uri de analiză a utilizatorilor; c. Cookie-uri pentru geotargeting; d. Cookie-uri de înregistrare; e. Cookie-uri pentru publicitate; f. Cookie-uri ale furnizorilor de publicitate.',
        'a. Cookie-uri de performanță',
        'Prin acest tip de cookie-uri sunt memorate preferințele utilizatorului acestei pagini de internet, astfel încât setarea din nou a preferințelor în cazul vizitării ulterioare a paginii de internet nu mai este necesară.',
        'b. Cookie-uri de analiză a utilizatorilor',
        'Aceste cookie-uri ne informează dacă un anumit utilizator al paginii de internet a mai vizitat/utilizat această pagină de internet anterior. Aceste cookie-uri sunt utilizate doar în scopuri statistice.',
        'c. Cookie-uri pentru geotargeting',
        'Aceste cookie-uri sunt utilizate de către un soft care stabilește țara de proveniență a utilizatorului paginii de internet. Vor fi primite aceleași reclame indiferent de limba selectată.',
        'd. Cookie-uri pentru înregistrare',
        'Atunci când vă înregistrați pe acest site, se generează cookie-uri care memorează acest demers. Serverele utilizează aceste cookie-uri pentru a ne arăta contul cu care sunteți înregistrat. De asemenea, utilizarea acestor cookie-uri permite să asociem orice comentariu postat pe pagina de internet cu username-ul contului folosit. În cazul în care nu a fost selectată opțiunea „păstreazămă înregistrat”, aceste cookie-uri se vor șterge automat la momentul terminării sesiunii de navigare.',
        'e. Cookie-uri pentru publicitate',
        'Aceste cookie-uri permit aflarea vizualizării de către un utilizator a unei reclame online, tipul acesteia și timpul scurs de la momentul vizualizării respectivului mesaj publicitar. Ca atare, astfel de cookie-uri sunt folosite pentru targetarea publicității online. Aceste cookie-uri sunt anonime, stocând informații despre conținutul vizualizat, nu și despre utilizatori.']
    },
    {
      title: '7. Ce tip de informații sunt stocate și accesate prin intermediul cookie-urilor?',
      content: ['Cookie-urile păstrează informații într-un fișier text de mici dimensiuni care permit recunoașterea browserului. Această pagină de internet recunoaște browserul până când cookie-urile expiră sau sunt șterse.']
    },
    {
      title: '8. Particularizarea setărilor browserului în ceea ce privește cookie-urile',
      content: ['În cazul în care utilizarea cookie-urilor nu este deranjantă iar calculatorul sau echipamentul tehnic utilizat pentru navigarea pe această pagină de internet este folosit doar de către dumneavoastră, pot fi setate termene lungi de expirare pentru stocarea istoricului de navigare. În cazul în care calculatorul sau echipamentul tehnic utilizat pentru navigarea pe această pagină de internet este folosit de mai multe persoane, poate fi luată în considerare setarea pentru ștergerea datelor individuale de navigare de fiecare dată când browserul este închis.']
    },
    {
      title: '9. Cum pot fi oprite cookie-urile?',
      content: ['Dezactivarea și refuzul de a primi cookie-uri pot face această pagină de internet dificil de vizitat, atrăgând după sine limitări ale posibilităților de utilizare ale acesteia. Utilizatorii își pot configura browserul să respingă fișierele cookie sau să accepte cookie-uri de la o pagină de internet anume. Toate browserele moderne oferă posibilitatea de a schimba setările cookie-urilor. Aceste setări pot fi accesate, ca regulă, în secțiunea „opțiuni” sau în meniul de „preferințe” al browserului tău. Totuși, refuzarea sau dezactivarea cookie-urilor nu înseamnă că nu veți mai primi publicitate online – ci doar că aceasta nu va fi adaptată preferințelor și intereselor dumneavoastră, evidențiate prin comportamentul de navigare.',
        'Pentru orice întrebări suplimentare cu privire la modul în care sunt utilizate cookie-urile prin intermediul acestei pagini de internet, vă rugăm să vă adresați la datele specificate la Contact.']
    }
  ];

  selectedPolicy: 'terms' | 'cookies' | 'privacy' = 'terms';

  constructor(private http: HttpClient, private router: Router) { }

  ngOnInit(): void {
    const storedPolicy = localStorage.getItem('policy');
    localStorage.setItem('policy', '');
    if (storedPolicy && (storedPolicy === 'terms' || storedPolicy === 'privacy' || storedPolicy === 'cookies')) {
      this.selectedPolicy = storedPolicy;
    }
  }

  toggleDropdown(): void {
    this.dropdownVisible = !this.dropdownVisible;
  }

  @HostListener('document:click', ['$event'])
  closeDropdown(event: MouseEvent): void {
    const dropdown = document.getElementById('accountDropdown');
    const accountIcon = document.querySelector('.accountIconContainer');

    if (this.dropdownVisible && dropdown && !dropdown.contains(event.target as Node) && !accountIcon?.contains(event.target as Node)) {
      this.dropdownVisible = false;
    }
  }

  navigateTo(destination: string): void {
    const routes: { [key: string]: string } = {
      mainpage: '/',
      login: '/login',
      createAccount: '/create-subscription',
      contact: '/contact'
    };

    const route = routes[destination];
    this.router.navigate([route]);
  }

  footerNavigateTo(destination: string): void {
    localStorage.setItem('policy', destination);
    this.router.navigateByUrl('/').then(() => {
      this.router.navigate(['/terms-and-conditions']);
    });
  }

  subscribeNewsletter(email: string): void {
    (document.querySelector('form input') as HTMLInputElement).value = '';

    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]{2,}$/;
    if (!emailRegex.test(email))
      return;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = this.apiURL  + '/api/subscribeNewsletter';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe();
  }

  selectSection(section: string): void {
    this.selectedSection = section;
  }

  changePolicy(policy: 'terms' | 'cookies' | 'privacy'): void {
    this.selectedPolicy = policy;
    this.selectedSection = '';
  }

}
