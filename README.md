# Spécifications du projet : Analyse vidéo IA (C++ + MOT)

## Objectifs
- Détection et suivi multi-personnes dans une vidéo
- Calcul distance et vitesse
- Export CSV/JSON et vidéo annotée

## Makadam (phases)
1. MVP : Détection + suivi simple, distance en pixels
2. Multi-personnes avec MOT (DeepSORT/ByteTrack)
3. Calibration pixels->mètres
4. Statistiques complètes
5. Visualisation + export
6. Optimisation GPU et temps réel

## Build du projet

### Prérequis
- CMake ≥ 3.10
- Compilateur C++ (g++/clang++) compatible C++17
- OpenCV (paquet `libopencv-dev` sous Ubuntu/Debian)

Installation typique sous Ubuntu :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libopencv-dev
```

### Génération et compilation avec CMake

Depuis la racine du projet :

```bash
cmake -S . -B build
cmake --build build
```

L’exécutable principal sera généré dans le dossier `build` sous le nom `video_analysis`.

### Exécution

Depuis la racine du projet :

```bash
./build/video_analysis
```
