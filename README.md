# TP_SE rayane bouafia G3
# Q1: Quelles sont les structures de données à utiliser ?


.Matrices B, C et A : Vous avez déclaré des matrices pour stocker les valeurs de B, C et A.
.Tampon T : Vous avez utilisé une structure T pour le tampon.

# Q2: Comment allez-vous protéger l'accès à ces données?

Vous avez utilisé des mécanismes de synchronisation pour protéger l'accès concurrent aux données partagées :

Mutex (pthread_mutex_t mutex) : Vous utilisez un mutex pour protéger l'accès au tampon (buffer) et à la matrice résultante (A).

Sémaphores (sem_t empty et sem_t full) : Vous utilisez des sémaphores pour contrôler le nombre d'éléments dans le tampon.
# Q3: Quels sont les risques?
Condition de course (Race Condition) : Il est crucial de garantir que les threads producteurs et consommateurs accèdent de manière synchronisée au tampon et à la matrice résultante. Sinon, cela peut entraîner des résultats incorrects.

Blocage (Deadlock) : Assurez-vous d'éviter les situations de blocage en utilisant les sémaphores et les mutex de manière appropriée.

Dépassement de tampon (Buffer Overflow) : Il est important de gérer correctement la taille du tampon (N) pour éviter les dépassements de tampon.


