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
- **Recommandé** : ONNX Runtime pour l’inférence YOLOv8 (évite un bug OpenCV 4.6 avec certains modèles ONNX)

Installation typique sous Ubuntu :

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libopencv-dev
```

Pour utiliser **ONNX Runtime** (recommandé si vous avez une erreur `shape_utils.hpp` au chargement du modèle) :

1. Téléchargez le package Linux depuis [Releases ONNX Runtime](https://github.com/microsoft/onnxruntime/releases) (ex. `onnxruntime-linux-x64-1.16.3.tgz`).
2. Extrayez-le dans un répertoire (ex. `$HOME/onnxruntime`).
3. Configurez avec CMake en indiquant ce répertoire :

```bash
cmake -S . -B build -DONNXRUNTIME_ROOT=$HOME/onnxruntime
cmake --build build
```

Sans `ONNXRUNTIME_ROOT`, le projet utilise OpenCV DNN pour l’inférence (peut échouer avec OpenCV 4.6 sur YOLOv8 ; mettre à jour OpenCV ou utiliser ONNX Runtime dans ce cas).

### Génération et compilation avec CMake

Depuis la racine du projet (sans ONNX Runtime) :

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
