Kompilowanie przyk�adowych program�w pod systemem Windows
=========================================================

Testowane �rodowisko:
Windows 7
Sourcery CodeBench Lite 2011.09-69 for ARM EABI

Pakiet Sourcery CodeBench Lite for ARM EABI dost�pny jest pod adresem
http://www.codesourcery.com. Jego zainstalowanie sprowadza si� do
uruchomienia �ci�gni�tego pliku wykonywalnego i klikni�cia kilka razy
myszk�. �eby skompilowa� przyk�adowe programy, wystarczy zainstalowa�
wersj� minimaln� zawieraj�c� narz�dzia uruchamiane z wiersza polece�
(ang. command line) bez graficznego �rodowiska programistycznego IDE
(ang. Integrated Development Environment).

Aby skompilowa� przyk�adowy program, nale�y:

1. Rozpakowa� archiwum.

2. Wej�� do katalogu, gdzie rozpakowane jest archiwum, a nast�pnie do
   podkatalogu make\scripts. Przeedytowa� dwie pocz�tkowe linie pliku
   makefile.in. Powinny wygl�da� tak:

   # TOOLCHAIN = arm-elf
   TOOLCHAIN = arm-none-eabi

3. Uruchomi� konsol� systemu. Wej�� do katalogu z plikiem makefile dla
   odpowiedniego programu przyk�adowego, czyli do jednego z katalog�w
   make\usb*, np.:

   cd make\usb0_led_103

4. Utworzy� katalog roboczy poleceniem (ignoruj�c wypisywane
   komunikaty o b��dach):

   cs-make mkdir

5. Skompilowa� przyk�ad poleceniem (ignoruj�c komunikat "Sk�adnia
   polecenia jest niepoprawna."):

   cs-make

Polecenie cs-make z pakietu Sourcery ma spore ograniczenia:
* Nie mo�na go wywo�a� z parametrem clean.
* Nie zadzia�a z plikiem makefile umieszczonym w katalogu make\scripts.
