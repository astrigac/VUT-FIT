#!/usr/bin/env python3

import numpy as np
import re

# Inicializace bodů a středů
assignment_text = """
Zadani c. 392

Metodou K-means clustering zaradte nasledujci body (x,y,z) v 3D prostoru 
(-3, 4, 8) (-8,-6,-4) (-7, 8, 2) ( 5, 7,-8) ( 5,-7, 8) ( 6, 9, 3) (-4, 8, 8) (-9, 6, 7) (-8,-3, 7) (-5, 3,-7) 
do 3 shluku s pocatecnimi stredy 
(-4, 8, 8) (-5, 3,-7) (-9, 6, 7).

Vysledky vypoctu zaokrouhlujte na dve desetinna mista.
"""

# Funkce pro extrakci souřadnic z textu
def extract_coordinates(text):
    pattern = r'\(\s*([-\d]+)\s*,\s*([-\d]+)\s*,\s*([-\d]+)\s*\)'
    return np.array([tuple(map(int, match)) for match in re.findall(pattern, text)])

# Funkce pro výpočet Euklidovské vzdálenosti
def euclidean_distance(x, y):
    return np.sqrt(np.sum((x - y) ** 2))

def print_clusters(clusters, centers, iteration=None):
    if iteration is not None:
        print(f"Iteracia c. {iteration}")
    else:
        print("VYSLEDOK:")
    for i, center in enumerate(centers):
        # Formátování středu
        center_str = "(" + ", ".join(f"{coord:.2f}" for coord in center) + ")"
        print(f"Shluk {i + 1}:")
        print(f"    Stred: {center_str}")
        # Formátování bodů
        points_str = " ".join("(" + ", ".join(str(int(coord)) for coord in point) + ")" for point in clusters[i])
        print(f"    Body: {points_str}")
    print("\n")  # Přidá prázdnou řádku pro lepší oddělení iterací

# Definice funkce pro provedení jedné iterace K-means
def k_means_iteration(points, centers):
    clusters = {i: [] for i in range(len(centers))}
    for p in points:
        distances = [euclidean_distance(p, center) for center in centers]
        nearest_center_idx = np.argmin(distances)
        clusters[nearest_center_idx].append(p)
    new_centers = []
    for i in range(len(centers)):
        if len(clusters[i]) > 0:  # Zabránění dělení nulou pokud by byl nějaký shluk prázdný
            new_centers.append(np.mean(clusters[i], axis=0))
        else:
            new_centers.append(centers[i])  # Ponecháme starý střed pokud není žádný bod
    return clusters, new_centers

if __name__ == "__main__":
    print(f"{assignment_text}")
    parts = re.split(r'(?<=\))\s*\.\s*Vysledky', assignment_text)
    points_text = re.split(r'do \d+ shluku s pocatecnimi stredy', parts[0])[0]
    centers_text = re.split(r'do \d+ shluku s pocatecnimi stredy', parts[0])[1]

    points = extract_coordinates(points_text)
    centers = extract_coordinates(centers_text)
    
    # Iterační proces
    iterations = 0
    while True:
        new_clusters, new_centers = k_means_iteration(points, centers)
        if np.allclose(new_centers, centers, atol=1e-4):  # Kontrola, zda se středy změnily
            print_clusters(new_clusters, centers)
            break
        else:
            print_clusters(new_clusters, centers, iterations + 1)
            centers = np.array(new_centers)
            iterations += 1
