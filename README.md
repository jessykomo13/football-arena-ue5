# Football Arena UE5 ⚽

Nouveau projet **Unreal Engine 5**, totalement indépendant de Foot Run 3D (Unity).

## Ce que contient la V0.1

- Terrain de football 3D construit au lancement en C++.
- Caméra 3D qui suit le joueur bleu, contrôle clavier et commandes tactiles de prototype.
- 4 joueurs bleus contre 4 joueurs rouges, avec course et déplacement IA simples.
- Ballon mobile, possession, tirs en hauteur, score, remises en jeu.
- Deux gardiens qui se déplacent et essaient d'arrêter les tirs.
- Interface de score et instructions à l'écran.

**Important :** les joueurs sont actuellement des **personnages 3D provisoires assemblés à partir de formes simples**. Ce ne sont **pas** encore des personnages avec squelette (Skeletal Mesh), animation professionnelle, matchmaking, clubs ni multijoueur. Il faut tester et améliorer cette base avant d'ajouter ces fonctionnalités.

## Ouvrir le projet sur ordinateur

1. Installer **Unreal Engine 5.5 ou plus récent** avec le support C++ et **Visual Studio 2022** (Desktop development with C++ + composants Unreal) sous Windows.
2. Cloner ou télécharger ce dépôt, puis ouvrir `FootballArena.uproject`.
3. Accepter la génération des fichiers Visual Studio et la compilation du module C++.
4. Ouvrir la carte du projet. La scène est construite **automatiquement au lancement de Play**, donc la carte de départ peut paraître vide dans l'éditeur.
5. Cliquer sur **Play** pour tester.

### Commandes PC
- **ZQSD / WASD / flèches** : déplacer le joueur.
- **Espace** : tirer quand le joueur possède le ballon.
- Le tir vise un coin supérieur du but si le joueur est orienté vers l'avant.

### Commandes tactiles (prototype)
- Glisser dans la moitié gauche de l'écran : déplacement.
- Toucher la moitié droite : tir.

## Architecture

- `Source/FootballArena/ArenaMatch.*` : terrain, caméra, joueurs, ballon et simulation.
- `Source/FootballArena/ArenaHUD.*` : interface de score.
- `Source/FootballArena/ArenaGameMode.*` : démarrage de la partie.
- `Config/DefaultEngine.ini` : paramètres de carte et mode de jeu.

## À développer ensuite

1. Valider l'ouverture et la compilation dans Unreal.
2. Remplacer les formes par des personnages 3D **Skeletal Mesh** et de vraies animations (course, passe, tir, plongeon).
3. Améliorer conduite de balle, gardiens, IA et niveaux de difficulté.
4. Ajouter un menu, les matchs, les équipes et l'économie du jeu.
5. Adapter les performances et les contrôles mobile, puis préparer un vrai build iOS/Android.

## Mobile / GitHub

Ce dépôt est un **projet Unreal**, et **GitHub Pages ne peut pas exécuter directement ce jeu dans Safari**. Pour une application iPhone, il faudra une chaîne de compilation iOS, une signature Apple et des tests sur appareil. Le fait de publier du code ici ne produit pas automatiquement une application mobile.

## État
Prototype source V0.1, **non compilé et non testé dans l'éditeur Unreal dans cet environnement**. Les fichiers constituent une base de développement, pas une version finale.
