def state_to_vector(state, n):
    """
    Convertește starea (lista comparatoarelor) într-un vector de dimensiune n*(n-1)/2.
    Fiecare poziție din vector este 1 dacă comparatorul corespunzător există, altfel 0.
    """
    vector = []
    presence = {comp: 1 for comp in state}
    for i in range(n):
        for j in range(i+1, n):
            vector.append(presence.get((i, j), 0))
    print("30")
    return vector
