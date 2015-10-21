Kompilowanie przyk³adowych programów pod systemem Windows
=========================================================

Testowane œrodowisko:
Windows 7
Sourcery CodeBench Lite 2011.09-69 for ARM EABI

Pakiet Sourcery CodeBench Lite for ARM EABI dostêpny jest pod adresem
http://www.codesourcery.com. Jego zainstalowanie sprowadza siê do
uruchomienia œci¹gniêtego pliku wykonywalnego i klikniêcia kilka razy
myszk¹. ¯eby skompilowaæ przyk³adowe programy, wystarczy zainstalowaæ
wersjê minimaln¹ zawieraj¹c¹ narzêdzia uruchamiane z wiersza poleceñ
(ang. command line) bez graficznego œrodowiska programistycznego IDE
(ang. Integrated Development Environment).

Aby skompilowaæ przyk³adowy program, nale¿y:

1. Rozpakowaæ archiwum.

2. Wejœæ do katalogu, gdzie rozpakowane jest archiwum, a nastêpnie do
   podkatalogu make\scripts. Przeedytowaæ dwie pocz¹tkowe linie pliku
   makefile.in. Powinny wygl¹daæ tak:

   # TOOLCHAIN = arm-elf
   TOOLCHAIN = arm-none-eabi

3. Uruchomiæ konsolê systemu. Wejœæ do katalogu z plikiem makefile dla
   odpowiedniego programu przyk³adowego, czyli do jednego z katalogów
   make\usb*, np.:

   cd make\usb0_led_103

4. Utworzyæ katalog roboczy poleceniem (ignoruj¹c wypisywane
   komunikaty o b³êdach):

   cs-make mkdir

5. Skompilowaæ przyk³ad poleceniem (ignoruj¹c komunikat "Sk³adnia
   polecenia jest niepoprawna."):

   cs-make

Polecenie cs-make z pakietu Sourcery ma spore ograniczenia:
* Nie mo¿na go wywo³aæ z parametrem clean.
* Nie zadzia³a z plikiem makefile umieszczonym w katalogu make\scripts.
