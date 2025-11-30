# Umsetzungs-Strategie (Ich-Form)

---

## 1. Was ich bauen will (kurz)

1. **Slitherlink-Puzzles aus einer Textdatei einlesen** (Größe + Gitter mit Zahlen/Leerfeldern).
2. Einen **Solver programmieren**, der wahlweise **eine** oder **alle Lösungen** findet (per CLI-Parameter).
3. Die Lösung als **ASCII-Gitter + Koordinatenliste** ausgeben.
4. **Parallelisieren** und Laufzeiten (seriell vs. parallel) im **Release-Modus** messen.
5. Fokus: **Parallelisierung des Suchbaums** (Tasks, Work-Stealing, Speedup), keine GUI.

---

## 2. Slitherlink formal denken

- Punkte-Gitter: `(n+1) × (m+1)` Punkte.
- Zellen `(i,j)` mit Zahl {0,1,2,3} oder leer.
- Ziel: Teilmenge der horizontalen/vertikalen Kanten bildet **einen geschlossenen Zyklus** ohne Verzweigungen, Zellen erfüllen ihre Zahl.
- Indizes: Punkt `p = r*(m+1)+c`; Kanten horizontal `(r,c)-(r,c+1)`, vertikal `(r,c)-(r+1,c)`.

---

## 3. Dateiformat einlesen

- Robust: erste Zeile `n m`; danach `n` Zeilen mit `m` Zeichen:
  - `'0'..'3'` = Zahl
  - `'.'` oder `' '` = leer
- Parser tolerant für Spaces.

---

## 4. Serieller Solver (Backtracking + Constraints)

- Kanten ON/OFF in guter Reihenfolge entscheiden.
- Regeln pro Schritt:
  1) **Zellen**: `onCount > clue` oder `onCount + undecided < clue` → verwerfen.
  2) **Punkte**: Grad > 2 oder Grad 1 ohne offene Kanten → verwerfen.
  3) **Frühe Zyklusprüfung**: keine vorzeitigen Teilzyklen.
- Pseudocode:
  ```
  solve(i):
      if i == E: if valid cycle -> speichern
      try OFF -> if ok recurse
      try ON  -> if ok recurse
  ```
- Heuristik: Kanten nahe Zahlen zuerst.

---

## 5. Parallelisierung des Suchbaums

- Ab Tiefe `k` Teilbäume als **Tasks** (TBB/OpenMP/std::async) abspalten.
- Jeder Task eigene State-Kopie; nur folgende global teilen:
  - `atomic<bool> stopAfterFirst`
  - `atomic<int> solutionCount` oder `mutex` um Lösungsliste.
- Kurzschluss:
  ```cpp
  if (!findAll && stopAfterFirst.load(relaxed)) return;
  ```
- Thread-Pool/Task-Arena nutzen (z. B. TBB `task_group`), Tiefe dynamisch an Puzzlegröße/Dichte koppeln.

---

## 6. CLI-Modi

- Beispiel: `./slitherlink input.txt --all` oder Standard = erste Lösung.
- Flag `findAllSolutions` steuert Abbruch.

---

## 7. ASCII-Ausgabe

- Zeile Punkte/Horizontalkanten: `+` und `-` (oder ` `).
- Zeile Vertikalkanten/Ziffern: `|` oder ` ` plus Zelleninhalt.
- Danach Koordinatenliste des Zyklus.

---

## 8. Laufzeitmessung

- **Release-Build** (`-O3`, `-DCMAKE_BUILD_TYPE=Release`).
- Instanzen wählen, die Sekunden/Minuten brauchen (z. B. 20×20+).
- Mehrfach messen (5–10 Läufe); Mittelwert/Std-Abw.
- Vergleiche: seriell vs. parallel, unterschiedliche Threadzahlen.

---

## 9. Projektstruktur (Vorschlag)

- `main.cpp`: CLI, Einlesen, Mode, Zeitmessung, Solver-Aufruf, Ausgabe.
- `solver_serial.*`: Baseline Backtracking + Checks.
- `solver_parallel.*`: gleiche API, Tasks ab Tiefe `k` (TBB/OpenMP/async).
- `io.*`: Parser + ASCII-Renderer.

---

## 10. Nächste Schritte (konkret)

1. Parser nach obigem Format implementieren.
2. Serialen Solver mit Constraints + Heuristik bauen.
3. Parallelisierung ab Tiefe `k` (TBB `task_group` oder OpenMP tasks).
4. Release-Build einrichten; Tests mit 10×10/20×20.
5. Benchmarks seriell vs. parallel protokollieren (Speedup-Tabellen/Dia).
