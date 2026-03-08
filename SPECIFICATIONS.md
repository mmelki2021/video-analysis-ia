# Spécifications du projet : Analyse vidéo IA (C++ + MOT)

## Objectifs
- Détection et suivi multi-personnes dans une vidéo
- Calcul distance et vitesse
- Export CSV/JSON et vidéo annotée

## Stack technique
- **Langage** : C++17
- **Framework vision** : OpenCV (core + `dnn` + `highgui`)
- **Modèle de détection** : YOLOv8 (format ONNX, fichier `models/yolov8.onnx`)
- **Build** : CMake, exécutable principal `video_analysis`
- **OS cible** : Linux (Ubuntu)

## Architecture logique
- **`Detector`** : charge le modèle ONNX YOLOv8 via OpenCV DNN, retourne une liste de `Detection` (bbox, classe, confiance).
- **`Tracker`** : associe des ID persistants aux personnes (`TrackedObject`) entre les frames (MOT).
- **`Analyzer`** : calcule les distances et vitesses à partir des trajectoires (positions successives des ID).
- **`Visualizer`** : dessine bounding boxes, IDs, stats sur les frames vidéo.
- **`Exporter`** : enregistre les résultats agrégés (par ID) en CSV/JSON.

Entrée principale : chemin vers une vidéo (`./video_analysis <video_path>`).  
Sorties : 
- Fichiers `output/results.csv` et `output/results.json`.
- Fenêtre de visualisation temps réel (ou proche temps réel).

---

## Makadam (phases)

### 1. MVP : Détection + suivi simple, distance en pixels
- **Objectif fonctionnel**
  - Détecter des personnes dans chaque frame.
  - Leur attribuer un ID unique simple et suivre ces IDs d’une frame à l’autre.
  - Calculer une distance « brute » en **pixels** parcourus par ID.
- **Technique**
  - **`Detector`** :
    - Utiliser YOLOv8 ONNX + OpenCV DNN (`cv::dnn::readNetFromONNX`).
    - Prétraitement : `blobFromImage` (normalisation 1/255, `Size(640, 640)`).
    - Post-traitement : parsing des sorties YOLO (cx, cy, w, h, conf, scores classes) + `NMSBoxes`.
    - Filtrer sur la classe « personne » (si labels disponibles) et `confThreshold`.
  - **`Tracker`** (version simple) :
    - Matching frame(t) ↔ frame(t-1) avec une logique naïve (ex. distance IoU max, ou distance euclidienne du centre).
    - Si aucune correspondance satisfaisante, créer un **nouvel ID**.
  - **`Analyzer`** :
    - Stocker dernière position (centre de bbox) par ID.
    - À chaque frame, incrémenter la distance parcourue en pixels :  
      \[ distance\_pixels += \sqrt{\Delta x^2 + \Delta y^2} \]
    - Vitesse instantanée en pixels/frame (ou pixels/seconde si on connaît `fps`).
  - **`Visualizer`** :
    - Dessiner `cv::rectangle` pour chaque bbox.
    - Ajouter texte avec ID + vitesse approx. (`cv::putText`).
- **Contraintes**
  - Simplicité > performance parfaite.
  - Supporter au moins la lecture de vidéos MP4/AVI standard (codec courant).

### 2. Multi-personnes avec MOT (DeepSORT/ByteTrack)
- **Objectif fonctionnel**
  - Améliorer la stabilité du suivi multi-personnes.
  - Réduire les swaps d’ID, pertes et re-créations inutiles.
- **Technique**
  - Intégrer un algorithme MOT plus avancé (DeepSORT/ByteTrack ou équivalent).
  - **`Tracker`** :
    - Implémenter une structure de piste par ID (position, vitesse estimée, score de confiance de la piste).
    - Utiliser un algorithme d’assignation (Hungarian / coût IoU) pour associer détections ↔ pistes.
    - Gérer les pistes perdues (time-out après N frames sans détection).
  - **Option** : implémentation maison simplifiée inspirée d’un MOT existant si intégration directe complexe.
- **Contraintes**
  - Garder une complexité raisonnable pour rester temps réel ou quasi temps réel en CPU.
  - Interface publique du `Tracker` inchangée (`update(detections) -> vector<TrackedObject>`).

### 3. Calibration pixels → mètres
- **Objectif fonctionnel**
  - Convertir les distances en pixels en distances réelles (mètres ou autre unité).
  - Afficher des vitesses en m/s ou km/h.
- **Technique**
  - Approches possibles :
    - **Facteur d’échelle constant** : connaître une distance réelle dans la scène (ex. largeur d’une zone) et en déduire un ratio pixels/mètre.
    - **Homographie / calibration caméra** : si points de calibration disponibles, utiliser `findHomography` ou matrices intrinseques/extrinsèques.
  - **`Analyzer`** :
    - Ajouter un paramètre `pixelsToMeters` ou une fonction de conversion.
    - Convertir la distance cumulée pour chaque ID :  
      \[ distance\_metres = distance\_pixels \times pixelsToMeters \]
    - Vitesse :  
      \[ v = \frac{distance\_metres}{\Delta t} \]  
      avec \(\Delta t = \frac{1}{fps}\) ou basé sur les timestamps vidéo.
- **Contraintes**
  - Permettre une configuration par fichier (ex. `config.json`) ou variable dans le code.
  - Tolérer l’absence de calibration (fallback en pixels).

### 4. Statistiques complètes
- **Objectif fonctionnel**
  - Fournir, par personne (ID) :
    - Distance totale parcourue.
    - Vitesse moyenne / max.
    - Temps total de présence dans la scène.
  - Possibilité de statistiques globales (moyenne sur tous les IDs).
- **Technique**
  - **`Analyzer`** :
    - Conserver pour chaque ID :
      - Distance cumulée.
      - Vitesse instantanée par frame.
      - Nombre de frames / temps de suivi.
    - À la fin de la vidéo (`getResults()`), calculer :
      - `distance_totale`, `vitesse_moyenne`, `vitesse_max`.
    - Structurer `Stats` pour contenir ces champs (distance, vitesse moyenne, vitesse max, temps).
- **Contraintes**
  - Ne pas stocker toutes les frames en mémoire (calcul incrémental).
  - Garder l’interface d’export simple (map `ID -> Stats`).

### 5. Visualisation + export
- **Objectif fonctionnel**
  - Afficher une vidéo annotée en direct.
  - Exporter les résultats pour analyse ultérieure (CSV/JSON).
- **Technique**
  - **`Visualizer`** :
    - Dessiner pour chaque `TrackedObject` :
      - Bbox, ID.
      - Vitesse instantanée (et éventuellement distance cumulée).
    - Option : dessiner une **trajectoire** pour chaque ID (petite polyline des derniers points).
  - **`Exporter`** :
    - CSV :
      - Colonnes typiques : `id, distance, vitesse_moyenne, vitesse_max, temps_presence`.
    - JSON :
      - Objet par ID avec les mêmes champs, structure clé/valeur.
    - Option future : exporter aussi des **séries temporelles** (vitesse par frame) si besoin.
- **Contraintes**
  - Noms de fichiers configurables (`output/results.csv`, `output/results.json` par défaut).
  - Assurer que le dossier `output/` existe ou le créer au besoin.

### 6. Optimisation GPU et temps réel
- **Objectif fonctionnel**
  - Traiter les vidéos en temps réel ou proche temps réel (≥ 20–30 fps) selon la résolution.
- **Technique**
  - Optimisations possibles :
    - Utiliser un backend/target optimisé pour `cv::dnn::Net` :
      - `DNN_BACKEND_OPENCV` + `DNN_TARGET_CPU` (baseline).
      - Si GPU : `DNN_BACKEND_CUDA` + `DNN_TARGET_CUDA` (si OpenCV CUDA compilé).
    - Réduire la résolution d’entrée si nécessaire (downscale avant le blob).
    - Ajuster fréquence de détection (ex. détecter 1 frame sur 2/3 et suivre entre-temps).
  - Profiling :
    - Mesurer temps moyenne par frame (détection + tracking + visualisation).
    - Ajuster seuils et taille d’entrée YOLO pour respecter le budget temps.
- **Contraintes**
  - Rester compatible CPU-only par défaut.
  - Ne pas complexifier excessivement l’API publique (les optimisations restent internes aux classes).

---

Ces spécifications servent de guide pour faire évoluer progressivement le projet tout en gardant une architecture claire et modulaire (`Detector`, `Tracker`, `Analyzer`, `Visualizer`, `Exporter`).
