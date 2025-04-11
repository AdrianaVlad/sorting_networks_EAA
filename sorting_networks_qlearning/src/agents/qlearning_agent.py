import random
import numpy as np

class QLearningAgent:
    def __init__(self, env, epsilon=0.1, learning_rate=0.01, discount=0.99):
        self.env = env
        self.epsilon = epsilon # probabilitatea de explorare - pt strategia epsilon-greedy
        self.learning_rate = learning_rate # rata inv
        self.discount = discount # gamma - fact de discount
        self.q_table = {}  # Q-table in forma: {(state, action): valoare}
        self.action_space = self.define_action_space()

    def define_action_space(self):
        n = self.env.n # nr intrari in ret de sortare
        actions = []
        # acțiuni de adaugare pt fiecare pereche (i, j); genereaza toate comb posibile de indici i si j
        for i in range(n):
            for j in range(i+1, n):
                actions.append(('add', (i, j)))
        # print("9. agents qlearning_agents define action space")
        return actions

    def get_state_key(self, state):
        # print("10. agents qlearning_agents define state key")
        return tuple(state)

    def select_action(self, state):
        # select o acțiune folosind strategia epsilon-greedy
        state_key = self.get_state_key(state) # obtine cheia hash-uibila pt starea curenta

        if random.random() < self.epsilon: # cu probabilitate epsilon se alege o actiune aleatoare => explorare
            # print("10. agents qlearning_agents select an action")
            return random.choice(self.action_space)

        # cu prob 1 - epsilon, alege actiunea cu valoarea Q maxima (exploatare)
        # calc valorile Q pentru toate actiunile posibile in starea curenta
        q_values = [self.q_table.get((state_key, action), 0) for action in self.action_space]

        max_value = max(q_values) # geseste val q maxima

        # indentif valorile care au valoarea Q maxima
        best_actions = [a for a, q in zip(self.action_space, q_values) if q == max_value]
        # print("10. agents qlearning_agents select best action")
        return random.choice(best_actions)

    def update(self, state, action, reward, next_state, done):
        # actualizeaza valoarea Q pentru perechea (stare, actiune) folosind ecuația bellman
        state_key = self.get_state_key(state) # cheia pt starea curenta
        next_state_key = self.get_state_key(next_state) # cheia pt starea urm
        current_q = self.q_table.get((state_key, action), 0) # val q curenta pt perechea (stare, actiune), sau 0 daca nu exista
        future_qs = [self.q_table.get((next_state_key, a), 0) for a in self.action_space] # valorile q pentru toate actiunile posibile in starea urmatoare
        max_future_q = max(future_qs) if future_qs else 0 # obt val q max in starea urmat
        new_q = current_q + self.learning_rate * (reward + self.discount * max_future_q - current_q) # ecuatia ql
        self.q_table[(state_key, action)] = new_q # actualiz val in tabel
        # print("11. agents qlearning_agents update")

    def save_checkpoint(self, filename):
        import pickle
        with open(filename, "wb") as f:
            pickle.dump(self.q_table, f)
        # print("12. agents qlearning_agents save checkpoint")

    def load_checkpoint(self, filename):
        import pickle
        with open(filename, "rb") as f:
            self.q_table = pickle.load(f)
        # print("13. agents qlearning_agents load checkpoint")
