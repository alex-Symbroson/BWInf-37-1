
# Allgemeine Hinweise

## Kompilierung

Die Kompilierung der Programme erfolgte mit den bereitgestellten Makefiles und GNU Make 4.2.1 in den jeweiligen Ordnern erfolgen. Grundlegende Funktionen sind in `base` implementiert und werden von allen Programmen verwendet.
Als Kompiler wurde g++ (Ubuntu 8.2.0-7ubuntu1) 8.2.0 auf dem Betriebssystem Ubuntu 18.10 verwendet.
Man wechselt dazu in das Aufgabenverzeichnis und startet make. Ohne Argumente wird nur kompiliert, mit `make run` ausgeführt und mit `make clean` die Build-Dateien wieder gelöscht.
```sh
$ cd Aufgabe[i]
$ make run
```
Um dem Kompiler für das Ausführen Argumente mitzugeben, steht ARGS zur verfügung. Die Möglichen Argumente eines Programms sind mit --help einsehbar. Es kann in jedem Fall ohne zusätzliche Flag eine Eingabedatei angegeben werden
```sh
$ make run ARGS="--help"
```
Die Ausgabe ist an manchen Stellen mit Escape-Sequenzen farbig hervorgehoben. Diese werden aber nicht von jeder Konsole unterstützt (uA. unter Windows). Verwendet wurde GNOME Terminal 3.30.1, jedoch werden diese auch von meisten anderen linux-Terminals unterstützt, unter anderem Xterm und darauf basierende Varianten.
In der Dokumentation sind die Ausgaben so gut wie Möglich der Konsole nachempfunden worden.
