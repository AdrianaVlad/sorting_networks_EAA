import numpy as np
from .network import ComparatorNetwork
from .evaluation import evaluate_network

class SortingNetworkEnv:
    def __init__(self, n, max_steps=50):
        self.n = n # nr intr ret sort
        self.max_steps = max_steps
        self.network = ComparatorNetwork(n) # instanta a retelei de comp
        self.current_step = 0

    def reset(self): # reset ret, si contor
        self.network.reset()
        self.current_step = 0
        # print("21. environment simulation reset")
        return self.get_state()

    def step(self, action):
        # actiune ca un tuplu de forma ('actiune', (i, j))
        action_type, params = action
        if action_type == 'add':
            i, j = params
            self.network.add_comparator(i, j)
        elif action_type == 'remove':
            index = params
            self.network.remove_comparator(index)
        # Se pot adăuga și alte tipuri de acțiuni, ex. reordonare, modificare

        self.current_step += 1
        state = self.get_state() # obtine noua stare a mediului
        # flag 'done' daca s-au atins maximul de pași sau daca rețeaua este valida (det daca simularea s-a terminat)
        done = self.current_step >= self.max_steps or evaluate_network(self.network, self.n)
        reward = self.compute_reward(done)
        info = {}
        # print("22. environment simulation step")
        return state, reward, done, info

    def get_state(self):
        # reprezentarea starii -> lista de comparatoare
        # print("23. environment simulation state")
        return self.network.comparators

    def compute_reward(self, done):
        # daca rețeaua este sortata, oferim o recompensa pozitiva proporționala cu simplitatea (nr de comparatoare)
        is_valid = evaluate_network(self.network, self.n)
        # print("24.0. environment simulation reward")
        if is_valid:
            # print("24.1. environment simulation reward")
            return 100 - len(self.network.comparators)
        else:
            # print("24.1. environment simulation reward")
            return -1