# Expat

Expat ist die einzige externe Abhängigkeit des CodeGenerators, die **nicht**
header-only ist. Sie wird für den ARXML-Parser gebraucht
(`OptiScan::Parser::Arxml::ArxmlReader`).

- **Version:** 2.10.0, veröffentlicht am 2026-11-02
- **Lizenz:** MIT — kommerziell kostenfrei nutzbar
- **Quelle:** https://github.com/libexpat/libexpat/releases/tag/R_2_10_0

## Einchecken

Das entpackte Release gehört **vollständig** in dieses Verzeichnis, sodass
`Source/Extern/Expat/CMakeLists.txt` existiert:

```
Source/Extern/Expat/
    CMakeLists.txt
    COPYING
    lib/
    cmake/
    ...
```

Nicht nur `lib/` kopieren — das mitgelieferte CMake wird mitbenutzt.

## Warum das mitgelieferte CMake und keine eigene Quelldateiliste

Zwischen 2.6.4 und 2.10.0 hat sich das Dateilayout von `lib/` spürbar geändert:
`xcs.c`, `entities.h`, `hash_table.h` und `memory_sanitizer.h` sind neu, und die
Entropiequelle ist jetzt auf sechs `random_*.c` aufgeteilt, von denen je nach
Plattform eine übersetzt wird. Eine handgepflegte Liste würde beim nächsten Update
stillschweigend das Falsche bauen.

`Source/Extern/CMakeLists.txt` schaltet alles ab, was über die statische Bibliothek
hinausgeht: Werkzeuge, Tests, Beispiele, Handbuchseiten, Fuzzer, pkg-config und die
Install-Regeln. Übrig bleibt das Ziel `expat::expat`, das an `Extern` gehängt wird.

## Beim Aktualisieren beachten

Expat erscheint derzeit in kurzen Abständen (2.8.2 im Juni 2026, 2.10.0 im November
2026) und hat eine einschlägige CVE-Historie. Die Version sollte mitgezogen werden,
statt dauerhaft auf einem Stand zu bleiben. Nach einem Update:

1. Prüfen, ob die abgeschalteten `EXPAT_*`-Optionen noch so heißen.
2. Diese Datei auf die neue Version und das neue Datum aktualisieren.
3. Den Prüfpfad des ARXML-Parsers laufen lassen — die Elementzählung über
   `BACKBONE_EXT_CAN_2025_31a.arxml` muss unverändert bleiben.
